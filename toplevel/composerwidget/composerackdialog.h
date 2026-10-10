#ifndef COMPOSERACKDIALOG_H
#define COMPOSERACKDIALOG_H

#include <QDialog>

namespace Ui {
class ComposerAckDialog;
}

class ComposerAckDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ComposerAckDialog(QWidget *parent = nullptr);
    ~ComposerAckDialog();

private:
    Ui::ComposerAckDialog *ui;
};

#endif // COMPOSERACKDIALOG_H
