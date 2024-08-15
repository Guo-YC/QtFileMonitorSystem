#ifndef KMONITORWORKER_H
#define KMONITORWORKER_H

#include <QObject>
#include <QString>
#include <windows.h>
#include <QStringList>

class KMonitorWorker : public QObject
{
    Q_OBJECT

public:
    explicit KMonitorWorker(const QString& path, QObject* parent = nullptr);
    ~KMonitorWorker();
    void startMonitoring();
    void stopMonitoring();
    void pause();
    void resume();
    void setShouldMonitor(bool shouldMonitor);
    bool isMonitoring() const;
    bool isPaused() const;
    void setEventsState(QVector<bool> state);
    void setIncludeExtensions(const QStringList& extensions);
    void setExcludeExtensions(const QStringList& extensions);
    void setIncludeFiles(const QStringList& files);  // 新增方法
    void setExcludeFiles(const QStringList& files);  // 新增方法

signals:
    void fileAdded(const QString& path);
    void fileRemoved(const QString& path);
    void fileModified(const QString& path);
    void fileRenamed(const QString& oldPath, const QString& newPath);

public slots:
    void monitorDirectory();

private:
    QString m_directoryPath;
    HANDLE m_directoryHandle;
    bool m_monitoring;
    bool m_paused;
    bool m_shouldMonitor;
    QVector<bool> m_eventsState{ true, true, true, true };
    QStringList m_includeExtensions;
    QStringList m_excludeExtensions;
    QStringList m_includeFiles;  // 新增成员变量
    QStringList m_excludeFiles;  // 新增成员变量
};

#endif // KMONITORWORKER_H
