#ifndef MELODYGLOBALSWIDGET_H
#define MELODYGLOBALSWIDGET_H

#include <QMainWindow>

#include "application.h"
#include "common/mdiarea/mdisubwindow.h"
#include "commands/editoplsettingscommand.h"
#include "commands/editprojectoplltypecommand.h"

namespace Ui {
    class MelodyGlobalsWidget;
}

class MelodyGlobalsWidget : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MelodyGlobalsWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~MelodyGlobalsWidget();

        void setSettings(OPLSettings* settings);

        void doUpdate();
    private:
        Ui::MelodyGlobalsWidget *ui;
        Application* _app;
        OPLSettings* _settings;

    private slots:
        void patchsetChanged();
        void fmChanged();

        // QWidget interface
    protected:
        void closeEvent(QCloseEvent* event);
};

#endif // MELODYGLOBALSWIDGET_H
