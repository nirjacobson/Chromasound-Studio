#ifndef OPNWIDGETWINDOW_H
#define OPNWIDGETWINDOW_H

#include <QMainWindow>

#include "application.h"

namespace Ui {
    class FMWidgetWindow;
}

class FMWidgetWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit FMWidgetWindow(QWidget *parent = nullptr, Application* app = nullptr);
        ~FMWidgetWindow();

        void setSettings(FMChannelSettings* settings);

        void pressKey(const int key);
        void releaseKey(const int key);

        void doUpdate();

    signals:
        void keyPressed(const int key, const int velocity);
        void keyReleased(const int key);

    private:
        Ui::FMWidgetWindow *ui;

        // QWidget interface
    protected:
        void closeEvent(QCloseEvent* event);
};

#endif // OPNWIDGETWINDOW_H
