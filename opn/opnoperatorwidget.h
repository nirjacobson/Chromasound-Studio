#ifndef OPNOPERATORWIDGET_H
#define OPNOPERATORWIDGET_H

#include <QWidget>

#include "application.h"
#include "project/channel/opnoperatorsettings.h"
#include "commands/editopnoperatorsettingscommand.h"

namespace Ui {
    class OPNOperatorWidget;
}

class OPNOperatorWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit OPNOperatorWidget(QWidget *parent = nullptr);
        ~OPNOperatorWidget();

        void setApplication(Application* app);

        void setSettings(OPNOperatorSettings* settings);

    private:
        Ui::OPNOperatorWidget *ui;
        Application* _app;

        OPNOperatorSettings* _settings;

        int dtToIndex(const int dt) const;

    private slots:
        void arDialChanged(const int value);
        void t1lDialChanged(const int value);
        void d1rDialChanged(const int value);
        void t2lDialChanged(const int value);
        void d2rDialChanged(const int value);
        void rrDialChanged(const int value);
        void mulChanged(const int idx);
        void dtChanged(const int idx);
        void rsDialChanged(const int value);
        void amLedClicked(bool);
        void resetEnvelopeSettings();
};

#endif // OPNOPERATORWIDGET_H
