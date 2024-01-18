#ifndef PATCHTABLEMODEL_H
#define PATCHTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QString>

class PatchTableModel : public QAbstractTableModel
{
        Q_OBJECT

    public:
        explicit PatchTableModel(QObject *parent, QList<QString>& names, QList<QList<int> >& octaves);

        // Header:
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        // Basic functionality:
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

        bool setData(const QModelIndex& index, const QVariant& value, int role) override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;

        void insertRow(const QString& name, const QList<int>& octaves);
        void removeRow(const int index);
        void clear();

    private:
        QList<QString>& _names;
        QList<QList<int>>& _octaves;
};

#endif // PATCHTABLEMODEL_H
