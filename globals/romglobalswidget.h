#ifndef ROMGLOBALSWIDGET_H
#define ROMGLOBALSWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QMimeData>

#include "application.h"
#include "formats/rom.h"
#include "romtablemodel.h"


namespace Ui {
class ROMGlobalsWidget;
}

class ROMGlobalsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ROMGlobalsWidget(QWidget *parent = nullptr, Application* app = nullptr, const QString& romFilePath = "");
    ~ROMGlobalsWidget();

    void setApplication(Application* app);

    void setROMFile(const QString& path);
    const QString& romFile() const;

    void doUpdate();

signals:
    void updated();

private:
    Ui::ROMGlobalsWidget *ui;
    Application* _app;

    QString _romPath;
    ROM _rom;
    ROMTableModel _tableModel;

private slots:
    void open();
    void resetTriggered();

    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
};

#endif // ROMGLOBALSWIDGET_H
