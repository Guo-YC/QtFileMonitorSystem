#include "klogmanager.h"
#include "klogdatabasehandler.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

KLogManager& KLogManager::getInstance()
{
    static KLogManager instance;
    return instance;
}

KLogManager::KLogManager()
{
    m_dbHandler = new KLogDatabaseHandler();
    m_dbHandler->moveToThread(&m_dbThread);

    connect(&m_dbThread, &QThread::finished, m_dbHandler, &QObject::deleteLater);
    connect(m_dbHandler, &KLogDatabaseHandler::logInserted, this, [this]() {
        qDebug() << "Log inserted into the database.";
        });
    connect(m_dbHandler, &KLogDatabaseHandler::queryCompleted, this, &KLogManager::queryCompleted);
    connect(m_dbHandler, &KLogDatabaseHandler::searchCompleted, this, &KLogManager::searchCompleted);

    m_dbThread.start();
}


KLogManager::~KLogManager()
{
    m_dbThread.quit();
    m_dbThread.wait();
    
}

void KLogManager::logEvent(const QString& type, const QString& event)
{
    QString log = QString("[%1] Event: %2").arg(QDateTime::currentDateTime().toString(Qt::ISODate), event);   
    m_dbHandler->insertLog(type, event);
}

void KLogManager::logUserAction(const QString& action)
{
    QString log = QString("[%1] User Action: %2").arg(QDateTime::currentDateTime().toString(Qt::ISODate), action);
    emit logUpdated(log);
}

void KLogManager::queryLogs(const QDateTime& startTime, const QDateTime& endTime)
{
    m_dbHandler->queryLogs(startTime, endTime);   
}

void KLogManager::searchLogs(const QString& keyword)
{
    m_dbHandler->searchLogs(keyword);    
}
