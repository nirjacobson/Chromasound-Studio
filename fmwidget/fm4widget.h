#ifndef FM4WIDGET_H
#define FM4WIDGET_H

#include <QWidget>

#include <QFileDialog>
#include <QStyleOption>
#include <QMimeData>

#include "application.h"
#include "project/channel/fmchannelsettings.h"
#include "bson.h"
#include "commands/setfmchannelsettingscommand.h"
#include "mdiarea/mdisubwindow.h"

namespace Ui {
class FM4Widget;
}
    
    class FM4Widget : public QWidget
{
        Q_OBJECT

    public:
        explicit FM4Widget(QWidget *parent = nullptr, Application* app = nullptr);
        ~FM4Widget();

        void setApplication(Application* app);

        void setSettings(FMChannelSettings* settings);

        void pressKey(const int key);
        void releaseKey(const int key);

        void doUpdate();

    signals:
        void keyPressed(const char key, const int velocity);
        void keyReleased(const char key);

    private:
        Ui::FM4Widget *ui;
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

#endif // FM4WIDGET_H
