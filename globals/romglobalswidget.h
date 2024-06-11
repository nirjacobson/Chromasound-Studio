#ifndef ROMGLOBALSWIDGET_H
#define ROMGLOBALSWIDGET_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QMimeData>

#include "application.h"
#include "formats/rom.h"
#include "romtablemodel.h"

#include "common/mdiarea/mdisubwindow.h"
#include "commands/setprojectromfilecommand.h"

namespace Ui {
class ROMGlobalsWidget;
}

class ROMGlobalsWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit ROMGlobalsWidget(QWidget *parent = nullptr, Application* app = nullptr);
    ~ROMGlobalsWidget();

    void doUpdate();

private:
    Ui::ROMGlobalsWidget *ui;
    Application* _app;

    ROM _rom;
    ROMTableModel _tableModel;

    void load(const QString& path);

private slots:
    void open();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent* event);

    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
};

#endif // ROMGLOBALSWIDGET_H
