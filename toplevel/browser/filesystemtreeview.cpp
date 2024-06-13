#include "filesystemtreeview.h"

FilesystemTreeView::FilesystemTreeView(QWidget* parent)
    : QTreeView(parent)
    , _newFolderAction("New folder...", this)
    , _renameAction("Rename", this)
    , _deleteAction("Delete", this)
    , _revealAction("Reveal", this)
    , _separator1(nullptr)
{
    _contextMenu.addAction(&_renameAction);
    _contextMenu.addAction(&_deleteAction);
    _contextMenu.addSeparator();
    _contextMenu.addAction(&_revealAction);

    setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &FilesystemTreeView::contextMenuRequested);

    connect(&_newFolderAction, &QAction::triggered, this, &FilesystemTreeView::newFolderTriggered);
    connect(&_renameAction, &QAction::triggered, this, &FilesystemTreeView::renameTriggered);
    connect(&_deleteAction, &QAction::triggered, this, &FilesystemTreeView::deleteTriggered);
    connect(&_revealAction, &QAction::triggered, this, &FilesystemTreeView::revealTriggered);
}


void FilesystemTreeView::contextMenuRequested(const QPoint& p)
{
    QSortFilterProxyModel* sortFilterProxyModel = dynamic_cast<QSortFilterProxyModel*>(QTreeView::model());
    QFileSystemModel* filesystemModel = dynamic_cast<QFileSystemModel*>(sortFilterProxyModel->sourceModel());
                              ;
    _index = indexAt(p);
    if (!_index.isValid()) {
        _index = rootIndex();
    }

    _fileInfo = filesystemModel->fileInfo(sortFilterProxyModel->mapToSource(_index));

    if (_fileInfo.isDir()) {
        _contextMenu.insertAction(&_renameAction, &_newFolderAction);
        _separator1 = _contextMenu.insertSeparator(&_renameAction);
    } else {
        _contextMenu.removeAction(&_newFolderAction);
        _contextMenu.removeAction(_separator1);
    }

    _contextMenu.exec(mapToGlobal(p));
}

void FilesystemTreeView::newFolderTriggered()
{
    QString name = QInputDialog::getText(nullptr, "New Folder", "Name:");

    if (!name.isNull()) {
        QDir dir(_fileInfo.absoluteFilePath());
        dir.mkdir(name);
    }
}

void FilesystemTreeView::renameTriggered()
{
    edit(_index);
}

void FilesystemTreeView::deleteTriggered()
{
    QMessageBox::StandardButton b = QMessageBox::question(nullptr, "Confirm", QString("Delete %1?").arg(_fileInfo.fileName()));

    if (b == QMessageBox::StandardButton::Yes) {
        if (_fileInfo.isDir()) {
            QDir dir(_fileInfo.filesystemAbsoluteFilePath());
            dir.removeRecursively();
        } else {
            QFile file(_fileInfo.filesystemAbsoluteFilePath());
            file.remove();
        }
    }
}

void FilesystemTreeView::revealTriggered()
{
    QStringList args;
    args << "--session";
    args << "--dest=org.freedesktop.FileManager1";
    args << "--type=method_call";
    args << "/org/freedesktop/FileManager1";
    args << "org.freedesktop.FileManager1.ShowItems";
    args << "array:string:file://" + _fileInfo.absoluteFilePath();
    args << "string:";

    QProcess::startDetached("dbus-send", args);
}
