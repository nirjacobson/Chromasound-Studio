#ifndef ROMWIDGET_H
#define ROMWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QItemSelection>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QFileDialog>
#include <QDropEvent>
#include <QMimeData>

#include "application.h"
#include "formats/rom.h"
#include "formats/bson.h"
#include "commands/editpcmchannelsettingscommand.h"
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
        SampleItemDelegate(QObject* parent, Application* app);
        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

        void setApplication(Application* app);
    private:
        Application* _app;

        // QAbstractItemDelegate interface
    public:
        void setEditorData(QWidget* editor, const QModelIndex& index) const;
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
    };

public:
    explicit ROMWidget(QWidget *parent = nullptr, Application* app = nullptr);
    ~ROMWidget();

    void setApplication(Application* app);

    void setSettings(PCMChannelSettings* settings);
    void doUpdate();

private:
    Ui::ROMWidget *ui;
    ROMWidgetTableModel _tableModel;
    KeyItemDelegate _keyDelegate;
    SampleItemDelegate _sampleDelegate;

    QList<QString> _keys;
    QList<int> _samples;

    Application* _app;

    PCMChannelSettings* _settings;

    static int stringToKey(const QString& str);
    static QString keyToString(const int key);

private slots:
    void tableModelUpdated();
    void addClicked();
    void removeClicked();
    void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

public slots:
    void saveTriggered();
    void openTriggered();

private slots:

    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
};

#endif // ROMWIDGET_H
