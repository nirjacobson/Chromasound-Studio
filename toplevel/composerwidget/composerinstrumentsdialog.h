#ifndef COMPOSERINSTRUMENTSDIALOG_H
#define COMPOSERINSTRUMENTSDIALOG_H

#include <QDialog>

namespace Ui {
class ComposerInstrumentsDialog;
}

class ComposerInstrumentsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ComposerInstrumentsDialog(QWidget *parent = nullptr);
    ~ComposerInstrumentsDialog();

private:
    Ui::ComposerInstrumentsDialog *ui;
};

#endif // COMPOSERINSTRUMENTSDIALOG_H
