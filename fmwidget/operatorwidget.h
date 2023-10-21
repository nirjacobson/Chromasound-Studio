#ifndef OPERATORWIDGET_H
#define OPERATORWIDGET_H

#include <QWidget>

#include "application.h"
#include "project/channel/operatorsettings.h"
#include "commands/editoperatorsettingscommand.h"

namespace Ui {
    class OperatorWidget;
}

class OperatorWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit OperatorWidget(QWidget *parent = nullptr);
        ~OperatorWidget();

        void setApplication(Application* app);

        void setSettings(OperatorSettings* settings);

    private:
        Ui::OperatorWidget *ui;
        Application* _app;

        OperatorSettings* _settings;

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

#endif // OPERATORWIDGET_H
