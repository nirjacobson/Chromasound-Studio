#include "unselectablestringlistmodel.h"

UnselectableStringListModel::UnselectableStringListModel(QObject *parent)
    : QStringListModel{parent}
{

}

Qt::ItemFlags UnselectableStringListModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QStringListModel::flags(index);

    flags &= ~Qt::ItemIsSelectable;

    return flags;
}
