#include "playlisttablemodel.h"
#include "player.h"

PlaylistTableModel::PlaylistTableModel(QObject *parent, QList<PlaylistItem>& items)
    : QAbstractTableModel(parent)
    , _items(items)
{
}

QVariant PlaylistTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical) {
        return QVariant();
    }

    switch (section) {
        case 0:
            return "Title";
        case 1:
            return "Artist";
        case 2:
            return "Album";
        case 3:
            return "Length";
        default:
            return "";
    }
}

int PlaylistTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return _items.size();
}

int PlaylistTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 4;
}

QVariant PlaylistTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !(role == Qt::DisplayRole || role == Qt::TextAlignmentRole)) {
        return QVariant();
    }

    if (role == Qt::TextAlignmentRole) {
        if (index.column() == 3) {
            return QVariant(Qt::AlignRight | Qt::AlignVCenter);
        }

        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    }

    int length;
    switch (index.column()) {
        case 0:
            return _items[index.row()].title();
        case 1:
            return _items[index.row()].artist();
        case 2:
            return _items[index.row()].album();
        case 3:
            length = _items[index.row()].length();
            return QString("%1:%2").arg(length / 60).arg(length % 60, 2, 10, QLatin1Char('0'));
        default:
            break;
    }

    return QVariant();
}

void PlaylistTableModel::insertRow(const PlaylistItem& item)
{
    beginRemoveRows(QModelIndex(), rowCount(), rowCount());
    _items.append(item);
    endInsertRows();
}

void PlaylistTableModel::removeRow(const int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    _items.remove(index);
    endRemoveRows();
}

void PlaylistTableModel::clear()
{
    if (rowCount() == 0) return;

    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    _items.clear();
    endRemoveRows();
}

QStringList PlaylistTableModel::mimeTypes() const
{
    QStringList types;
    types << "text/uri-list";

    return types;
}

QMimeData* PlaylistTableModel::mimeData(const QModelIndexList& indexes) const
{
    QMimeData* mimeData = new QMimeData;
    QStringList paths;

    for (const QModelIndex& index : indexes) {
        if (index.isValid()) {
            paths.append(QString("file://%1").arg(_items[index.row()].path()));
        }
    }
    QString pathsString = paths.join("\r\n");

    mimeData->setData("text/uri-list", pathsString.toUtf8());
    return mimeData;
}

Qt::ItemFlags PlaylistTableModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    return Qt::ItemIsDragEnabled | defaultFlags;
}

