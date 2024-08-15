#include "kmainwindow.h"
#include "ui_kmainwindow.h"
#include "klogmanager.h"
#include "kdirectorymodel.h"
#include "keditdialog.h"
#include "kdirectorymodelexporter.h"
#include "klogdatabasehandler.h"
#include "kmonitoringcontroller.h"
#include "kdirectorytableview.h"
#include "kdirectoryentry.h"
#include "logworker.h"

#include <QTableView>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QHeaderView>
#include <QTextStream>
#include <QDateTime>
#include <QApplication>
#include <QCloseEvent>

KMainWindow::KMainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::KMainWindow())
    , m_trayIcon(new QSystemTrayIcon(this))
    , m_logTextBrowser(new QTextBrowser(this))
    , m_activityLogTextBrowser(new QTextBrowser(this))
    , m_directoryTableView(new KDirectoryTableView(this))
    , m_directoryModel(new KDirectoryModel(this))
    , m_tabWidget(new QTabWidget(this))            
    , m_monitoringController(new KMonitoringController(this))
    , m_quickAddEdit(new QLineEdit(this)) // 初始化快速添加的编辑框    
    , m_addButton(new QPushButton(QString::fromLocal8Bit("新增目录"), this)) // 初始化添加按钮
    , m_logWorker(new LogWorker(m_logTextBrowser,this))  // 实例化 LogWorker
    , m_logThread(new QThread(this))                // 创建 QThread 实例
    , m_showMessage(false)
{
    m_ui->setupUi(this);
    setupUi();
    setupMenuBar();
    setupLayout();
    setupTabs();
    setupLogControls();
    setupSearchLogControls();
    setupQuickAddLayout(); // 新增调用快速添加布局的函数


    // 将 logWorker 移动到子线程
    m_logWorker->moveToThread(m_logThread);

    // 启动子线程
    m_logThread->start();

    // 确保在主线程关闭时停止子线程
    connect(this, &QMainWindow::destroyed, m_logThread, &QThread::quit);
    connect(m_logThread, &QThread::finished, m_logWorker, &QObject::deleteLater);
    connect(m_logThread, &QThread::finished, m_logThread, &QObject::deleteLater);

    connect(m_monitoringController, &KMonitoringController::fileAdded, this, &KMainWindow::onFileAdded);
    connect(m_monitoringController, &KMonitoringController::fileRemoved, this, &KMainWindow::onFileRemoved);
    connect(m_monitoringController, &KMonitoringController::fileModified, this, &KMainWindow::onFileModified);
    connect(m_monitoringController, &KMonitoringController::fileRenamed, this, &KMainWindow::onFileRenamed);
    connect(&KLogManager::getInstance(), &KLogManager::logUpdated, this, &KMainWindow::updateActivityLog);

    connect(m_directoryTableView, &KDirectoryTableView::pauseMonitoring, this, &KMainWindow::onPauseMonitoring);
    connect(m_directoryTableView, &KDirectoryTableView::resumeMonitoring, this, &KMainWindow::onResumeMonitoring);
    connect(m_directoryTableView, &KDirectoryTableView::addDirectory, this, &KMainWindow::onAddDirectory);
    connect(m_directoryTableView, &KDirectoryTableView::editDirectory, this, &KMainWindow::onEditDirectory);
    connect(m_directoryTableView, &KDirectoryTableView::editRequested, this, &KMainWindow::onEditRequested);
    connect(m_directoryTableView, &KDirectoryTableView::deleteRequested, this, &KMainWindow::onDeleteRequested);
    
    // 连接浏览按钮和添加按钮的信号和槽    
    connect(m_addButton, &QPushButton::clicked, this, &KMainWindow::onQuickAddDirectory);


    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &KMainWindow::updateDateTimeEdits);
    m_timer->start(10000);  // 设置计时器每60秒触发一次

    m_notificationTimer = new QTimer(this);
    m_notificationTimer->setInterval(1000);  // 每秒触发
    connect(m_notificationTimer, &QTimer::timeout, this, &KMainWindow::showAccumulatedNotifications);
    m_notificationTimer->start();
}

KMainWindow::~KMainWindow()
{
    if (m_logThread->isRunning())
    {
        m_logThread->quit();
        m_logThread->wait();
    }

    delete m_timer;
    delete m_notificationTimer;
    
    delete m_ui;    
}


void KMainWindow::setupUi()
{
    setWindowTitle(QString::fromLocal8Bit("Qt文件监听器"));
    m_trayIcon->setIcon(QIcon(":/image/jianshi32.png"));
    setWindowIcon(QIcon(":/image/jianshi32.png"));
    QMenu* trayIconMenu = new QMenu(this);
    QAction* quitAction = trayIconMenu->addAction(QString::fromLocal8Bit("退出"));
    QAction* restoreAction = trayIconMenu->addAction(QString::fromLocal8Bit("主界面"));

    (void)connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    (void)connect(restoreAction, &QAction::triggered, this, &KMainWindow::onRestoreActivated);

    m_trayIcon->setContextMenu(trayIconMenu);
    m_trayIcon->setToolTip(QString::fromLocal8Bit("Qt文件监听器"));

    m_trayIcon->show();
    // 连接托盘图标的双击事件到槽函数
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &KMainWindow::onTrayIconActivated);

    m_directoryTableView->setModel(m_directoryModel);
    m_directoryTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_directoryTableView->setColumnWidth(0, 400);
    m_directoryTableView->setColumnWidth(1, 300);
    //m_directoryTableView->setColumnWidth(2, 150);

    QHeaderView* header = m_directoryTableView->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Interactive);
    header->setSectionResizeMode(1, QHeaderView::Interactive);
    header->setSectionResizeMode(2, QHeaderView::Stretch);

    QVBoxLayout* layout = new QVBoxLayout(this);
    
    layout->addWidget(m_directoryTableView);

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    resize(1200, 800);
}

void KMainWindow::setupMenuBar()
{
    QMenuBar* menuBar = this->menuBar();
    //QMenu* fileMenu = menuBar->addMenu(QString::fromLocal8Bit("文件"));
    QMenu* configMenu = menuBar->addMenu(QString::fromLocal8Bit("配置管理"));

    
    QAction* importConfigAction = new QAction(QString::fromLocal8Bit("导入配置"), this);
    QAction* exportConfigAction = new QAction(QString::fromLocal8Bit("导出配置"), this);
    configMenu->addAction(importConfigAction);
    configMenu->addAction(exportConfigAction);
    connect(importConfigAction, &QAction::triggered, this, &KMainWindow::onImportDirectoryEntries);
    connect(exportConfigAction, &QAction::triggered, this, &KMainWindow::onExportDirectoryEntries);

}

void KMainWindow::setupLayout()
{
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // 将目录表视图添加到主布局中，并设置占比
    mainLayout->addWidget(m_directoryTableView, 2);  // 设置 m_directoryTableView 占比为1

    // 创建占位符 widget 或者其他控件，并设置占比
    mainLayout->addWidget(m_tabWidget, 3);  // 设置 m_tabWidget 的占比为3

    setCentralWidget(centralWidget);
}



void KMainWindow::setupTabs()
{
    QWidget* logTab = new QWidget(this);
    QVBoxLayout* logTabLayout = new QVBoxLayout(logTab);
    logTabLayout->addWidget(m_logTextBrowser);
    m_tabWidget->addTab(logTab, QString::fromLocal8Bit("监听日志"));

    QWidget* searchLogTab = new QWidget(this);
    QVBoxLayout* searchLogTabLayout = new QVBoxLayout(searchLogTab);    
    m_tabWidget->addTab(searchLogTab, QString::fromLocal8Bit("日志查询"));

    QWidget* activityLogTab = new QWidget(this);
    QVBoxLayout* activityLogTabLayout = new QVBoxLayout(activityLogTab);
    activityLogTabLayout->addWidget(m_activityLogTextBrowser);
    m_tabWidget->addTab(activityLogTab, QString::fromLocal8Bit("活动日志"));
}

void KMainWindow::setupLogControls()
{
    QWidget* logControlsWidget = new QWidget(this);
    QHBoxLayout* logControlsLayout = new QHBoxLayout(logControlsWidget);


    m_saveLogButton = new QPushButton(QString::fromLocal8Bit("保存日志"), this);
    connect(m_saveLogButton, &QPushButton::clicked, this, &KMainWindow::saveLogToFile);
    logControlsLayout->addWidget(m_saveLogButton);

    QWidget* logTab = m_tabWidget->widget(0);
    QVBoxLayout* logTabLayout = static_cast<QVBoxLayout*>(logTab->layout());
    logTabLayout->insertWidget(0, logControlsWidget);
}


void KMainWindow::setupSearchLogControls()
{
    QWidget* searchLogControlsWidget = new QWidget(this);
    QVBoxLayout* searchLogControlsLayout = new QVBoxLayout(searchLogControlsWidget);

    // 时间范围选择部分
    QHBoxLayout* timeSelectionLayout = new QHBoxLayout(this);

    timeSelectionLayout->addWidget(new QLabel(QString::fromLocal8Bit("开始时间：")));
    m_startTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime().addDays(-1), this);
    m_startTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss"); // 设置显示格式
    m_startTimeEdit->setCalendarPopup(false); // 禁用日历弹出窗口
    m_startTimeEdit->setWrapping(true); // 允许时间上下键调节
    m_startTimeEdit->setStyleSheet("QDateTimeEdit::drop-down { width: 0px; }"); // 移除下拉框箭头
    timeSelectionLayout->addWidget(m_startTimeEdit);

    timeSelectionLayout->addWidget(new QLabel(QString::fromLocal8Bit("结束时间："),this));
    m_endTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    m_endTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss"); // 设置显示格式
    m_endTimeEdit->setCalendarPopup(false); // 禁用日历弹出窗口
    m_endTimeEdit->setWrapping(true); // 允许时间上下键调节
    m_endTimeEdit->setStyleSheet("QDateTimeEdit::drop-down { width: 0px; }"); // 移除下拉框箭头
    timeSelectionLayout->addWidget(m_endTimeEdit);

    m_queryButton = new QPushButton(QString::fromLocal8Bit("按范围查询"), this);
    connect(m_queryButton, &QPushButton::clicked, this, &KMainWindow::onQueryButtonClicked);
    timeSelectionLayout->addWidget(m_queryButton);

    searchLogControlsLayout->addLayout(timeSelectionLayout);

    // 关键字查询部分
    QHBoxLayout* keywordLayout = new QHBoxLayout(this);

    keywordLayout->addWidget(new QLabel(QString::fromLocal8Bit("关键字："),this));
    m_searchEdit = new QLineEdit(this);
    keywordLayout->addWidget(m_searchEdit);

    m_searchButton = new QPushButton(QString::fromLocal8Bit("按关键字搜索"), this);
    connect(m_searchButton, &QPushButton::clicked, this, &KMainWindow::onSearchButtonClicked);
    keywordLayout->addWidget(m_searchButton);

    searchLogControlsLayout->addLayout(keywordLayout);

    // 初始化日志结果表格视图和模型
    m_logTableView = new QTableView(this);
    m_logTableModel = new QStandardItemModel(0, 3, this);
    m_logTableModel->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("时间") << QString::fromLocal8Bit("事件类型") << QString::fromLocal8Bit("消息内容"));
    m_logTableView->setModel(m_logTableModel);
    m_logTableView->setSortingEnabled(true);  // 启用排序功能

    m_logTableView->setColumnWidth(0, 200);
    QHeaderView* header = m_logTableView->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Fixed); // 第一列：固定宽度
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents); // 第二列：根据内容调整
    header->setSectionResizeMode(2, QHeaderView::Stretch); // 第三列：自动扩展以展示全部内容

    searchLogControlsLayout->addWidget(m_logTableView);

    QWidget* searchLogTab = m_tabWidget->widget(1);
    QVBoxLayout* searchLogTabLayout = static_cast<QVBoxLayout*>(searchLogTab->layout());
    searchLogTabLayout->addWidget(searchLogControlsWidget);

    connect(&KLogManager::getInstance(), &KLogManager::queryCompleted, this, &KMainWindow::onQueryCompleted);
    connect(&KLogManager::getInstance(), &KLogManager::searchCompleted, this, &KMainWindow::onSearchCompleted);
}








void KMainWindow::setupQuickAddLayout()
{
    QWidget* quickAddWidget = new QWidget(this);
    QHBoxLayout* quickAddLayout = new QHBoxLayout(quickAddWidget);
    m_quickAddEdit->setPlaceholderText(QString::fromLocal8Bit("可输入目录路径快速添加"));
    quickAddLayout->addWidget(new QLabel(QString::fromLocal8Bit("快速新增："),this));
    quickAddLayout->addWidget(m_quickAddEdit);    
    quickAddLayout->addWidget(m_addButton);

    // 获取当前的中央布局并添加新的快速添加布局到顶部
    QVBoxLayout* centralLayout = qobject_cast<QVBoxLayout*>(centralWidget()->layout());
    if (centralLayout)
    {
        centralLayout->insertWidget(0, quickAddWidget);
    }
}


void KMainWindow::onSearchButtonClicked()
{
    QString keyword = m_searchEdit->text();  // 获取用户输入的关键字
    if (!keyword.isEmpty())
    {
        // 设置鼠标样式为加载中
        QApplication::setOverrideCursor(Qt::WaitCursor);
        m_searchButton->setText(QString::fromLocal8Bit("查询中..."));
        // 延迟执行查询操作，确保UI更新先执行
        QTimer::singleShot(100, this, [this, keyword]() {
            KLogManager::getInstance().searchLogs(keyword);  // 使用关键字进行搜索
            });
        
    }
    else
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("关键字为空"), QString::fromLocal8Bit("请输入要搜索的关键字"));
    }
}



void KMainWindow::onQueryButtonClicked()
{
    QDateTime startTime = m_startTimeEdit->dateTime();  // 获取开始时间
    QDateTime endTime = m_endTimeEdit->dateTime();      // 获取结束时间

    if (startTime > endTime)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("时间范围错误"), QString::fromLocal8Bit("开始时间不能晚于结束时间"));
        return;
    }

    // 设置鼠标样式为加载中
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_queryButton->setText(QString::fromLocal8Bit("查询中..."));
    // 延迟执行查询操作，确保UI更新先执行
    QTimer::singleShot(100, this, [this, startTime, endTime]() {
        KLogManager::getInstance().queryLogs(startTime, endTime);  // 使用时间范围进行查询
        }); 
}


void KMainWindow::onQueryCompleted(const QList<sqlDate>& results)
{
    // 清空之前的数据
    m_logTableModel->removeRows(0, m_logTableModel->rowCount());

    // 添加新数据到表格视图
    for (const sqlDate& log : results)
    {
        QList<QStandardItem*> items;
        items.append(new QStandardItem(log.time));      // 时间
        items.append(new QStandardItem(log.type));      // 事件类型
        items.append(new QStandardItem(log.message));   // 消息内容

        m_logTableModel->appendRow(items);

    }
    // 恢复鼠标样式
    m_queryButton->setText(QString::fromLocal8Bit("按范围查询"));
    QApplication::restoreOverrideCursor();
}

void KMainWindow::onSearchCompleted(const QList<sqlDate>& results)
{
    // 清空之前的数据
    m_logTableModel->removeRows(0, m_logTableModel->rowCount());

    // 添加新数据到表格视图
    for (const sqlDate& log : results)
    {
        QList<QStandardItem*> items;
        items.append(new QStandardItem(log.time));      // 时间
        items.append(new QStandardItem(log.type));      // 事件类型
        items.append(new QStandardItem(log.message));   // 消息内容

        m_logTableModel->appendRow(items);
    }
    // 恢复鼠标样式
    QApplication::restoreOverrideCursor();
    m_searchButton->setText(QString::fromLocal8Bit("按关键字查询"));
}


void KMainWindow::onQuickAddDirectory()
{
    QString path;
    if (!m_quickAddEdit->text().isEmpty()) 
    {
        QFileInfo checkDir(m_quickAddEdit->text());
        if (checkDir.exists() && checkDir.isDir()) 
        {
            path = m_quickAddEdit->text();
        }
        else 
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("无效路径"), QString::fromLocal8Bit("请输入有效的目录路径。"));
            return;
        }
    }
    else 
    {
        path = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择目录"), "/", QFileDialog::ShowDirsOnly);
    }
    if (!path.isEmpty())
    {
        m_monitoringController->addDirectory(path);
        KDirectoryEntry entry(path);
        m_directoryModel->addEntry(entry);
        m_quickAddEdit->clear();
    }
}

void KMainWindow::updateLogView(const QList<QString>& logs)
{
}

void KMainWindow::onAddDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择目录"), "/", QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty())
    {
        m_monitoringController->addDirectory(dir);
        KDirectoryEntry entry(dir);
        m_directoryModel->addEntry(entry);
    }
}

void KMainWindow::onRemoveDirectory()
{
    QModelIndex index = m_directoryTableView->currentIndex();
    if (index.isValid())
    {
        QString path = m_directoryModel->data(index, Qt::DisplayRole).toString();
        m_monitoringController->removeDirectory(path);
        m_directoryModel->removeEntry(index.row());
    }
}

void KMainWindow::onPauseMonitoring(const QModelIndex& _index)
{

    int index = _index.row();
    KDirectoryEntry entry = m_directoryModel->getEntry(index);
    QString path = entry.getPath();
    m_monitoringController->pauseMonitoring(path);
    //int index = m_directoryModel->getEntryIndexByPath(path);
    if (index != -1)
    {
        KDirectoryEntry entry = m_directoryModel->getEntry(index);
        entry.setMonitorState(QString::fromLocal8Bit("已暂停监控"));
        m_directoryModel->updateEntry(index, entry);
    }    
}

void KMainWindow::onResumeMonitoring(const QModelIndex& _index)
{
    int index = _index.row();
    KDirectoryEntry entry = m_directoryModel->getEntry(index);
    QString path = entry.getPath();
    m_monitoringController->resumeMonitoring(path);
    //int index = m_directoryModel->getEntryIndexByPath(path);    
    if (index != -1)
    {
        KDirectoryEntry entry = m_directoryModel->getEntry(index);
        entry.setMonitorState(QString::fromLocal8Bit("正在监控"));
        m_directoryModel->updateEntry(index, entry);
    }
}

void KMainWindow::onEditDirectory(const QString& path)
{
    // 编辑目录的实现
}

void KMainWindow::onFileAdded(const QString& path)
{
    QString timeStamp = QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss");
    QString log = QString("<font color=\"green\">") + QString::fromLocal8Bit("新增</font> (") + timeStamp + "): " + path ;
    appendLogToTextBrowser(log);
    //m_trayIcon->showMessage(QString::fromLocal8Bit("文件监控"), QString::fromLocal8Bit("检测到文件新增: ") + path);
    m_pendingNotifications.enqueue(QString::fromLocal8Bit("检测到文件新增: ") + path);
}

void KMainWindow::onFileRemoved(const QString& path)
{
    QString timeStamp = QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss");
    QString log = QString("<font color=\"red\">") + QString::fromLocal8Bit("删除</font> (") + timeStamp + "): " + path;
    appendLogToTextBrowser(log);
    //m_trayIcon->showMessage(QString::fromLocal8Bit("文件监控"), QString::fromLocal8Bit("检测到文件删除: ") + path);
    m_pendingNotifications.enqueue(QString::fromLocal8Bit("检测到文件删除: ") + path);
}

void KMainWindow::onFileModified(const QString& path)
{
    QString timeStamp = QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss");
    QString log = QString("<font color=\"blue\">") + QString::fromLocal8Bit("修改</font> (") + timeStamp + "): " + path;
    appendLogToTextBrowser(log);
    //m_trayIcon->showMessage(QString::fromLocal8Bit("文件监控"), QString::fromLocal8Bit("检测到文件修改: ") + path);
    m_pendingNotifications.enqueue(QString::fromLocal8Bit("检测到文件修改: ") + path);
}

void KMainWindow::onFileRenamed(const QString& oldPath, const QString& newPath)
{
    QString timeStamp = QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss");
    QString log = QString("<font color=\"orange\">") + QString::fromLocal8Bit("重命名</font> (") + timeStamp + "): " + oldPath + QString::fromLocal8Bit(" 到 ") + newPath;
    appendLogToTextBrowser(log);
    //m_trayIcon->showMessage(QString::fromLocal8Bit("文件监控"), QString::fromLocal8Bit("检测到文件重命名: ") + oldPath + QString::fromLocal8Bit(" 到 ") + newPath);
    m_pendingNotifications.enqueue(QString::fromLocal8Bit("检测到文件重命名: ") + oldPath + QString::fromLocal8Bit(" 到 ") + newPath);
}

void KMainWindow::appendLogToTextBrowser(const QString& log)
{
    //m_logTextBrowser->append(log);
    QMetaObject::invokeMethod(m_logWorker, "appendLog", Qt::QueuedConnection, Q_ARG(QString, log));    
//    m_logWorker->appendLog(log);
}


void KMainWindow::updateActivityLog(const QString& log)
{
    m_activityLogTextBrowser->append(log);
}


void KMainWindow::saveLogToFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存日志"), "", QString::fromLocal8Bit("文本文件 (*.txt);;所有文件 (*)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("保存失败"), QString::fromLocal8Bit("无法保存日志到文件"));
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << m_logTextBrowser->toPlainText();
    file.close();
    QMessageBox::information(this, QString::fromLocal8Bit("保存成功"), QString::fromLocal8Bit("日志已成功保存到文件"));
}

void KMainWindow::addDirectoryEntry(const QString& path)
{
    KDirectoryEntry entry(path);
    m_directoryModel->addEntry(entry);
}

void KMainWindow::removeDirectoryEntry(int row)
{
    m_directoryModel->removeEntry(row);
}

void KMainWindow::updateDirectoryEntry(int row, const QString& path, const QString& events, bool subdirectories)
{
    KDirectoryEntry entry(path);
    entry.setEvents(events.split(",").toVector());
    m_directoryModel->updateEntry(row, entry);
}

void KMainWindow::onEditRequested(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    int row = index.row();
    KDirectoryEntry entry = m_directoryModel->getEntry(row);

    KEditDialog editDialog(this);
    editDialog.setPath(entry.getPath());
    editDialog.setEventsState(entry.getEventsState());
    editDialog.setIncludeExtensions(entry.getIncludeExtensions());
    editDialog.setExcludeExtensions(entry.getExcludeExtensions());
    editDialog.setIncludeFiles(entry.getIncludeFiles());
    editDialog.setExcludeFiles(entry.getExcludeFiles());

    if (editDialog.exec() == QDialog::Accepted)
    {
        entry.setPath(editDialog.getPath());
        QVector<bool> eventsState = editDialog.getEventsState();
        for (int i = 0; i < eventsState.size(); ++i)
        {
            entry.setEventStateByIndex(i, eventsState[i]);
        }
        entry.setIncludeExtensions(editDialog.getIncludeExtensions());
        entry.setExcludeExtensions(editDialog.getExcludeExtensions());
        entry.setIncludeFiles(editDialog.getIncludeFiles());
        entry.setExcludeFiles(editDialog.getExcludeFiles());
        m_directoryModel->updateEntry(row, entry);

        // 同步事件状态到监控控制器
        m_monitoringController->setEventsState(entry.getPath(), eventsState);
        m_monitoringController->setIncludeExtensions(entry.getPath(), entry.getIncludeExtensions());
        m_monitoringController->setExcludeExtensions(entry.getPath(), entry.getExcludeExtensions());
        m_monitoringController->setIncludeFiles(entry.getPath(), entry.getIncludeFiles());
        m_monitoringController->setExcludeFiles(entry.getPath(), entry.getExcludeFiles());
    }
}


void KMainWindow::onDeleteRequested(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    int row = index.row();
    KDirectoryEntry entry = m_directoryModel->getEntry(row);
    QString path = entry.getPath();
    m_monitoringController->removeDirectory(path);
    m_directoryModel->removeEntry(index.row());
}



void KMainWindow::onExportDirectoryEntries()
{
    QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("导出目录"), "", QString::fromLocal8Bit("JSON 文件 (*.json);;所有文件 (*)"));
    if (!fileName.isEmpty())
    {
        if (KDirectoryModelExporter::exportToFile(*m_directoryModel, fileName))
        {
            QMessageBox::information(this, QString::fromLocal8Bit("导出成功"), QString::fromLocal8Bit("目录已成功导出到文件"));
        }
        else
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("导出失败"), QString::fromLocal8Bit("无法导出目录到文件"));
        }
    }
}

void KMainWindow::onImportDirectoryEntries()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("导入目录"), "", QString::fromLocal8Bit("JSON 文件 (*.json);;所有文件 (*)"));
    if (!fileName.isEmpty())
    {
        if (KDirectoryModelExporter::importFromFile(*m_directoryModel, fileName))
        {
            // 遍历所有导入的目录条目
            for (int i = 0; i < m_directoryModel->rowCount(); ++i)
            {
                KDirectoryEntry entry = m_directoryModel->getEntry(i);
                QString path = entry.getPath();

                // 开始监控
                m_monitoringController->addDirectory(path);
                m_monitoringController->setEventsState(path, entry.getEventsState());
                m_monitoringController->setIncludeExtensions(path, entry.getIncludeExtensions());
                m_monitoringController->setExcludeExtensions(path, entry.getExcludeExtensions());
                m_monitoringController->setIncludeFiles(path, entry.getIncludeFiles());
                m_monitoringController->setExcludeFiles(path, entry.getExcludeFiles());
            }

            QMessageBox::information(this, QString::fromLocal8Bit("导入成功"), QString::fromLocal8Bit("目录已成功从文件导入并开始监控"));
        }
        else
        {
            QMessageBox::warning(this, QString::fromLocal8Bit("导入失败"), QString::fromLocal8Bit("无法从文件导入目录"));
        }
    }
}
    
// 实现更新时间控件的槽函数
void KMainWindow::updateDateTimeEdits() 
{
    QDateTime current = QDateTime::currentDateTime();
    m_startTimeEdit->setDateTime(current.addDays(-1));  // 更新开始时间为当前时间减去一天
    m_endTimeEdit->setDateTime(current);                // 更新结束时间为当前时间
}

// 添加此函数以处理窗口关闭事件，使其最小化到托盘
void KMainWindow::closeEvent(QCloseEvent* event)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(QString::fromLocal8Bit("Qt文件监听器"));
    msgBox.setText(QString::fromLocal8Bit("您想要关闭还是最小化到系统托盘？"));
    msgBox.addButton(QString::fromLocal8Bit("关闭"), QMessageBox::AcceptRole);
    msgBox.addButton(QString::fromLocal8Bit("最小化"), QMessageBox::RejectRole);
    msgBox.setDefaultButton(QMessageBox::NoButton);

    int ret = msgBox.exec();

    if (ret == QMessageBox::AcceptRole)
    {
        QApplication::quit();  // 用户选择关闭程序
    }
    else
    {
        m_showMessage = true;
        hide();                // 用户选择最小化到托盘
        event->ignore();       // 忽略关闭事件，不关闭程序
    }
}

void KMainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) 
    {
        m_showMessage = false;
        this->showNormal();  // 恢复主窗口
        this->activateWindow();  // 将窗口置于前台
    }
}

void KMainWindow::onRestoreActivated()
{
    m_showMessage = false;
    this->showNormal();  // 恢复主窗口
    this->activateWindow();  // 将窗口置于前台
}

void KMainWindow::showAccumulatedNotifications()
{
    if (!m_pendingNotifications.isEmpty()) 
    {
        int count = m_pendingNotifications.size();
        QString message;

        if (count == 1) 
        {
            message = m_pendingNotifications.dequeue();
        }
        else 
        {
            message = QString::fromLocal8Bit("检测到多个文件变化: ") + QString::number(count) + QString::fromLocal8Bit(" 条消息");
            m_pendingNotifications.clear();  // 清空队列
        }

        if (m_showMessage == true) 
            m_trayIcon->showMessage(QString::fromLocal8Bit("文件监控"), message, QSystemTrayIcon::Information, 1000);
    }
}