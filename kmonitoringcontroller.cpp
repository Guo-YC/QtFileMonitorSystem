#include "kmonitoringcontroller.h"
#include "kfilemonitor.h"
#include "klogmanager.h"
KMonitoringController::KMonitoringController(QObject* parent)
    : QObject(parent)
    , m_fileMonitor(new KFileMonitor(parent))
    , m_logManager(&KLogManager::getInstance())
{
    connect(m_fileMonitor, &KFileMonitor::fileAdded, this, &KMonitoringController::onFileAdded);
    connect(m_fileMonitor, &KFileMonitor::fileRemoved, this, &KMonitoringController::onFileRemoved);
    connect(m_fileMonitor, &KFileMonitor::fileModified, this, &KMonitoringController::onFileModified);
    connect(m_fileMonitor, &KFileMonitor::fileRenamed, this, &KMonitoringController::onFileRenamed);
}
KMonitoringController::~KMonitoringController()
{
    delete m_fileMonitor;
}
void KMonitoringController::addDirectory(const QString& path)
{
    m_fileMonitor->startMonitoring(path);
    m_logManager->logUserAction("Add directory: " + path);
}

void KMonitoringController::removeDirectory(const QString& path)
{
    m_fileMonitor->stopMonitoring(path);
    m_logManager->logUserAction("Remove directory: " + path);
}

void KMonitoringController::pauseMonitoring(const QString& path)
{
    m_fileMonitor->pauseMonitoring(path);
    m_logManager->logUserAction("Pause monitoring: " + path);
}

void KMonitoringController::resumeMonitoring(const QString& path)
{
    m_fileMonitor->resumeMonitoring(path);
    m_logManager->logUserAction("Resume monitoring: " + path);
}



void KMonitoringController::setEventsState(const QString& path, const QVector<bool>& eventsState)
{
    m_fileMonitor->setEventsState(path, eventsState);
    m_logManager->logUserAction("setEventsState: " + path);
}

void KMonitoringController::setIncludeExtensions(const QString& path, const QStringList& extensions)
{
    m_fileMonitor->setIncludeExtensions(path, extensions);
    m_logManager->logUserAction("setIncludeExtensions: " + path);
}

void KMonitoringController::setExcludeExtensions(const QString& path, const QStringList& extensions)
{
    m_fileMonitor->setExcludeExtensions(path, extensions);
    m_logManager->logUserAction("setExcludeExtensions: " + path);
}

void KMonitoringController::setIncludeFiles(const QString& path, const QStringList& files)  // 新增方法
{
    m_fileMonitor->setIncludeFiles(path, files);
    m_logManager->logUserAction("setIncludeFiles: " + path);
}

void KMonitoringController::setExcludeFiles(const QString& path, const QStringList& files)  // 新增方法
{
    m_fileMonitor->setExcludeFiles(path, files);
    m_logManager->logUserAction("setExcludeFiles: " + path);
}

void KMonitoringController::onFileAdded(const QString& path)
{
    emit fileAdded(path);
    m_logManager->logEvent(QString::fromLocal8Bit("新增"), QString::fromLocal8Bit("新增: ") + path);
}

void KMonitoringController::onFileRemoved(const QString& path)
{
    emit fileRemoved(path);
    m_logManager->logEvent(QString::fromLocal8Bit("删除"), QString::fromLocal8Bit("删除: ")+ path);
}

void KMonitoringController::onFileModified(const QString& path)
{
    emit fileModified(path);
    m_logManager->logEvent(QString::fromLocal8Bit("修改"), QString::fromLocal8Bit("修改: ") + path);
}

void KMonitoringController::onFileRenamed(const QString& oldPath, const QString& newPath)
{
    emit fileRenamed(oldPath, newPath);
    m_logManager->logEvent(QString::fromLocal8Bit("重命名"), QString::fromLocal8Bit("重命名: ") + oldPath + " to " + newPath);
}
