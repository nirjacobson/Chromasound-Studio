#ifndef ROMBUILDERDIALOG_H
#define ROMBUILDERDIALOG_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QMimeData>

#include "rombuildertablemodel.h"
#include "common/mdiarea/mdisubwindow.h"

namespace Ui {
class ROMBuilderDialog;
}

class ROMBuilderDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit ROMBuilderDialog(QWidget *parent = nullptr);
    ~ROMBuilderDialog();

private:
    Ui::ROMBuilderDialog *ui;

    QList<QString> _names;
    QList<QString> _paths;
    ROMBuilderTableModel _tableModel;

    QByteArray header() const;

    void recalculateSize();

    void add_(const QString& path);

private slots:
    void moveUp();
    void moveDown();
    void remove();
    void add();

    void save();

    void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void closeEvent(QCloseEvent* event);
};

#endif // ROMBUILDERDIALOG_H
