#ifndef KFILEMONITOR_H
#define KFILEMONITOR_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QHash>


class KMonitorWorker;
class KFileMonitor : public QObject
{
    Q_OBJECT

public:
    explicit KFileMonitor(QObject* parent = nullptr);
    ~KFileMonitor();
    void startMonitoring(const QString& path);
    void stopMonitoring(const QString& path);
    void stopAllMonitoring();
    void pauseMonitoring(const QString& path);
    void resumeMonitoring(const QString& path);
    void setMonitoringState(const QString& path, bool shouldMonitor);
    void setEventsState(const QString& path, const QVector<bool>& eventsState);
    void setIncludeExtensions(const QString& path, const QStringList& extensions);
    void setExcludeExtensions(const QString& path, const QStringList& extensions);
    void setIncludeFiles(const QString& path, const QStringList& files);  // 新增方法
    void setExcludeFiles(const QString& path, const QStringList& files);  // 新增方法
    bool isMonitoring(const QString& path) const;

signals:
    void fileAdded(const QString& path);
    void fileRemoved(const QString& path);
    void fileModified(const QString& path);
    void fileRenamed(const QString& oldPath, const QString& newPath);

private slots:
    void onFileAdded(const QString& path);
    void onFileRemoved(const QString& path);
    void onFileModified(const QString& path);
    void onFileRenamed(const QString& oldPath, const QString& newPath);

private:
    QHash<QString, QThread*> m_monitorThreads;
    QHash<QString, KMonitorWorker*> m_monitorWorkers;
    QHash<QString, QVector<bool>> m_eventsStateMap;
};

#endif // KFILEMONITOR_H
