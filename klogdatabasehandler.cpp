#include "klogdatabasehandler.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QThread>
#include <QMutexLocker>

// 互斥锁用于保护数据库操作
QMutex dbMutex;

KLogDatabaseHandler::KLogDatabaseHandler(QObject* parent)
    : QObject(parent)
{
    initializeDatabase();
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &KLogDatabaseHandler::timerInsert);
    m_timer->start(1000);  // 1秒间隔
}

KLogDatabaseHandler::~KLogDatabaseHandler()
{
    QMutexLocker locker(&dbMutex);  // 确保安全关闭数据库连接
    if (m_db.isOpen())
    {
        m_db.close();
    }
    QSqlDatabase::removeDatabase(m_db.connectionName()); // 删除数据库连接
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
    QMutexLocker locker(&dbMutex);  // 保护数据库初始化
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

    // 创建索引
    QString createIndexQuery = "CREATE INDEX IF NOT EXISTS idx_timestamp ON logs (timestamp)";

    if (!query.exec(createIndexQuery)) 
    {
        qDebug() << "Error: Failed to create index on timestamp column." << query.lastError();
    }
}


void KLogDatabaseHandler::insertPoolData()
{
    QMutexLocker locker(&dbMutex);  // 保护数据库操作
    const int batchSize = 1000;  // 每次处理的最大日志数量

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

        // 先检查是否已经在事务中
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
    //QMutexLocker locker(&dbMutex);  // 保护日志插入操作
    sqlDate date;
    date.time = QDateTime::currentDateTime().toString(Qt::ISODate);
    date.type = eventType;
    date.message = message;

    m_pool.enqueue(date);
}

QList<sqlDate> KLogDatabaseHandler::queryLogs(const QDateTime& startTime, const QDateTime& endTime)
{
    QList<sqlDate> result;
    QMutexLocker locker(&dbMutex);  // 保护查询操作
    QSqlQuery query(m_db);

    query.prepare("SELECT timestamp, event_type, message FROM logs WHERE timestamp BETWEEN :start AND :end ORDER BY timestamp ASC");
    query.bindValue(":start", startTime.toString(Qt::ISODate));
    query.bindValue(":end", endTime.toString(Qt::ISODate));

    // 检查是否已经在事务中
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
    QMutexLocker locker(&dbMutex);  // 保护搜索操作
    QSqlQuery query(m_db);

    query.prepare("SELECT timestamp, event_type, message FROM logs WHERE message LIKE :keyword ORDER BY timestamp ASC");
    query.bindValue(":keyword", "%" + keyword + "%");

    // 检查是否已经在事务中
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
