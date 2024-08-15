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
    void setIncludeFiles(const QStringList& files);  // ��������
    QStringList getIncludeFiles() const;             // ��������
    void setExcludeFiles(const QStringList& files);  // ��������
    QStringList getExcludeFiles() const;             // ��������

private slots:
    void onAddIncludeExtension();
    void onRemoveIncludeExtension();
    void onAddExcludeExtension();
    void onRemoveExcludeExtension();
    void onAddIncludeFile();  // �����ۺ���
    void onRemoveIncludeFile();  // �����ۺ���
    void onAddExcludeFile();  // �����ۺ���
    void onRemoveExcludeFile();  // �����ۺ���

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
    QListWidget* m_includeFilesList;  // ������Ա����
    QListWidget* m_excludeFilesList;  // ������Ա����
    QLineEdit* m_includeFileEdit;     // ������Ա����
    QLineEdit* m_excludeFileEdit;     // ������Ա����
    QPushButton* m_addIncludeFileButton;  // ������Ա����
    QPushButton* m_removeIncludeFileButton;  // ������Ա����
    QPushButton* m_addExcludeFileButton;  // ������Ա����
    QPushButton* m_removeExcludeFileButton;  // ������Ա����
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
};

#endif // __KEDITDIALOG_H_
