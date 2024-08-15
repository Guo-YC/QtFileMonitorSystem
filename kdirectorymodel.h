#ifndef __KDIRECTORYMODEL_H
#define __KDIRECTORYMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>
#include <QString>
#include <QVector>
#include <QStringList>
class KDirectoryEntry;
class KDirectoryModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit KDirectoryModel(QObject* parent = nullptr);
    ~KDirectoryModel() = default;
    // 必需的函数重载
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;    
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // 自定义函数
    void addEntry(const KDirectoryEntry& entry);
    void removeEntry(int row);
    void updateEntry(int row, const KDirectoryEntry& entry);
    KDirectoryEntry getEntry(int row) const;  // 新增方法
    int getEntryIndexByPath(const QString& path);    

private:
    QVector<KDirectoryEntry> m_entries;
    QStringList m_headers;
};

#endif // __KDIRECTORYMODEL_H
