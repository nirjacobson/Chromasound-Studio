#ifndef PIANOROLLQUANTIZATIONDIVISIONDIALOG_H
#define PIANOROLLQUANTIZATIONDIVISIONDIALOG_H

#include <QDialog>

namespace Ui {
class PianoRollQuantizationDivisionDialog;
}

class PianoRollQuantizationDivisionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PianoRollQuantizationDivisionDialog(QWidget *parent = nullptr);
    ~PianoRollQuantizationDivisionDialog();

    int denominator() const;
    void setDenominator(int d);

private:
    Ui::PianoRollQuantizationDivisionDialog *ui;

    int _denominator;

private slots:
    void denominatorChanged(int d);
};

#endif // PIANOROLLQUANTIZATIONDIVISIONDIALOG_H
