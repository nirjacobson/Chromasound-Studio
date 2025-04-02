#ifndef PCMWIDGETTABLEMODEL_H
#define PCMWIDGETTABLEMODEL_H

#include <QAbstractTableModel>

#include "application.h"
#include "formats/rom.h"

class PCMWidgetTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit PCMWidgetTableModel(QObject *parent, Application* app, QList<QString>& keys, QList<int>& samples);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void insertRow();
    void insertRow(const QString& key, const int sample);
    void removeRow(const int index);
    void clear();

    void setApplication(Application* app);

private:
    Application* _app;
    QList<QString>& _keys;
    QList<int>& _samples;

    // QAbstractItemModel interface
public:
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

signals:
    void updated();
};

#endif // PCMWIDGETTABLEMODEL_H
