#ifndef OPNWIDGET_H
#define OPNWIDGET_H

#include <QWidget>

#include <QFileDialog>
#include <QStyleOption>
#include <QMimeData>

#include "application.h"
#include "project/channel/fmchannelsettings.h"
#include "formats/bson.h"
#include "commands/setfmchannelsettingscommand.h"
#include "common/mdiarea/mdisubwindow.h"

namespace Ui {
    class OPNWidget;
}

class OPNWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit OPNWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~OPNWidget();

        void setApplication(Application* app);

        void setSettings(FMChannelSettings* settings);

        void pressKey(const int key);
        void releaseKey(const int key);

        void doUpdate();

    signals:
        void keyPressed(const char key, const int velocity);
        void keyReleased(const char key);

    private:
        Ui::OPNWidget *ui;
        Application* _app;
        FMChannelSettings* _settings;

    public slots:
        void newTriggered();
        void openTriggered();
        void saveTriggered();

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);
};

#endif // OPNWIDGET_H
