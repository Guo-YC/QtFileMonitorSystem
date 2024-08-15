#include "logworker.h"

LogWorker::LogWorker(QTextBrowser* logBrowser, QObject* parent)
    : QObject(parent), m_logTextBrowser(logBrowser), m_timer(new QTimer(this))
{
    // 每秒触发一次 processLogs
    connect(m_timer, &QTimer::timeout, this, &LogWorker::processLogs);
    m_timer->start(1000);  // 1秒间隔
}

LogWorker::~LogWorker()
{
    m_timer->stop();
}

void LogWorker::appendLog(const QString& log)
{
    QMutexLocker locker(&m_mutex);  // 确保线程安全
    m_logQueue.enqueue(log);  // 将日志加入队列
}

void LogWorker::processLogs()
{
    QMutexLocker locker(&m_mutex);  // 确保线程安全

    if (m_logQueue.isEmpty()) 
    {
        return;  // 队列为空，直接返回
    }

    QStringList compressedLogs;  // 用于存储压缩后的日志
    QString currentLog = m_logQueue.dequeue();  // 获取队列中的第一条日志
    int count = 1;

    // 遍历队列，进行压缩
    int sz = m_logQueue.size();
    while (sz>0 && sz--) 
    {
        QString nextLog = m_logQueue.dequeue();
        if (nextLog == currentLog) 
        {
            count++;  // 如果连续的日志相同，计数器加一
        }
        else 
        {
            if (count > 1) 
            {
                //QMetaObject::invokeMethod(m_logTextBrowser, "append", Qt::QueuedConnection, Q_ARG(QString, QString("%1 (x%2)").arg(currentLog).arg(count)));
                compressedLogs.append(QString("%1 (x%2)").arg(currentLog).arg(count));
            }
            else 
            {
                //QMetaObject::invokeMethod(m_logTextBrowser, "append", Qt::QueuedConnection, Q_ARG(QString, currentLog));
                compressedLogs.append(currentLog);
            }            
            currentLog = nextLog;
            count = 1;
        }
    }

    // 将压缩后的日志追加到 QTextBrowser 中，并滚动到底部
    int compressedLogs_sz = compressedLogs.size();
    for (int i = 0; i < compressedLogs_sz; ) 
    {
        QString log = compressedLogs[i];
        QMetaObject::invokeMethod(m_logTextBrowser, "append", Qt::QueuedConnection, Q_ARG(QString, log));
        i += 1 >= compressedLogs_sz/20  ? 1 : compressedLogs_sz / 20;
        if (1 < compressedLogs_sz / 20)
        {
            QMetaObject::invokeMethod(m_logTextBrowser, "append", Qt::QueuedConnection, Q_ARG(QString, QString::fromLocal8Bit("...(此处省略")+ QString::number(compressedLogs_sz / 20-1)+QString::fromLocal8Bit("条消息)")));
        }
    }

    // 处理最后一条日志
    if (count > 1) 
    {
        QMetaObject::invokeMethod(m_logTextBrowser, "append", Qt::QueuedConnection, Q_ARG(QString, QString("%1 (x%2)").arg(currentLog).arg(count)));
    }
    else 
    {
        QMetaObject::invokeMethod(m_logTextBrowser, "append", Qt::QueuedConnection, Q_ARG(QString, currentLog));
    }


        
}

