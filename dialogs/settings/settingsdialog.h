#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

#include "FM_PSG_Studio.h"

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit SettingsDialog(QWidget *parent = nullptr);
        ~SettingsDialog();

    private:
        Ui::SettingsDialog *ui;

    private slots:
        void accepted();
};

#endif // SETTINGSDIALOG_H
