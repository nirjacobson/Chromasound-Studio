#ifndef SSGGLOBALSWIDGET_H
#define SSGGLOBALSWIDGET_H

#include <QWidget>

#include "application.h"
#include "common/mdiarea/mdisubwindow.h"
#include "commands/editprojectnoisefrequencycommand.h"
#include "commands/editprojectenvelopefrequencycommand.h"
#include "commands/editssgenvelopesettingscommand.h"

namespace Ui {
    class SSGGlobalsWidget;
}

class SSGGlobalsWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit SSGGlobalsWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~SSGGlobalsWidget();

        void doUpdate();

    private:
        Ui::SSGGlobalsWidget *ui;
        Application* _app;

    private slots:
        void envelopeShapeChanged();
        void envelopeFrequencyChanged();
        void noiseFrequencyChanged();

        // QWidget interface
    protected:
        void closeEvent(QCloseEvent* event);
};

#endif // SSGGLOBALSWIDGET_H
