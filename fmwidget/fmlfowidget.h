#ifndef FMLFOWIDGET_H
#define FMLFOWIDGET_H

#include <QWidget>

#include "application.h"
#include "commands/editlfosettingscommand.h"

namespace Ui {
class FMLFOWidget;
}

class FMLFOWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FMLFOWidget(QWidget *parent = nullptr);
    ~FMLFOWidget();

    void setApplication(Application* app);
    void setSettings(LFOSettings* settings);

private:
    Ui::FMLFOWidget *ui;
    Application* _app;
    LFOSettings* _settings;

private slots:
    void amsSliderChanged(const int value);
    void fmsSliderChanged(const int value);
};

#endif // FMLFOWIDGET_H
