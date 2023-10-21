#ifndef ALGORITHMWIDGET_H
#define ALGORITHMWIDGET_H

#include <QWidget>
#include <QCommonStyle>

#include "application.h"
#include "project/channel/algorithmsettings.h"
#include "commands/editalgorithmsettingscommand.h"

namespace Ui {
    class AlgorithmWidget;
}

class AlgorithmWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit AlgorithmWidget(QWidget *parent = nullptr);
        ~AlgorithmWidget();

        void setApplication(Application* app);
        void setSettings(AlgorithmSettings* settings);

    private:
        Ui::AlgorithmWidget *ui;
        Application* _app;
        AlgorithmSettings* _settings;

    private slots:
        void prevAlgorithm();
        void nextAlgorithm();
        void feedbackDialChanged(const int feedback);
};

#endif // ALGORITHMWIDGET_H
