#ifndef MIDIPADSDIALOG_H
#define MIDIPADSDIALOG_H

#include <QDialog>

namespace Ui {
class MIDIPadsDialog;
}

class MIDIPadsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MIDIPadsDialog(QWidget *parent = nullptr);
    ~MIDIPadsDialog();

    void pad(quint8 id);

    QString setting() const;

private:
    Ui::MIDIPadsDialog *ui;
    int _n;
    QString _setting;
    bool _recording;

private slots:
    void recordTriggered();
};

#endif // MIDIPADSDIALOG_H
