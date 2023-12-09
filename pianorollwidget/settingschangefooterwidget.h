#ifndef SETTINGSCHANGEFOOTERWIDGET_H
#define SETTINGSCHANGEFOOTERWIDGET_H

#include <QWidget>

#include "project/channel/channelsettings.h"

namespace Ui {
class SettingsChangeFooterWidget;
}

class SettingsChangeFooterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsChangeFooterWidget(QWidget *parent = nullptr);
    ~SettingsChangeFooterWidget();

    void setSettings(ChannelSettings& settings);

signals:
    void removeClicked();
    void doneClicked();

private:
    Ui::SettingsChangeFooterWidget *ui;
    ChannelSettings* _settings;

private slots:
    void volumeChanged(int volume);
};

#endif // SETTINGSCHANGEFOOTERWIDGET_H
