#ifndef KLOGMANAGER_H
#define KLOGMANAGER_H

#include <QObject>
#include <QThread>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QSqlDatabase>
#include <QTimer>
#include <QQueue>  // Include for QQueue
class KLogDatabaseHandler;
struct sqlDate;
class KLogManager : public QObject
{
    Q_OBJECT

public:
    static KLogManager& getInstance();
    void logEvent(const QString& type, const QString& event);
    void logUserAction(const QString& action);
    void queryLogs(const QDateTime& startTime, const QDateTime& endTime);
    void searchLogs(const QString& keyword);

signals:
    void logUpdated(const QString& log);
    void queryCompleted(const QList<sqlDate>& results);
    void searchCompleted(const QList<sqlDate>& results);

private:
    KLogManager();
    ~KLogManager();
    KLogManager(const KLogManager&) = delete;
    KLogManager& operator=(const KLogManager&) = delete;

    QThread m_dbThread;
    KLogDatabaseHandler* m_dbHandler;
};

#endif // KLOGMANAGER_H
