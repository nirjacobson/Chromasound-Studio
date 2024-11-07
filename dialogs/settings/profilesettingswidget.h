#ifndef PROFILESETTINGSWIDGET_H
#define PROFILESETTINGSWIDGET_H

#include <QWidget>
#include <QSettings>

#include "Chromasound_Studio.h"

namespace Ui {
class ProfileSettingsWidget;
}

class ProfileSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProfileSettingsWidget(QWidget *parent = nullptr);
    ~ProfileSettingsWidget();

    QString device() const;
    Chromasound_Studio::Profile profile() const;

private:
    Ui::ProfileSettingsWidget *ui;

private slots:
    void comboBoxChanged(const int index);
    void deviceComboBoxChanged(const int index);
};

#endif // PROFILESETTINGSWIDGET_H
