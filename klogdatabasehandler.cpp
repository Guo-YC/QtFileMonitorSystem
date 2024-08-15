#include "klogdatabasehandler.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QThread>
#include <QMutexLocker>

// ���������ڱ������ݿ����
QMutex dbMutex;

KLogDatabaseHandler::KLogDatabaseHandler(QObject* parent)
    : QObject(parent)
{
    initializeDatabase();
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &KLogDatabaseHandler::timerInsert);
    m_timer->start(1000);  // 1����
}

KLogDatabaseHandler::~KLogDatabaseHandler()
{
    QMutexLocker locker(&dbMutex);  // ȷ����ȫ�ر����ݿ�����
    if (m_db.isOpen())
    {
        m_db.close();
    }
    QSqlDatabase::removeDatabase(m_db.connectionName()); // ɾ�����ݿ�����
    delete m_timer;
}

void KLogDatabaseHandler::timerInsert()
{
    if (!m_pool.isEmpty()) 
    {
        insertPoolData();
    }
}

void KLogDatabaseHandler::initializeDatabase()
{
    QMutexLocker locker(&dbMutex);  // �������ݿ��ʼ��
    QString connectionName = QString("ConnThread%1").arg((quintptr)QThread::currentThreadId());
    m_db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    m_db.setDatabaseName("logs.db");

    if (!m_db.open()) 
    {
        qDebug() << "Error: Failed to connect to database." << m_db.lastError();
        return;
    }

    QSqlQuery query(m_db);
    QString createTableQuery = "CREATE TABLE IF NOT EXISTS logs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "timestamp DATETIME, "
        "event_type TEXT, "
        "message TEXT)";

    if (!query.exec(createTableQuery)) 
    {
        qDebug() << "Error: Failed to create table." << query.lastError();
    }

    // ��������
    QString createIndexQuery = "CREATE INDEX IF NOT EXISTS idx_timestamp ON logs (timestamp)";

    if (!query.exec(createIndexQuery)) 
    {
        qDebug() << "Error: Failed to create index on timestamp column." << query.lastError();
    }
}


void KLogDatabaseHandler::insertPoolData()
{
    QMutexLocker locker(&dbMutex);  // �������ݿ����
    const int batchSize = 1000;  // ÿ�δ���������־����

    if (!m_pool.isEmpty()) 
    {
        QSqlQuery query(m_db);
        query.prepare("INSERT INTO logs (timestamp, event_type, message) VALUES (:timestamp, :event_type, :message)");

        QVariantList timestamps, eventTypes, messages;
        int count = 0;

        while (!m_pool.isEmpty() && count < batchSize) 
        {
            const auto& log = m_pool.dequeue();
            timestamps << log.time;
            eventTypes << log.type;
            messages << log.message;
            count++;
        }

        query.bindValue(":timestamp", timestamps);
        query.bindValue(":event_type", eventTypes);
        query.bindValue(":message", messages);

        // �ȼ���Ƿ��Ѿ���������
        if (!m_db.transaction()) {
            qDebug() << "Error: Failed to start transaction." << m_db.lastError();
            return;
        }

        if (!query.execBatch()) {
            qDebug() << "Error: Failed to execute batch insert." << query.lastError();
            m_db.rollback();
        }
        else if (!m_db.commit()) {
            qDebug() << "Error: Failed to commit transaction." << m_db.lastError();
            m_db.rollback();
        }
        else {
            qDebug() << "Batch insert and transaction committed successfully.";
        }
    }
}


void KLogDatabaseHandler::insertLog(const QString& eventType, const QString& message)
{
    //QMutexLocker locker(&dbMutex);  // ������־�������
    sqlDate date;
    date.time = QDateTime::currentDateTime().toString(Qt::ISODate);
    date.type = eventType;
    date.message = message;

    m_pool.enqueue(date);
}

QList<sqlDate> KLogDatabaseHandler::queryLogs(const QDateTime& startTime, const QDateTime& endTime)
{
    QList<sqlDate> result;
    QMutexLocker locker(&dbMutex);  // ������ѯ����
    QSqlQuery query(m_db);

    query.prepare("SELECT timestamp, event_type, message FROM logs WHERE timestamp BETWEEN :start AND :end ORDER BY timestamp ASC");
    query.bindValue(":start", startTime.toString(Qt::ISODate));
    query.bindValue(":end", endTime.toString(Qt::ISODate));

    // ����Ƿ��Ѿ���������
    if (!m_db.transaction()) 
    {
        qDebug() << "Error: Failed to start transaction." << m_db.lastError();
        return result;
    }

    if (query.exec()) 
    {
        while (query.next()) 
        {
            sqlDate log;
            log.time = query.value("timestamp").toString();
            log.type = query.value("event_type").toString();
            log.message = query.value("message").toString();

            result.append(log);
        }
        if (!m_db.commit()) 
        {
            qDebug() << "Error: Failed to commit transaction." << m_db.lastError();
            m_db.rollback();
        }
    }
    else 
    {
        qDebug() << "Error: Failed to query logs." << query.lastError();
        m_db.rollback();
    }

    emit queryCompleted(result);
    return result;
}

QList<sqlDate> KLogDatabaseHandler::searchLogs(const QString& keyword)
{
    QList<sqlDate> result;
    QMutexLocker locker(&dbMutex);  // ������������
    QSqlQuery query(m_db);

    query.prepare("SELECT timestamp, event_type, message FROM logs WHERE message LIKE :keyword ORDER BY timestamp ASC");
    query.bindValue(":keyword", "%" + keyword + "%");

    // ����Ƿ��Ѿ���������
    if (!m_db.transaction()) 
    {
        qDebug() << "Error: Failed to start transaction." << m_db.lastError();
        return result;
    }

    if (query.exec()) 
    {
        while (query.next()) 
        {
            sqlDate log;
            log.time = query.value("timestamp").toString();
            log.type = query.value("event_type").toString();
            log.message = query.value("message").toString();

            result.append(log);
        }
        if (!m_db.commit()) 
        {
            qDebug() << "Error: Failed to commit transaction." << m_db.lastError();
            m_db.rollback();
        }
    }
    else 
    {
        qDebug() << "Error: Failed to search logs." << query.lastError();
        m_db.rollback();
    }

    emit searchCompleted(result);
    return result;
}
