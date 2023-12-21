#include "sortfilterproxymodel.h"

SortFilterProxyModel::SortFilterProxyModel()
{

}

bool SortFilterProxyModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
    FilesystemModel* sourceModel = dynamic_cast<FilesystemModel*>(QSortFilterProxyModel::sourceModel());

    if (sourceModel) {
        QFileInfo leftInfo = sourceModel->fileInfo(source_left);
        QFileInfo rightInfo = sourceModel->fileInfo(source_right);

        if (leftInfo.isDir() && !rightInfo.isDir()) {
            return false;
        }

        if (!leftInfo.isDir() && rightInfo.isDir()) {
            return true;
        }
    }

    return !QSortFilterProxyModel::lessThan(source_left, source_right);
}

Qt::DropActions SortFilterProxyModel::supportedDropActions() const
{
    return Qt::IgnoreAction;
}

Qt::ItemFlags SortFilterProxyModel::flags(const QModelIndex& index) const
{
    FilesystemModel* sourceModel = dynamic_cast<FilesystemModel*>(QSortFilterProxyModel::sourceModel());
    Qt::ItemFlags defaultFlags = QSortFilterProxyModel::flags(index);

    if (index.isValid() && sourceModel->fileInfo(mapToSource(index)).isFile()) {
        return Qt::ItemIsDragEnabled | defaultFlags;
    }

    return defaultFlags;
}

QStringList SortFilterProxyModel::mimeTypes() const
{
    QStringList types;
    types << "text/uri-list";

    return types;
}

QMimeData* SortFilterProxyModel::mimeData(const QModelIndexList& indexes) const
{
    FilesystemModel* sourceModel = dynamic_cast<FilesystemModel*>(QSortFilterProxyModel::sourceModel());
    QMimeData* mimeData = new QMimeData;
    QStringList paths;

    for (const QModelIndex& index : indexes) {
        if (index.isValid()) {
            paths.append(QString("file://%1").arg(sourceModel->fileInfo(mapToSource(index)).absoluteFilePath()));
        }
    }
    QString pathsString = paths.join("\r\n");

    mimeData->setData("text/uri-list", pathsString.toUtf8());
    return mimeData;
}
