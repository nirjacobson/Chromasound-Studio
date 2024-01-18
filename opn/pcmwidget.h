#ifndef PCMWIDGET_H
#define PCMWIDGET_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMimeData>

#include "application.h"
#include "common/mdiarea/mdisubwindow.h"
#include "commands/setpcmchannelsettingscommand.h"

namespace Ui {
    class PCMWidget;
}

class PCMWidget : public QMainWindow
{
        Q_OBJECT

    public:
        explicit PCMWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~PCMWidget();

        void setSettings(PCMChannelSettings* settings);

        void doUpdate();

    private:
        Ui::PCMWidget *ui;
        Application* _app;

        PCMChannelSettings* _settings;

    private slots:
        void openTriggered();

        // QWidget interface
    protected:
        void closeEvent(QCloseEvent* event);
        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);
};

#endif // PCMWIDGET_H
