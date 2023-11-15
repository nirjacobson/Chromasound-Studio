#include "patchtablemodel.h"

PatchTableModel::PatchTableModel(QObject *parent, QList<QString>& names, QList<QList<int>>& octaves)
    : QAbstractTableModel(parent)
    , _names(names)
    , _octaves(octaves)
{

}

QVariant PatchTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical) {
        return QVariant();
    }

    switch (section) {
    case 0:
        return "Name";
    case 1:
        return "Octaves";
    default:
        return "";
    }
}

int PatchTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return _names.size();
}

int PatchTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant PatchTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole) {
        if (role == Qt::EditRole && index.column() == 0) {
            return _names[index.row()];
        }
        return QVariant();
    }

    QStringList octaves;
    switch (index.column()) {
    case 0:
        return _names[index.row()];
    case 1:
        for (int octave : _octaves[index.row()]) {
            octaves.append(QString::number(octave));
        }

        return octaves.join(", ");
    default:
        break;
    }

    return QVariant();
}

bool PatchTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.column() == 0) {
        _names[index.row()] = value.toString();

        return true;
    }

    return false;
}

Qt::ItemFlags PatchTableModel::flags(const QModelIndex& index) const
{
    if (index.column() == 0) {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }

    return QAbstractTableModel::flags(index);
}

void PatchTableModel::insertRow(const QString& name, const QList<int>& octaves)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _names.append(name);
    _octaves.append(octaves);
    endInsertRows();
}

void PatchTableModel::removeRow(const int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    _names.remove(index);
    _octaves.remove(index);
    endRemoveRows();
}

void PatchTableModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    _names.clear();
    _octaves.clear();
    endRemoveRows();
}
