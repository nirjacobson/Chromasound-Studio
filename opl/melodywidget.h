#ifndef MELODYWIDGET_H
#define MELODYWIDGET_H

#include <QWidget>

#include "application.h"
#include "project/channel/melodychannelsettings.h"
#include "commands/editmelodychannelsettingscommand.h"
#include "opl/opll.h"

namespace Ui {
    class MelodyWidget;
}

class MelodyWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit MelodyWidget(QWidget *parent = nullptr, Application* app = nullptr);
        ~MelodyWidget();

        void setApplication(Application* app);

        void setSettings(MelodyChannelSettings* settings);
        void doUpdate();

    private:
        Ui::MelodyWidget *ui;
        Application* _app;
        MelodyChannelSettings* _settings;

    private slots:
        void selectionChanged(int index);

};

#endif // MELODYWIDGET_H
