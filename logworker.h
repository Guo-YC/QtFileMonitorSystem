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
    void processLogs();  // ����ѹ����־

private:
    QTextBrowser* m_logTextBrowser;
    QMutex m_mutex;  // ���ڱ�����־���еĻ�����
    QQueue<QString> m_logQueue;  // �洢��־�Ķ���
    QTimer* m_timer;  // ��ʱ������ÿ�������־
};

#endif // LOGWORKER_H
