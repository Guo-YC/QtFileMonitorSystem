#include "logworker.h"

LogWorker::LogWorker(QTextBrowser* logBrowser, QObject* parent)
    : QObject(parent), m_logTextBrowser(logBrowser), m_timer(new QTimer(this))
{
    // ÿ�봥��һ�� processLogs
    connect(m_timer, &QTimer::timeout, this, &LogWorker::processLogs);
    m_timer->start(1000);  // 1����
}

LogWorker::~LogWorker()
{
    m_timer->stop();
}

void LogWorker::appendLog(const QString& log)
{
    QMutexLocker locker(&m_mutex);  // ȷ���̰߳�ȫ
    m_logQueue.enqueue(log);  // ����־�������
}

void LogWorker::processLogs()
{
    QMutexLocker locker(&m_mutex);  // ȷ���̰߳�ȫ

    if (m_logQueue.isEmpty()) 
    {
        return;  // ����Ϊ�գ�ֱ�ӷ���
    }

    QStringList compressedLogs;  // ���ڴ洢ѹ�������־
    QString currentLog = m_logQueue.dequeue();  // ��ȡ�����еĵ�һ����־
    int count = 1;

    // �������У�����ѹ��
    int sz = m_logQueue.size();
    while (sz>0 && sz--) 
    {
        QString nextLog = m_logQueue.dequeue();
        if (nextLog == currentLog) 
        {
            count++;  // �����������־��ͬ����������һ
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

    // ��ѹ�������־׷�ӵ� QTextBrowser �У����������ײ�
    int compressedLogs_sz = compressedLogs.size();
    for (int i = 0; i < compressedLogs_sz; ) 
    {
        QString log = compressedLogs[i];
        QMetaObject::invokeMethod(m_logTextBrowser, "append", Qt::QueuedConnection, Q_ARG(QString, log));
        i += 1 >= compressedLogs_sz/20  ? 1 : compressedLogs_sz / 20;
        if (1 < compressedLogs_sz / 20)
        {
            QMetaObject::invokeMethod(m_logTextBrowser, "append", Qt::QueuedConnection, Q_ARG(QString, QString::fromLocal8Bit("...(�˴�ʡ��")+ QString::number(compressedLogs_sz / 20-1)+QString::fromLocal8Bit("����Ϣ)")));
        }
    }

    // �������һ����־
    if (count > 1) 
    {
        QMetaObject::invokeMethod(m_logTextBrowser, "append", Qt::QueuedConnection, Q_ARG(QString, QString("%1 (x%2)").arg(currentLog).arg(count)));
    }
    else 
    {
        QMetaObject::invokeMethod(m_logTextBrowser, "append", Qt::QueuedConnection, Q_ARG(QString, currentLog));
    }


        
}

