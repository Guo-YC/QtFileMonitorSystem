#ifndef __KEDITDIALOG_H_
#define __KEDITDIALOG_H_

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>

class KEditDialog : public QDialog
{
    Q_OBJECT

public:
    KEditDialog(QWidget* parent = nullptr);
    ~KEditDialog() = default;
    void setPath(const QString& path);
    QString getPath() const;
    void setEventsState(const QVector<bool>& eventsState);
    QVector<bool> getEventsState() const;
    void setIncludeExtensions(const QStringList& extensions);
    QStringList getIncludeExtensions() const;
    void setExcludeExtensions(const QStringList& extensions);
    QStringList getExcludeExtensions() const;
    void setIncludeFiles(const QStringList& files);  // 新增方法
    QStringList getIncludeFiles() const;             // 新增方法
    void setExcludeFiles(const QStringList& files);  // 新增方法
    QStringList getExcludeFiles() const;             // 新增方法

private slots:
    void onAddIncludeExtension();
    void onRemoveIncludeExtension();
    void onAddExcludeExtension();
    void onRemoveExcludeExtension();
    void onAddIncludeFile();  // 新增槽函数
    void onRemoveIncludeFile();  // 新增槽函数
    void onAddExcludeFile();  // 新增槽函数
    void onRemoveExcludeFile();  // 新增槽函数

private:
    QLineEdit* m_pathEdit;
    QCheckBox* m_newFileCheckBox;
    QCheckBox* m_modifyCheckBox;
    QCheckBox* m_deleteCheckBox;
    QCheckBox* m_renameCheckBox;
    QListWidget* m_includeExtensionsList;
    QListWidget* m_excludeExtensionsList;
    QLineEdit* m_includeExtensionEdit;
    QLineEdit* m_excludeExtensionEdit;
    QPushButton* m_addIncludeButton;
    QPushButton* m_removeIncludeButton;
    QPushButton* m_addExcludeButton;
    QPushButton* m_removeExcludeButton;
    QListWidget* m_includeFilesList;  // 新增成员变量
    QListWidget* m_excludeFilesList;  // 新增成员变量
    QLineEdit* m_includeFileEdit;     // 新增成员变量
    QLineEdit* m_excludeFileEdit;     // 新增成员变量
    QPushButton* m_addIncludeFileButton;  // 新增成员变量
    QPushButton* m_removeIncludeFileButton;  // 新增成员变量
    QPushButton* m_addExcludeFileButton;  // 新增成员变量
    QPushButton* m_removeExcludeFileButton;  // 新增成员变量
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
};

#endif // __KEDITDIALOG_H_
