#ifndef PCMWIDGET_H
#define PCMWIDGET_H

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
#include "pcmwidgettablemodel.h"

namespace Ui {
class PCMWidget;
}

class PCMWidget : public QWidget
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
    explicit PCMWidget(QWidget *parent = nullptr, Application* app = nullptr);
    ~PCMWidget();

    void setApplication(Application* app);

    void setSettings(PCMChannelSettings* settings);
    void doUpdate();

private:
    Ui::PCMWidget *ui;
    PCMWidgetTableModel _tableModel;
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

#endif // PCMWIDGET_H
