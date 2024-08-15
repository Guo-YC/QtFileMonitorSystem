#ifndef __KDIRECTORYENTRY_H_
#define __KDIRECTORYENTRY_H_

#include <QString>
#include <QVector>
#include <QStringList>
class KDirectoryEntry
{
public:
    KDirectoryEntry(const QString& path = QString());
    ~KDirectoryEntry();

    QString getPath() const;
    QVector<QString> getEvents() const;
    QString getEventsStr() const;
    bool getEventStateByIndex(int index) const;
    QVector<bool> getEventsState() const;
    QStringList getIncludeExtensions() const;
    QString getIncludeExtensionsStr() const;  
    QString getExcludeExtensionsStr() const;
    QStringList getExcludeExtensions() const;
    QStringList getIncludeFiles() const;  // 新增方法
    QStringList getExcludeFiles() const;  // 新增方法

    void setPath(const QString& path);
    void setEvents(const QVector<QString>& events);
    void setEventStateByIndex(int index, bool state);
    void setMonitorState(const QString& state);
    QString getMonitorState() const;

    void setIncludeExtensions(const QStringList& extensions);
    void setExcludeExtensions(const QStringList& extensions);
    void setIncludeFiles(const QStringList& files);
    void setExcludeFiles(const QStringList& files);    

private:
    void updateEventsStr();

    QString m_path;
    QVector<QString> m_events;
    QVector<bool> m_eventsState{ true, true, true, true };
    QString m_monitorState;
    QString m_eventsStr;
    QStringList m_includeExtensions{};
    QStringList m_excludeExtensions{};
    QStringList m_includeFiles{};  // 新增成员变量
    QStringList m_excludeFiles{};  // 新增成员变量
};

#endif // __KDIRECTORYENTRY_H_
