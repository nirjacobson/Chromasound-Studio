#ifndef FILESYSTEMTREEVIEW_H
#define FILESYSTEMTREEVIEW_H

#include <QTreeView>
#include <QMouseEvent>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QInputDialog>
#include <QProcess>

#include "filesystemmodel.h"

class FilesystemTreeView : public QTreeView
{
public:
    FilesystemTreeView(QWidget* parent);

private:
    QMenu _contextMenu;
    QAction _newFolderAction;
    QAction _renameAction;
    QAction _deleteAction;
    QAction _revealAction;
    QAction* _separator1;

    QModelIndex _index;
    QFileInfo _fileInfo;

private slots:
    void contextMenuRequested(const QPoint& p);
    void newFolderTriggered();
    void renameTriggered();
    void deleteTriggered();
    void revealTriggered();
};

#endif // FILESYSTEMTREEVIEW_H
