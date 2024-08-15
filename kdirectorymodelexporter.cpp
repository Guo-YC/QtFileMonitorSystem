#include "kdirectorymodelexporter.h"
#include "kdirectoryentry.h"
#include "kdirectorymodel.h"
bool KDirectoryModelExporter::exportToFile(const KDirectoryModel& model, const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QJsonArray jsonArray;
    for (int i = 0; i < model.rowCount(); ++i)
    {
        QJsonObject jsonEntry = serializeEntry(model.getEntry(i));
        jsonArray.append(jsonEntry);
    }

    QJsonDocument jsonDoc(jsonArray);
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << jsonDoc.toJson();
    file.close();

    return true;
}

bool KDirectoryModelExporter::importFromFile(KDirectoryModel& model, const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString jsonString = in.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (jsonDoc.isNull() || !jsonDoc.isArray())
    {
        return false;
    }

    QJsonArray jsonArray = jsonDoc.array();
    for (const QJsonValue& value : jsonArray)
    {
        if (value.isObject())
        {
            KDirectoryEntry entry = deserializeEntry(value.toObject());
            model.addEntry(entry);
        }
    }

    return true;
}

QJsonObject KDirectoryModelExporter::serializeEntry(const KDirectoryEntry& entry)
{
    QJsonObject jsonObject;
    jsonObject["path"] = entry.getPath();
    jsonObject["events"] = QJsonArray::fromStringList(entry.getEventsStr().split(","));
    jsonObject["monitorState"] = entry.getMonitorState();
    jsonObject["includeExtensions"] = QJsonArray::fromStringList(entry.getIncludeExtensions());
    jsonObject["excludeExtensions"] = QJsonArray::fromStringList(entry.getExcludeExtensions());
    jsonObject["includeFiles"] = QJsonArray::fromStringList(entry.getIncludeFiles());
    jsonObject["excludeFiles"] = QJsonArray::fromStringList(entry.getExcludeFiles());

    return jsonObject;
}

KDirectoryEntry KDirectoryModelExporter::deserializeEntry(const QJsonObject& jsonObject)
{
    KDirectoryEntry entry(jsonObject["path"].toString());

    QStringList eventsList;
    for (const QJsonValue& event : jsonObject["events"].toArray())
    {
        eventsList.append(event.toString());
    }
    entry.setEvents(eventsList.toVector());

    entry.setMonitorState(jsonObject["monitorState"].toString());

    QStringList includeExtensions;
    for (const QJsonValue& ext : jsonObject["includeExtensions"].toArray())
    {
        includeExtensions.append(ext.toString());
    }
    entry.setIncludeExtensions(includeExtensions);

    QStringList excludeExtensions;
    for (const QJsonValue& ext : jsonObject["excludeExtensions"].toArray())
    {
        excludeExtensions.append(ext.toString());
    }
    entry.setExcludeExtensions(excludeExtensions);

    QStringList includeFiles;
    for (const QJsonValue& file : jsonObject["includeFiles"].toArray())
    {
        includeFiles.append(file.toString());
    }
    entry.setIncludeFiles(includeFiles);

    QStringList excludeFiles;
    for (const QJsonValue& file : jsonObject["excludeFiles"].toArray())
    {
        excludeFiles.append(file.toString());
    }
    entry.setExcludeFiles(excludeFiles);

    return entry;
}