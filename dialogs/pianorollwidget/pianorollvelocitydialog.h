#ifndef PIANOROLLVELOCITYDIALOG_H
#define PIANOROLLVELOCITYDIALOG_H

#include <QDialog>

namespace Ui {
class PianoRollVelocityDialog;
}

class PianoRollVelocityDialog : public QDialog
{
    Q_OBJECT

public:
    enum Operation {
        Set,
        Scale,
        Add
    };

    explicit PianoRollVelocityDialog(QWidget *parent = nullptr);
    ~PianoRollVelocityDialog();

    Operation operation() const;
    int value() const;
    double doubleValue() const;

    void setOperation(const Operation op);
    void setValue(const int val);

private:
    Ui::PianoRollVelocityDialog *ui;

private slots:
    void radioButtonClicked(bool checked);
};

#endif // PIANOROLLVELOCITYDIALOG_H
