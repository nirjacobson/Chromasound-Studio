#ifndef PLAYLISTTABLEMODEL_H
#define PLAYLISTTABLEMODEL_H

#include <QAbstractTableModel>

#include "playlistitem.h"

class PlaylistTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit PlaylistTableModel(QObject *parent, QList<PlaylistItem>& items);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void insertRow(const PlaylistItem& item);
    void removeRow(const int index);
    void clear();

private:
    QList<PlaylistItem>& _items;
};

#endif // PLAYLISTTABLEMODEL_H
