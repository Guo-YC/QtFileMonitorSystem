#include "kmonitorworker.h"
#include <QDebug>
#include <QDir>
#include <QTimer>

KMonitorWorker::KMonitorWorker(const QString& path, QObject* parent)
    : QObject(parent), m_directoryPath(path), m_monitoring(false), m_paused(false), m_shouldMonitor(true), m_directoryHandle(INVALID_HANDLE_VALUE)
{
}

KMonitorWorker::~KMonitorWorker()
{
    stopMonitoring();
}

void KMonitorWorker::startMonitoring()
{
    if (m_monitoring)
        return;

    m_monitoring = true;
    QTimer::singleShot(0, this, &KMonitorWorker::monitorDirectory); // 使用 QTimer 在下一个事件循环中启动监控
}

void KMonitorWorker::stopMonitoring()
{
    if (!m_monitoring)
        return;

    m_monitoring = false;
    if (m_directoryHandle != INVALID_HANDLE_VALUE)
    {
        CancelIo(m_directoryHandle);
        CloseHandle(m_directoryHandle);
        m_directoryHandle = INVALID_HANDLE_VALUE;
    }
}

void KMonitorWorker::pause()
{
    m_paused = true;
    setShouldMonitor(false);
}

void KMonitorWorker::resume()
{
    if (m_paused)
    {
        m_paused = false;
        setShouldMonitor(true);
        QTimer::singleShot(0, this, &KMonitorWorker::monitorDirectory); // 使用 QTimer 在下一个事件循环中继续监控
    }
}

void KMonitorWorker::setShouldMonitor(bool shouldMonitor)
{
    m_shouldMonitor = shouldMonitor;
}

bool KMonitorWorker::isMonitoring() const
{
    return m_monitoring;
}

bool KMonitorWorker::isPaused() const
{
    return m_paused;
}

void KMonitorWorker::setEventsState(QVector<bool> state)
{
    m_eventsState = state;
}

void KMonitorWorker::setIncludeExtensions(const QStringList& extensions)
{
    m_includeExtensions = extensions;
}

void KMonitorWorker::setExcludeExtensions(const QStringList& extensions)
{
    m_excludeExtensions = extensions;
}

void KMonitorWorker::setIncludeFiles(const QStringList& files)
{
    m_includeFiles = files;
}

void KMonitorWorker::setExcludeFiles(const QStringList& files)
{
    m_excludeFiles = files;
}

void KMonitorWorker::monitorDirectory()
{
    if (m_paused || !m_monitoring)
        return;

    std::wstring path = m_directoryPath.toStdWString();

    m_directoryHandle = CreateFileW(
        path.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL);

    if (m_directoryHandle == INVALID_HANDLE_VALUE)
    {
        qDebug() << "Failed to get directory handle.";
        return;
    }

    char buffer[1024];
    DWORD bytesReturned;
    FILE_NOTIFY_INFORMATION* pNotify;
    QString fileName;

    while (m_monitoring && !m_paused)
    {
        if (ReadDirectoryChangesW(
            m_directoryHandle,
            buffer,
            sizeof(buffer),
            TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
            FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS |
            FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY,
            &bytesReturned,
            NULL,
            NULL))
        {
            pNotify = (FILE_NOTIFY_INFORMATION*)buffer;

            do
            {
                fileName = QString::fromWCharArray(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
                QString fileExtension =QString(".") + QFileInfo(fileName).suffix();

                bool shouldEmitSignal = true;

                // 判断是否在包含的文件后缀列表中
                if (!m_includeExtensions.isEmpty() && !m_includeExtensions.contains(fileExtension, Qt::CaseInsensitive))
                {
                    shouldEmitSignal = false;
                }

                // 判断是否在排除的文件后缀列表中
                if (!m_excludeExtensions.isEmpty() && m_excludeExtensions.contains(fileExtension, Qt::CaseInsensitive))
                {
                    shouldEmitSignal = false;
                }

                // 判断是否在包含的文件路径列表中
                if (!m_includeFiles.isEmpty() && !m_includeFiles.contains(QDir(m_directoryPath).filePath(fileName), Qt::CaseInsensitive))
                {
                    shouldEmitSignal = false;
                }

                // 判断是否在排除的文件路径列表中
                if (!m_excludeFiles.isEmpty() && m_excludeFiles.contains(QDir(m_directoryPath).filePath(fileName), Qt::CaseInsensitive))
                {
                    shouldEmitSignal = false;
                }

                if (m_shouldMonitor && shouldEmitSignal)  // 只在需要监控且符合过滤器条件时发出信号
                {
                    switch (pNotify->Action)
                    {
                    case FILE_ACTION_ADDED:
                        if (m_eventsState[0])
                            emit fileAdded(QDir(m_directoryPath).filePath(fileName));
                        break;
                    case FILE_ACTION_REMOVED:
                        if (m_eventsState[2]) emit fileRemoved(QDir(m_directoryPath).filePath(fileName));
                        break;
                    case FILE_ACTION_MODIFIED:
                        if (m_eventsState[1]) emit fileModified(QDir(m_directoryPath).filePath(fileName));
                        break;
                    case FILE_ACTION_RENAMED_OLD_NAME:
                        if (pNotify->NextEntryOffset)
                        {
                            pNotify = (FILE_NOTIFY_INFORMATION*)((LPBYTE)pNotify + pNotify->NextEntryOffset);
                            QString newFileName = QString::fromWCharArray(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
                            if (m_eventsState[3]) emit fileRenamed(QDir(m_directoryPath).filePath(fileName), QDir(m_directoryPath).filePath(newFileName));
                        }
                        break;
                    default:
                        break;
                    }
                }

                pNotify = pNotify->NextEntryOffset
                    ? (FILE_NOTIFY_INFORMATION*)((LPBYTE)pNotify + pNotify->NextEntryOffset)
                    : NULL;
            } while (pNotify);
        }
        else
        {
            qDebug() << "ReadDirectoryChangesW failed.";
            break;
        }
    }

    CloseHandle(m_directoryHandle);
    m_directoryHandle = INVALID_HANDLE_VALUE;

    if (m_monitoring && !m_paused)
    {
        QTimer::singleShot(0, this, &KMonitorWorker::monitorDirectory); // 使用 QTimer 在下一个事件循环中继续监控
    }
}
