#ifndef __KDIRECTORYMODELEXPORTER_H_
#define __KDIRECTORYMODELEXPORTER_H_

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>


class KDirectoryEntry;
class KDirectoryModel;
class KDirectoryModelExporter
{
public:
    static bool exportToFile(const KDirectoryModel& model, const QString& fileName);
    static bool importFromFile(KDirectoryModel& model, const QString& fileName);

private:
    static QJsonObject serializeEntry(const KDirectoryEntry& entry);
    static KDirectoryEntry deserializeEntry(const QJsonObject& jsonObject);
};

#endif // __KDIRECTORYMODELEXPORTER_H_