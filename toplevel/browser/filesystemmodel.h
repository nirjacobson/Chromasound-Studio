#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QFileSystemModel>

class FilesystemModel : public QFileSystemModel
{
    public:
        FilesystemModel();

        // QAbstractItemModel interface
    public:
        Qt::ItemFlags flags(const QModelIndex& index) const;
};

#endif // FILESYSTEMMODEL_H
