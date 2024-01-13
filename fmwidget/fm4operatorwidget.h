#ifndef FM4OPERATORWIDGET_H
#define FM4OPERATORWIDGET_H

#include <QWidget>

#include "application.h"
#include "project/channel/fm4operatorsettings.h"
#include "commands/editfm4operatorsettingscommand.h"

namespace Ui {
    class FM4OperatorWidget;
}

class FM4OperatorWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit FM4OperatorWidget(QWidget *parent = nullptr);
        ~FM4OperatorWidget();

        void setApplication(Application* app);

        void setSettings(FM4OperatorSettings* settings);

    private:
        Ui::FM4OperatorWidget *ui;
        Application* _app;

        FM4OperatorSettings* _settings;

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

#endif // FM4OPERATORWIDGET_H
