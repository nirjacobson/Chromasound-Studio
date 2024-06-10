#ifndef ROMTABLEMODEL_H
#define ROMTABLEMODEL_H

#include <QAbstractTableModel>
#include <QFileInfo>

#include "formats/rom.h"

class ROMTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ROMTableModel(QObject *parent, const ROM& rom);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void doUpdate();

private:
    const ROM& _rom;
};

#endif // ROMTABLEMODEL_H
