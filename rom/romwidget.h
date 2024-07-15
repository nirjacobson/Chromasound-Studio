#ifndef ROMWIDGET_H
#define ROMWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QItemSelection>
#include <QLineEdit>
#include <QRegularExpressionValidator>

#include "application.h"
#include "formats/rom.h"
#include "commands/editromchannelsettingscommand.h"
#include "romwidgettablemodel.h"

namespace Ui {
class ROMWidget;
}

class ROMWidget : public QWidget
{
    Q_OBJECT

    class KeyItemDelegate : public QStyledItemDelegate {
        // QAbstractItemDelegate interface
    public:
        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        void setEditorData(QWidget* editor, const QModelIndex& index) const;
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
    };

    class SampleItemDelegate : public QStyledItemDelegate {
        // QAbstractItemDelegate interface
    public:
        SampleItemDelegate(QObject* parent, Application* app, Channel::Type romType);
        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

        void setApplication(Application* app);
    private:
        Application* _app;
        Channel::Type _romType;

        // QAbstractItemDelegate interface
    public:
        void setEditorData(QWidget* editor, const QModelIndex& index) const;
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
    };

public:
    explicit ROMWidget(QWidget *parent = nullptr, Application* app = nullptr, Channel::Type romType = Channel::Type::SPCM);
    ~ROMWidget();

    void setApplication(Application* app);

    void setSettings(ROMChannelSettings* settings);
    void doUpdate();

private:
    Ui::ROMWidget *ui;
    ROMWidgetTableModel _tableModel;
    KeyItemDelegate _keyDelegate;
    SampleItemDelegate _sampleDelegate;

    QList<QString> _keys;
    QList<int> _samples;

    Application* _app;

    Channel::Type _romType;
    ROMChannelSettings* _settings;

    static int stringToKey(const QString& str);
    static QString keyToString(const int key);

private slots:
    void tableModelUpdated();
    void addClicked();
    void removeClicked();
    void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private slots:
};

#endif // ROMWIDGET_H
