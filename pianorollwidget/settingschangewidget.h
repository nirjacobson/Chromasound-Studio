#ifndef SETTINGSCHANGEWIDGET_H
#define SETTINGSCHANGEWIDGET_H

#include <QWidget>

#include "application.h"

namespace Ui {
class SettingsChangeWidget;
}

class SettingsChangeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsChangeWidget(QWidget *parent = nullptr);
    ~SettingsChangeWidget();

    void setApplication(Application* app);

    void pressKey(const int key);
    void releaseKey(const int key);

    void doUpdate();

    void setSettings(ChannelSettings& settings);

signals:
    void removeClicked();
    void doneClicked();

    void keyOn(const int key, const int velocity);
    void keyOff(const int key);

private:
    Ui::SettingsChangeWidget *ui;
};

#endif // SETTINGSCHANGEWIDGET_H
