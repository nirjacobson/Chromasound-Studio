#ifndef ROMBUILDERTABLEMODEL_H
#define ROMBUILDERTABLEMODEL_H

#include <QAbstractTableModel>
#include <QFileInfo>
#include <QMimeData>

class ROMBuilderTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ROMBuilderTableModel(QObject *parent, QList<QString>& names, QList<QString>& paths);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;


    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void insertRow(const QString& name, const QString& path);

    void removeRow(const int index);
    void clear();
    void moveUp(const int index);
    void moveDown(const int index);
    quint32 offsetOf(const int index) const;
private:
    QList<QString>& _names;
    QList<QString>& _paths;

    // QAbstractItemModel interface
public:
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList& indexes) const override;
};

#endif // ROMBUILDERTABLEMODEL_H
