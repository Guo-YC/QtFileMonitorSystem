#ifndef LOGWORKER_H
#define LOGWORKER_H

#include <QObject>
#include <QString>
#include <QTextBrowser>
#include <QMutex>
#include <QTimer>
#include <QQueue>

class LogWorker : public QObject
{
    Q_OBJECT

public:
    explicit LogWorker(QTextBrowser* logBrowser, QObject* parent = nullptr);
    ~LogWorker();

public slots:
    void appendLog(const QString& log);

private slots:
    void processLogs();  // 处理并压缩日志

private:
    QTextBrowser* m_logTextBrowser;
    QMutex m_mutex;  // 用于保护日志队列的互斥锁
    QQueue<QString> m_logQueue;  // 存储日志的队列
    QTimer* m_timer;  // 定时器用于每秒更新日志
};

#endif // LOGWORKER_H
