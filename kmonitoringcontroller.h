#ifndef KMONITORINGCONTROLLER_H
#define KMONITORINGCONTROLLER_H

#include <QObject>

class KLogManager;
class KFileMonitor;
struct sqlDate;
class KMonitoringController : public QObject
{
    Q_OBJECT

public:
    explicit KMonitoringController(QObject* parent = nullptr);
    ~KMonitoringController();
    void addDirectory(const QString& path);
    void removeDirectory(const QString& path);
    void pauseMonitoring(const QString& path);
    void resumeMonitoring(const QString& path);
    void setEventsState(const QString& path, const QVector<bool>& eventsState);
    void setIncludeExtensions(const QString& path, const QStringList& extensions);
    void setExcludeExtensions(const QString& path, const QStringList& extensions);
    void setIncludeFiles(const QString& path, const QStringList& files);  // 新增方法
    void setExcludeFiles(const QString& path, const QStringList& files);  // 新增方法

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
    KFileMonitor* m_fileMonitor;
    KLogManager* m_logManager;    
};

#endif // KMONITORINGCONTROLLER_H
