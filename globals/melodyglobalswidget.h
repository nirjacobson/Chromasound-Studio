#ifndef MELODYGLOBALSWIDGET_H
#define MELODYGLOBALSWIDGET_H

#include <QMainWindow>

#include "application.h"
#include "mdiarea/mdisubwindow.h"

namespace Ui {
    class MelodyGlobalsWidget;
}

class MelodyGlobalsWidget : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MelodyGlobalsWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~MelodyGlobalsWidget();

        void setSettings(FM2Settings* settings);

        void doUpdate();
    private:
        Ui::MelodyGlobalsWidget *ui;
        Application* _app;
        FM2Settings* _settings;

    private slots:
        void fmChanged();

        // QWidget interface
    protected:
        void closeEvent(QCloseEvent* event);
};

#endif // MELODYGLOBALSWIDGET_H
