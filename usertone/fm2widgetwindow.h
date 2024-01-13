#ifndef FM2WIDGETWINDOW_H
#define FM2WIDGETWINDOW_H

#include <QMainWindow>

#include "application.h"
#include "mdiarea/mdisubwindow.h"

namespace Ui {
    class FM2WidgetWindow;
}

class FM2WidgetWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit FM2WidgetWindow(QWidget *parent = nullptr, Application* app = nullptr);
        ~FM2WidgetWindow();

        void setSettings(FM2Settings* settings);

        void doUpdate();
    private:
        Ui::FM2WidgetWindow *ui;
        Application* _app;
        FM2Settings* _settings;

    private slots:
        void fmChanged();

        // QWidget interface
    protected:
        void closeEvent(QCloseEvent* event);
};

#endif // FM2WIDGETWINDOW_H
