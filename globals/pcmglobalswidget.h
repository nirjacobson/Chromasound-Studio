#ifndef PCMGLOBALSWIDGET_H
#define PCMGLOBALSWIDGET_H

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

class PCMGlobalsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PCMGlobalsWidget(QWidget *parent = nullptr, Application* app = nullptr, const QString& romFilePath = "");
    ~PCMGlobalsWidget();

    void setApplication(Application* app);

    void setROMFile(const QString& path);
    const QString& romFile() const;

    void open();
    void reset();

    void doUpdate();

signals:
    void updated();

private:
    Ui::ROMGlobalsWidget *ui;
    Application* _app;

    QString _romPath;
    ROM _rom;
    ROMTableModel _tableModel;

    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
};

#endif // PCMGLOBALSWIDGET_H
