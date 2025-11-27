#include "rombuildertablemodel.h"

ROMBuilderTableModel::ROMBuilderTableModel(QObject *parent, QList<QString>& names, QList<QString>& paths)
    : QAbstractTableModel(parent)
    , _names(names)
    , _paths(paths)
{}

QVariant ROMBuilderTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical) {
        return QVariant();
    }

    switch (section) {
    case 0:
        return "Offset";
    case 1:
        return "Name";
    case 2:
        return "File";
    default:
        return "";
    }
}

int ROMBuilderTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return _names.size();
}

int ROMBuilderTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 3;
}

QVariant ROMBuilderTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole) {
        if (role == Qt::EditRole && index.column() == 1) {
            return _names[index.row()];
        }
        if (role == Qt::TextAlignmentRole && index.column() == 0) {
            return (int)(Qt::AlignRight | Qt::AlignVCenter);
        }
        return QVariant();
    }

    switch (index.column()) {
        case 0:
            return offsetOf(index.row());
        case 1:
            return _names[index.row()];
        case 2:
            return QFileInfo(_paths[index.row()]).fileName();
        default:
            break;
    }


    return QVariant();
}

bool ROMBuilderTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.column() == 1) {
        _names[index.row()] = value.toString();

        return true;
    }

    return false;
}

Qt::ItemFlags ROMBuilderTableModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index) | Qt::ItemIsDragEnabled;

    if (index.column() == 1) {
        return flags | Qt::ItemIsEditable;
    }

    return flags;
}

void ROMBuilderTableModel::insertRow(const QString& name, const QString& path)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _names.append(name);
    _paths.append(path);
    endInsertRows();
}

void ROMBuilderTableModel::removeRow(const int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    _names.remove(index);
    _paths.remove(index);
    endRemoveRows();
}

void ROMBuilderTableModel::clear()
{
    if (rowCount() == 0) return;

    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    _names.clear();
    _paths.clear();
    endRemoveRows();
}

void ROMBuilderTableModel::moveUp(const int index)
{
    beginMoveRows(QModelIndex(), index, index, QModelIndex(), index - 1);
    _names.swapItemsAt(index - 1, index);
    _paths.swapItemsAt(index - 1, index);
    endMoveRows();
}

void ROMBuilderTableModel::moveDown(const int index)
{
    beginMoveRows(QModelIndex(), index, index, QModelIndex(), index + 2);
    _names.swapItemsAt(index + 1, index);
    _paths.swapItemsAt(index + 1, index);
    endMoveRows();
}

quint32 ROMBuilderTableModel::offsetOf(const int index) const
{
    quint32 offset = 0;

    // Number of entries
    offset += sizeof(quint16);

    for (int i = 0; i < _names.size(); i++) {
        // Offset
        offset += sizeof(quint32);
        // Name
        offset += _names[i].size() + 1;
    }

    for (int i = 0; i < index; i++) {
        offset += qMin(65535, QFileInfo(_paths[i]).size());
    }

    return offset;
}

QStringList ROMBuilderTableModel::mimeTypes() const
{
    QStringList types;
    types << "text/uri-list";

    return types;
}

QMimeData* ROMBuilderTableModel::mimeData(const QModelIndexList& indexes) const
{
    QMimeData* mimeData = new QMimeData;
    QStringList paths;

    for (const QModelIndex& index : indexes) {
        if (index.isValid()) {
            paths.append(QString("file://%1").arg(_paths[index.row()]));
        }
    }
    QString pathsString = paths.join("\r\n");

    mimeData->setData("text/uri-list", pathsString.toUtf8());
    return mimeData;
}
