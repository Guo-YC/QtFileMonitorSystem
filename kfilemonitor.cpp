#include "kfilemonitor.h"
#include "kmonitorworker.h"
KFileMonitor::KFileMonitor(QObject* parent)
    : QObject(parent)
{
}

KFileMonitor::~KFileMonitor()
{
    stopAllMonitoring();
}

void KFileMonitor::startMonitoring(const QString& path)
{
    if (m_monitorThreads.contains(path))
        return;

    KMonitorWorker* worker = new KMonitorWorker(path);
    QThread* thread = new QThread(this);

    connect(thread, &QThread::started, worker, &KMonitorWorker::startMonitoring);
    connect(thread, &QThread::finished, worker, &KMonitorWorker::deleteLater);
    connect(worker, &KMonitorWorker::fileAdded, this, &KFileMonitor::onFileAdded);
    connect(worker, &KMonitorWorker::fileRemoved, this, &KFileMonitor::onFileRemoved);
    connect(worker, &KMonitorWorker::fileModified, this, &KFileMonitor::onFileModified);
    connect(worker, &KMonitorWorker::fileRenamed, this, &KFileMonitor::onFileRenamed);

    worker->moveToThread(thread);
    thread->start();

    m_monitorThreads.insert(path, thread);
    m_monitorWorkers.insert(path, worker);

    // 初始化事件状态
    m_eventsStateMap.insert(path, QVector<bool>{true, true, true, true});
}

void KFileMonitor::stopMonitoring(const QString& path)
{
    if (!m_monitorThreads.contains(path))
        return;

    KMonitorWorker* worker = m_monitorWorkers.value(path);
    QThread* thread = m_monitorThreads.value(path);

    if (worker && thread)
    {
        worker->pause();
        worker->stopMonitoring();
        thread->terminate();
        thread->wait();
        delete thread;

        m_monitorThreads.remove(path);
        m_monitorWorkers.remove(path);
        m_eventsStateMap.remove(path);  // 移除对应的事件状态
    }
}

void KFileMonitor::stopAllMonitoring()
{
    foreach(const QString & path, m_monitorThreads.keys())
    {
        stopMonitoring(path);
    }
}

void KFileMonitor::pauseMonitoring(const QString& path)
{
    if (m_monitorWorkers.contains(path))
    {
        KMonitorWorker* worker = m_monitorWorkers.value(path);
        worker->pause();
    }
}

void KFileMonitor::resumeMonitoring(const QString& path)
{
    if (m_monitorWorkers.contains(path))
    {
        KMonitorWorker* worker = m_monitorWorkers.value(path);
        worker->resume();
    }
}

void KFileMonitor::setMonitoringState(const QString& path, bool shouldMonitor)
{
    if (m_monitorWorkers.contains(path))
    {
        KMonitorWorker* worker = m_monitorWorkers.value(path);
        worker->setShouldMonitor(shouldMonitor);
    }
}

void KFileMonitor::setEventsState(const QString& path, const QVector<bool>& eventsState)
{
    if (m_eventsStateMap.contains(path))
    {
        m_eventsStateMap[path] = eventsState;
        KMonitorWorker* worker = m_monitorWorkers.value(path);
        worker->setEventsState(eventsState);
    }
}

void KFileMonitor::setIncludeExtensions(const QString& path, const QStringList& extensions)
{
    if (m_monitorWorkers.contains(path))
    {
        KMonitorWorker* worker = m_monitorWorkers.value(path);
        worker->setIncludeExtensions(extensions);
    }
}

void KFileMonitor::setExcludeExtensions(const QString& path, const QStringList& extensions)
{
    if (m_monitorWorkers.contains(path))
    {
        KMonitorWorker* worker = m_monitorWorkers.value(path);
        worker->setExcludeExtensions(extensions);
    }
}

void KFileMonitor::setIncludeFiles(const QString& path, const QStringList& files)  // 新增方法
{
    if (m_monitorWorkers.contains(path))
    {
        KMonitorWorker* worker = m_monitorWorkers.value(path);
        worker->setIncludeFiles(files);
    }
}

void KFileMonitor::setExcludeFiles(const QString& path, const QStringList& files)  // 新增方法
{
    if (m_monitorWorkers.contains(path))
    {
        KMonitorWorker* worker = m_monitorWorkers.value(path);
        worker->setExcludeFiles(files);
    }
}

bool KFileMonitor::isMonitoring(const QString& path) const
{
    if (m_monitorWorkers.contains(path))
    {
        const KMonitorWorker* worker = m_monitorWorkers.value(path);
        return worker->isMonitoring() && !worker->isPaused();
    }
    return false;
}

void KFileMonitor::onFileAdded(const QString& path)
{
    emit fileAdded(path);
}

void KFileMonitor::onFileRemoved(const QString& path)
{
    emit fileRemoved(path);
}

void KFileMonitor::onFileModified(const QString& path)
{
    emit fileModified(path);
}

void KFileMonitor::onFileRenamed(const QString& oldPath, const QString& newPath)
{
    emit fileRenamed(oldPath, newPath);
}
