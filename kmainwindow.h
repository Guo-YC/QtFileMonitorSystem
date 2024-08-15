#ifndef __KMAINWINDOW_H_
#define __KMAINWINDOW_H_

#include "kmonitoringcontroller.h"
#include "kdirectorytableview.h"
#include "kdirectorymodel.h"
#include "keditdialog.h"
#include "logworker.h"

#include <QMainWindow>
#include <QTextBrowser>
#include <QStandardItemModel>
#include <QSystemTrayIcon>
#include <QTabWidget>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QTableView>
#include <QQueue>
#include <QThread>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui { class KMainWindow; }
QT_END_NAMESPACE
class LogWorker;
class KDirectoryModel;
class KMonitoringController;
class KDirectoryTableView;
class sqlDate;
class KMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    KMainWindow(QWidget* parent = nullptr);
    ~KMainWindow();

private slots:
    void onSearchButtonClicked();
    void onQueryButtonClicked();
	void onQueryCompleted(const QList<sqlDate>& results);
    void onSearchCompleted(const QList<sqlDate>& results);
    void onAddDirectory();
    void onRemoveDirectory();
    void onPauseMonitoring(const QModelIndex& index);
    void onResumeMonitoring(const QModelIndex& index);
    void onEditDirectory(const QString& path);
    void onFileAdded(const QString& path);
    void onFileRemoved(const QString& path);
    void onFileModified(const QString& path);
    void onFileRenamed(const QString& oldPath, const QString& newPath);
    
    void updateActivityLog(const QString& log);
    void saveLogToFile();
    void onQuickAddDirectory(); // 新增槽函数

private:
    Ui::KMainWindow* m_ui = Q_NULLPTR;
    QSystemTrayIcon* m_trayIcon = Q_NULLPTR;
    QTextBrowser* m_logTextBrowser = Q_NULLPTR;
    QTextBrowser* m_activityLogTextBrowser = Q_NULLPTR;
    KDirectoryTableView* m_directoryTableView = Q_NULLPTR; 
    KDirectoryModel* m_directoryModel = Q_NULLPTR;
    QTabWidget* m_tabWidget = Q_NULLPTR;
    QLineEdit* m_searchEdit = Q_NULLPTR;
    QDateTimeEdit* m_startTimeEdit = Q_NULLPTR;
    QDateTimeEdit* m_endTimeEdit = Q_NULLPTR;
    QPushButton* m_searchButton = Q_NULLPTR;
    QPushButton* m_queryButton = Q_NULLPTR;
    QPushButton* m_saveLogButton = Q_NULLPTR;
    KMonitoringController* m_monitoringController = Q_NULLPTR;
    QLineEdit* m_quickAddEdit = Q_NULLPTR; // 新增快速添加的编辑框    
    QPushButton* m_addButton = Q_NULLPTR; // 新增添加按钮
    QTableView* m_logTableView = Q_NULLPTR;
    QStandardItemModel* m_logTableModel = Q_NULLPTR;
    QTimer* m_timer;
    LogWorker* m_logWorker = Q_NULLPTR;
    QThread* m_logThread = Q_NULLPTR;
    QQueue<QString> m_pendingNotifications;  // 存储待显示的通知
    QTimer* m_notificationTimer;  // 定时器用于合并通知
    bool m_showMessage ;
    void setupUi();
    void setupMenuBar();
    void setupLayout();
    void setupTabs();
    void setupLogControls();
    void setupSearchLogControls();
    void setupQuickAddLayout(); // 新增函数声明
    void updateLogView(const QList<QString>& logs);
    //void appendLogToTextBrowser(const QString& log, const QString& color);
    void addDirectoryEntry(const QString& path);
    void removeDirectoryEntry(int row);
    void updateDirectoryEntry(int row, const QString& path, const QString& events, bool subdirectories);
    void onEditRequested(const QModelIndex& index);
    void onDeleteRequested(const QModelIndex& index);
    void appendLogToTextBrowser(const QString& log);
    void onExportDirectoryEntries();
    void onImportDirectoryEntries();
    void updateDateTimeEdits();
    void closeEvent(QCloseEvent* event) override;
	void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onRestoreActivated();
    void showAccumulatedNotifications();
};

#endif // __KMAINWINDOW_H_
