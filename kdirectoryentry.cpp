#include "kdirectoryentry.h"
#include <QStringList>
// Constructor
KDirectoryEntry::KDirectoryEntry(const QString& path)
    : m_path(path)    
    , m_monitorState(QString::fromLocal8Bit("正在监听"))
{
    m_events.push_back(QString::fromLocal8Bit("新建文件"));
    m_events.push_back(QString::fromLocal8Bit("修改"));
    m_events.push_back(QString::fromLocal8Bit("删除"));
    m_events.push_back(QString::fromLocal8Bit("重命名"));
    updateEventsStr();  // 初始化时更新拼接结果
}

// Destructor
KDirectoryEntry::~KDirectoryEntry() {}

// Getters
QString KDirectoryEntry::getPath() const
{
    return m_path;
}

QVector<QString> KDirectoryEntry::getEvents() const
{
    return m_events;
}

QString KDirectoryEntry::getEventsStr() const
{
    return m_eventsStr;
}

QString KDirectoryEntry::getMonitorState() const
{
    return m_monitorState;
}

bool KDirectoryEntry::getEventStateByIndex(int index) const
{
    if (index >= 0 && index < m_eventsState.size())
    {
        return m_eventsState[index];
    }
    return false;
}

QVector<bool> KDirectoryEntry::getEventsState() const
{
    return m_eventsState;
}

QStringList KDirectoryEntry::getIncludeExtensions() const
{
    return m_includeExtensions;
}

QString KDirectoryEntry::getIncludeExtensionsStr() const
{
    QString str = m_includeExtensions.join(";");
    return str;
}


QString KDirectoryEntry::getExcludeExtensionsStr() const
{
    QString str = m_excludeExtensions.join(";");
    return str;
}
QStringList KDirectoryEntry::getExcludeExtensions() const
{
    return m_excludeExtensions;
}

QStringList KDirectoryEntry::getIncludeFiles() const
{
    return m_includeFiles;
}

QStringList KDirectoryEntry::getExcludeFiles() const
{
    return m_excludeFiles;
}

// Setters
void KDirectoryEntry::setPath(const QString& path)
{
    m_path = path;
}

void KDirectoryEntry::setEvents(const QVector<QString>& events)
{
    m_events = events;
    updateEventsStr();  // 更新事件时更新拼接结果
}

void KDirectoryEntry::setEventStateByIndex(int index, bool state)
{
    if (index >= 0 && index < m_eventsState.size())
    {
        m_eventsState[index] = state;
        updateEventsStr();  // 更新状态时更新拼接结果
    }
}

void KDirectoryEntry::setMonitorState(const QString& state)
{
    m_monitorState = state;
}

void KDirectoryEntry::setIncludeExtensions(const QStringList& extensions)
{
    m_includeExtensions = extensions;
}

void KDirectoryEntry::setExcludeExtensions(const QStringList& extensions)
{
    m_excludeExtensions = extensions;
}

void KDirectoryEntry::setIncludeFiles(const QStringList& files)
{
    m_includeFiles = files;
}

void KDirectoryEntry::setExcludeFiles(const QStringList& files)
{
    m_excludeFiles = files;
}

// Private method
void KDirectoryEntry::updateEventsStr()
{
    QString str;
    int num = 0;
    for (int i = 0; i < m_events.size(); ++i)
    {
        if (m_eventsState[i])
        {
            if (num != 0)
                str += QString::fromLocal8Bit(",");
            str += m_events[i];
            ++num;
        }
    }
    m_eventsStr = str;
}
