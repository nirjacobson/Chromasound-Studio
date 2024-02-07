#ifndef UNSELECTABLESTRINGLISTMODEL_H
#define UNSELECTABLESTRINGLISTMODEL_H

#include <QStringListModel>

class UnselectableStringListModel : public QStringListModel
{
    public:
        explicit UnselectableStringListModel(QObject *parent = nullptr);

        // QAbstractItemModel interface
    public:
        Qt::ItemFlags flags(const QModelIndex& index) const;
};

#endif // UNSELECTABLESTRINGLISTMODEL_H
