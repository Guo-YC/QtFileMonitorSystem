#ifndef KLOGDATABASEHANDLER_H
#define KLOGDATABASEHANDLER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QSqlDatabase>
#include <QTimer>
#include <QQueue>  // Include for QQueue

struct sqlDate
{
    QString time;
    QString type;
    QString message;
};

class KLogDatabaseHandler : public QObject
{
    Q_OBJECT

public:
    explicit KLogDatabaseHandler(QObject* parent = nullptr);
    ~KLogDatabaseHandler();

public slots:
    void insertLog(const QString& eventType, const QString& message);
    void timerInsert();  // Slot for timer-triggered inserts
    QList<sqlDate> queryLogs(const QDateTime& startTime, const QDateTime& endTime);
    QList<sqlDate> searchLogs(const QString& keyword);

signals:
    void logInserted();
    void queryCompleted(const QList<sqlDate>& results);
    void searchCompleted(const QList<sqlDate>& results);

private:
    void initializeDatabase();
    void insertPoolData();
    QQueue<sqlDate> m_pool;  // Use QQueue instead of QVector
    QSqlDatabase m_db;
    QTimer* m_timer;  // Timer for periodic log insertion
};

#endif // KLOGDATABASEHANDLER_H