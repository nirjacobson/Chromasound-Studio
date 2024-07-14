#include "romwidgettablemodel.h"

ROMWidgetTableModel::ROMWidgetTableModel(QObject* parent, Application* app, QList<QString>& keys, QList<int>& samples)
    : QAbstractTableModel(parent)
    , _keys(keys)
    , _samples(samples)
    , _app(app)
{

}

QVariant ROMWidgetTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical) {
        return QVariant();
    }

    switch (section) {
    case 0:
        return "Key";
    case 1:
        return "Sample";
    default:
        return "";
    }
}

int ROMWidgetTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return _keys.size();
}

int ROMWidgetTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant ROMWidgetTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::EditRole) {
        switch (index.column()) {
            case 0:
                return _keys[index.row()];
            case 1:
                return _samples[index.row()];
            default:
                break;
        }
    } else if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return _keys[index.row()];
            case 1:
                return ROM(_app->project().resolve(_app->project().romFile())).names()[_samples[index.row()]];
            default:
                break;
        }
    }


    return QVariant();
}

void ROMWidgetTableModel::insertRow()
{
    insertRow("C5", 0);
}

void ROMWidgetTableModel::insertRow(const QString& key, const int sample)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _keys.append(key);
    _samples.append(sample);
    endInsertRows();
}

void ROMWidgetTableModel::removeRow(const int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    _keys.remove(index);
    _samples.remove(index);
    endRemoveRows();
}

void ROMWidgetTableModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    _keys.clear();
    _samples.clear();
    endRemoveRows();
}

void ROMWidgetTableModel::setApplication(Application* app)
{
    _app = app;
}

Qt::ItemFlags ROMWidgetTableModel::flags(const QModelIndex& index) const
{
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool ROMWidgetTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    int keyBefore;
    int keyAfter;
    switch (index.column()) {
    case 0:
        _keys[index.row()] = value.toString();
        emit updated();
        return true;
    case 1:
        _samples[index.row()] = value.toInt();
        emit updated();
        return true;
    default:
        break;
    }

    return false;
}
