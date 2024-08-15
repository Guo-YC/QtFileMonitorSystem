#include "keditdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include <QGroupBox>
#include <QFileDialog>
KEditDialog::KEditDialog(QWidget* parent)
    : QDialog(parent)
    , m_pathEdit(new QLineEdit(this))
    , m_newFileCheckBox(new QCheckBox(QString::fromLocal8Bit("新建文件"), this))
    , m_modifyCheckBox(new QCheckBox(QString::fromLocal8Bit("修改"), this))
    , m_deleteCheckBox(new QCheckBox(QString::fromLocal8Bit("删除"), this))
    , m_renameCheckBox(new QCheckBox(QString::fromLocal8Bit("重命名"), this))
    , m_includeExtensionsList(new QListWidget(this))
    , m_excludeExtensionsList(new QListWidget(this))
    , m_includeExtensionEdit(new QLineEdit(this))
    , m_excludeExtensionEdit(new QLineEdit(this))
    , m_addIncludeButton(new QPushButton(QString::fromLocal8Bit("添加"), this))
    , m_removeIncludeButton(new QPushButton(QString::fromLocal8Bit("移除"), this))
    , m_addExcludeButton(new QPushButton(QString::fromLocal8Bit("添加"), this))
    , m_removeExcludeButton(new QPushButton(QString::fromLocal8Bit("移除"), this))
    , m_includeFilesList(new QListWidget(this))
    , m_excludeFilesList(new QListWidget(this))
    , m_addIncludeFileButton(new QPushButton(QString::fromLocal8Bit("选择文件"), this))
    , m_removeIncludeFileButton(new QPushButton(QString::fromLocal8Bit("移除"), this))
    , m_addExcludeFileButton(new QPushButton(QString::fromLocal8Bit("选择文件"), this))
    , m_removeExcludeFileButton(new QPushButton(QString::fromLocal8Bit("移除"), this))
    , m_okButton(new QPushButton(QString::fromLocal8Bit("确定"), this))
    , m_cancelButton(new QPushButton(QString::fromLocal8Bit("取消"), this))
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* pathLayout = new QHBoxLayout;
    pathLayout->addWidget(new QLabel(QString::fromLocal8Bit("目录：")));
    pathLayout->addWidget(m_pathEdit);
    mainLayout->addLayout(pathLayout);

    QHBoxLayout* eventsLayout = new QHBoxLayout;
    eventsLayout->addWidget(new QLabel(QString::fromLocal8Bit("监控事件：")));
    eventsLayout->addWidget(m_newFileCheckBox);
    eventsLayout->addWidget(m_modifyCheckBox);
    eventsLayout->addWidget(m_deleteCheckBox);
    eventsLayout->addWidget(m_renameCheckBox);
    mainLayout->addLayout(eventsLayout);

    QVBoxLayout* includeLayout = new QVBoxLayout;
    includeLayout->addWidget(new QLabel(QString::fromLocal8Bit("关注的文件后缀：")));
    QHBoxLayout* includeEditLayout = new QHBoxLayout;
    m_includeExtensionEdit->setPlaceholderText(QString::fromLocal8Bit("例如：.txt。输入后请按添加"));
    includeEditLayout->addWidget(m_includeExtensionEdit);
    includeEditLayout->addWidget(m_addIncludeButton);
    includeLayout->addLayout(includeEditLayout);
    includeLayout->addWidget(m_includeExtensionsList);
    includeLayout->addWidget(m_removeIncludeButton);

    QVBoxLayout* excludeLayout = new QVBoxLayout;
    excludeLayout->addWidget(new QLabel(QString::fromLocal8Bit("忽略的文件后缀：")));
    QHBoxLayout* excludeEditLayout = new QHBoxLayout;
    m_excludeExtensionEdit->setPlaceholderText(QString::fromLocal8Bit("例如：.log。输入后请按添加"));
    excludeEditLayout->addWidget(m_excludeExtensionEdit);
    excludeEditLayout->addWidget(m_addExcludeButton);
    excludeLayout->addLayout(excludeEditLayout);
    excludeLayout->addWidget(m_excludeExtensionsList);
    excludeLayout->addWidget(m_removeExcludeButton);

    QHBoxLayout* extensionsLayout = new QHBoxLayout;
    extensionsLayout->addLayout(includeLayout);
    extensionsLayout->addLayout(excludeLayout);

    QGroupBox* filterGroupBox = new QGroupBox(QString::fromLocal8Bit("文件后缀过滤器"));
    QVBoxLayout* filterGroupBoxLayout = new QVBoxLayout(filterGroupBox);
    filterGroupBoxLayout->addLayout(extensionsLayout);

    mainLayout->addWidget(filterGroupBox);

    QVBoxLayout* includeFileLayout = new QVBoxLayout;
    //includeFileLayout->addWidget(new QLabel(QString::fromLocal8Bit("关注的指定文件：")));
    QHBoxLayout* includeFileEditLayout = new QHBoxLayout;
    //m_includeFileEdit->setPlaceholderText(QString::fromLocal8Bit("添加完整文件路径"));
    //includeFileEditLayout->addWidget(m_includeFileEdit);
    includeFileEditLayout->addWidget(new QLabel(QString::fromLocal8Bit("关注的指定文件：")));
    includeFileEditLayout->addWidget(m_addIncludeFileButton);
    includeFileLayout->addLayout(includeFileEditLayout);
    includeFileLayout->addWidget(m_includeFilesList);
    includeFileLayout->addWidget(m_removeIncludeFileButton);

    QVBoxLayout* excludeFileLayout = new QVBoxLayout;
    //excludeFileLayout->addWidget(new QLabel(QString::fromLocal8Bit("忽略的指定文件：")));
    QHBoxLayout* excludeFileEditLayout = new QHBoxLayout;
    //m_excludeFileEdit->setPlaceholderText(QString::fromLocal8Bit("添加完整文件路径"));
    //excludeFileEditLayout->addWidget(m_excludeFileEdit);
    excludeFileEditLayout->addWidget(new QLabel(QString::fromLocal8Bit("忽略的指定文件：")));
    excludeFileEditLayout->addWidget(m_addExcludeFileButton);
    excludeFileLayout->addLayout(excludeFileEditLayout);
    excludeFileLayout->addWidget(m_excludeFilesList);
    excludeFileLayout->addWidget(m_removeExcludeFileButton);

    QHBoxLayout* filesLayout = new QHBoxLayout;
    filesLayout->addLayout(includeFileLayout);
    filesLayout->addLayout(excludeFileLayout);

    QGroupBox* fileFilterGroupBox = new QGroupBox(QString::fromLocal8Bit("指定文件过滤器"));
    QVBoxLayout* fileFilterGroupBoxLayout = new QVBoxLayout(fileFilterGroupBox);
    fileFilterGroupBoxLayout->addLayout(filesLayout);

    mainLayout->addWidget(fileFilterGroupBox);

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(m_okButton);
    buttonsLayout->addWidget(m_cancelButton);
    mainLayout->addLayout(buttonsLayout);

    resize(700, 500); // 宽度700，高度500

    connect(m_addIncludeButton, &QPushButton::clicked, this, &KEditDialog::onAddIncludeExtension);
    connect(m_removeIncludeButton, &QPushButton::clicked, this, &KEditDialog::onRemoveIncludeExtension);
    connect(m_addExcludeButton, &QPushButton::clicked, this, &KEditDialog::onAddExcludeExtension);
    connect(m_removeExcludeButton, &QPushButton::clicked, this, &KEditDialog::onRemoveExcludeExtension);
    connect(m_addIncludeFileButton, &QPushButton::clicked, this, &KEditDialog::onAddIncludeFile);
    connect(m_removeIncludeFileButton, &QPushButton::clicked, this, &KEditDialog::onRemoveIncludeFile);
    connect(m_addExcludeFileButton, &QPushButton::clicked, this, &KEditDialog::onAddExcludeFile);
    connect(m_removeExcludeFileButton, &QPushButton::clicked, this, &KEditDialog::onRemoveExcludeFile);
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void KEditDialog::setPath(const QString& path)
{
    m_pathEdit->setText(path);
}

QString KEditDialog::getPath() const
{
    return m_pathEdit->text();
}

void KEditDialog::setEventsState(const QVector<bool>& eventsState)
{
    if (eventsState.size() >= 4)
    {
        m_newFileCheckBox->setChecked(eventsState[0]);
        m_modifyCheckBox->setChecked(eventsState[1]);
        m_deleteCheckBox->setChecked(eventsState[2]);
        m_renameCheckBox->setChecked(eventsState[3]);
    }
}

QVector<bool> KEditDialog::getEventsState() const
{
    return QVector<bool>{
        m_newFileCheckBox->isChecked(),
            m_modifyCheckBox->isChecked(),
            m_deleteCheckBox->isChecked(),
            m_renameCheckBox->isChecked()
    };
}

void KEditDialog::setIncludeExtensions(const QStringList& extensions)
{
    m_includeExtensionsList->clear();
    m_includeExtensionsList->addItems(extensions);
}

QStringList KEditDialog::getIncludeExtensions() const
{
    QStringList extensions;
    for (int i = 0; i < m_includeExtensionsList->count(); ++i)
    {
        extensions << m_includeExtensionsList->item(i)->text();
    }
    return extensions;
}

void KEditDialog::setExcludeExtensions(const QStringList& extensions)
{
    m_excludeExtensionsList->clear();
    m_excludeExtensionsList->addItems(extensions);
}

QStringList KEditDialog::getExcludeExtensions() const
{
    QStringList extensions;
    for (int i = 0; i < m_excludeExtensionsList->count(); ++i)
    {
        extensions << m_excludeExtensionsList->item(i)->text();
    }
    return extensions;
}

void KEditDialog::setIncludeFiles(const QStringList& files)
{
    m_includeFilesList->clear();
    m_includeFilesList->addItems(files);
}

QStringList KEditDialog::getIncludeFiles() const
{
    QStringList files;
    for (int i = 0; i < m_includeFilesList->count(); ++i)
    {
        files << m_includeFilesList->item(i)->text();
    }
    return files;
}

void KEditDialog::setExcludeFiles(const QStringList& files)
{
    m_excludeFilesList->clear();
    m_excludeFilesList->addItems(files);
}

QStringList KEditDialog::getExcludeFiles() const
{
    QStringList files;
    for (int i = 0; i < m_excludeFilesList->count(); ++i)
    {
        files << m_excludeFilesList->item(i)->text();
    }
    return files;
}

void KEditDialog::onAddIncludeExtension()
{
    QString extension = m_includeExtensionEdit->text().trimmed();
    if (!extension.isEmpty())
    {
        m_includeExtensionsList->addItem(extension);
        m_includeExtensionEdit->clear();
    }
}

void KEditDialog::onRemoveIncludeExtension()
{
    QListWidgetItem* item = m_includeExtensionsList->currentItem();
    if (item)
    {
        delete m_includeExtensionsList->takeItem(m_includeExtensionsList->row(item));
    }
}

void KEditDialog::onAddExcludeExtension()
{
    QString extension = m_excludeExtensionEdit->text().trimmed();
    if (!extension.isEmpty())
    {
        m_excludeExtensionsList->addItem(extension);
        m_excludeExtensionEdit->clear();
    }
}

void KEditDialog::onRemoveExcludeExtension()
{
    QListWidgetItem* item = m_excludeExtensionsList->currentItem();
    if (item)
    {
        delete m_excludeExtensionsList->takeItem(m_excludeExtensionsList->row(item));
    }
}

void KEditDialog::onAddIncludeFile()
{
    QString file = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择文件"));
    if (!file.isEmpty())
    {
        m_includeFilesList->addItem(file);
    }
}

void KEditDialog::onRemoveIncludeFile()
{
    QListWidgetItem* item = m_includeFilesList->currentItem();
    if (item)
    {
        delete m_includeFilesList->takeItem(m_includeFilesList->row(item));
    }
}

void KEditDialog::onAddExcludeFile()
{
    QString file = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择文件"));
    if (!file.isEmpty())
    {
        m_excludeFilesList->addItem(file);
    }
}

void KEditDialog::onRemoveExcludeFile()
{
    QListWidgetItem* item = m_excludeFilesList->currentItem();
    if (item)
    {
        delete m_excludeFilesList->takeItem(m_excludeFilesList->row(item));
    }
}
