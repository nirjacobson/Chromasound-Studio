#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QMimeData>

#include "filesystemmodel.h"

class SortFilterProxyModel : public QSortFilterProxyModel
{
    public:
        SortFilterProxyModel();

        // QSortFilterProxyModel interface
    protected:
        bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const;

        // QAbstractItemModel interface
    public:
        Qt::DropActions supportedDropActions() const;
        Qt::ItemFlags flags(const QModelIndex& index) const;
        QStringList mimeTypes() const;
        QMimeData* mimeData(const QModelIndexList& indexes) const;
};

#endif // SORTFILTERPROXYMODEL_H
