#include "kdirectorymodel.h"
#include "kdirectoryentry.h"
KDirectoryModel::KDirectoryModel(QObject* parent)
    : QAbstractTableModel(parent)
    , m_headers({ QString::fromLocal8Bit("目录")
    , QString::fromLocal8Bit("监控事件")
    , QString::fromLocal8Bit("关注文件后缀") 
    , QString::fromLocal8Bit("忽略文件后缀") 
    , QString::fromLocal8Bit("监听状态") })
{
}

int KDirectoryModel::rowCount(const QModelIndex& /*parent*/) const
{
    return m_entries.size();
}

int KDirectoryModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 5; // 目录、监控事件、子目录
}



QVariant KDirectoryModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_entries.size() || index.column() >= 5)
        return QVariant();

    const KDirectoryEntry& entry = m_entries.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
        case 0: return entry.getPath();
        case 1: return entry.getEventsStr();
        case 2: return entry.getIncludeExtensionsStr();
        case 3: return entry.getExcludeExtensionsStr();
        case 4: return entry.getMonitorState();
        
        }
    }

    return QVariant();
}

QVariant KDirectoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        return m_headers.at(section);
    }

    return QVariant();
}

bool KDirectoryModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    return false;
}

Qt::ItemFlags KDirectoryModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

void KDirectoryModel::addEntry(const KDirectoryEntry& entry)
{
    beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size());
    m_entries.append(entry);
    endInsertRows();
}

void KDirectoryModel::removeEntry(int row)
{
    if (row < 0 || row >= m_entries.size())
        return;
    beginRemoveRows(QModelIndex(), row, row);
    m_entries.removeAt(row);
    endRemoveRows();
}

void KDirectoryModel::updateEntry(int row, const KDirectoryEntry& entry)
{
    if (row < 0 || row >= m_entries.size())
        return;
    m_entries[row] = entry;
    emit dataChanged(index(row, 0), index(row, columnCount() - 1));
}

KDirectoryEntry KDirectoryModel::getEntry(int row) const
{
    if (row < 0 || row >= m_entries.size())
        return KDirectoryEntry{};
    return m_entries.at(row);
}

int KDirectoryModel::getEntryIndexByPath(const QString& path) 
{
    int sz = m_entries.size();
    for (int i = 0; i < sz; ++i)
    {
        if (m_entries[i].getPath() == path) return i;
    }
    return -1;
}
