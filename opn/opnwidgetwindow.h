#ifndef OPNWIDGETWINDOW_H
#define OPNWIDGETWINDOW_H

#include <QMainWindow>

#include "application.h"

namespace Ui {
    class OPNWidgetWindow;
}

class OPNWidgetWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit OPNWidgetWindow(QWidget *parent = nullptr, Application* app = nullptr);
        ~OPNWidgetWindow();

        void setSettings(FMChannelSettings* settings);

        void pressKey(const int key);
        void releaseKey(const int key);

        void doUpdate();

    signals:
        void keyPressed(const int key, const int velocity);
        void keyReleased(const int key);

    private:
        Ui::OPNWidgetWindow *ui;

        // QWidget interface
    protected:
        void closeEvent(QCloseEvent* event);
};

#endif // OPNWIDGETWINDOW_H
