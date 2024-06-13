#include "filesystemmodel.h"

FilesystemModel::FilesystemModel()
{

}

Qt::ItemFlags FilesystemModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QFileSystemModel::flags(index);

    if (index.column() == 0) {
        return flags | Qt::ItemIsEditable;
    }

    return flags;
}
