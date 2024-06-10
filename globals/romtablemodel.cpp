#include "romtablemodel.h"

ROMTableModel::ROMTableModel(QObject *parent, const ROM& rom)
    : QAbstractTableModel(parent)
    , _rom(rom)
{}

QVariant ROMTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical) {
        return QVariant();
    }

    switch (section) {
    case 0:
        return "Offset";
    case 1:
        return "Name";
    default:
        return "";
    }
}

int ROMTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return _rom.names().size();
}

int ROMTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant ROMTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole) {
        if (role == Qt::TextAlignmentRole && index.column() == 0) {
            return (int)(Qt::AlignRight | Qt::AlignVCenter);
        }
        return QVariant();
    }

    switch (index.column()) {
    case 0:
        return _rom.offsets()[index.row()];
    case 1:
        return _rom.names()[index.row()];
    default:
        break;
    }


    return QVariant();
}

void ROMTableModel::doUpdate()
{
    beginResetModel();
    endResetModel();
}
