#ifndef ROMBUILDERDIALOG_H
#define ROMBUILDERDIALOG_H

#include <QMainWindow>
#include <QFileDialog>

#include "rombuildertablemodel.h"

namespace Ui {
class ROMBuilderDialog;
}

class ROMBuilderDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit ROMBuilderDialog(QWidget *parent = nullptr);
    ~ROMBuilderDialog();

private:
    Ui::ROMBuilderDialog *ui;

    QList<QString> _names;
    QList<QString> _paths;
    ROMBuilderTableModel _tableModel;

    QByteArray header() const;

    void recalculateSize();

private slots:
    void moveUp();
    void moveDown();
    void remove();
    void add();

    void save();
};

#endif // ROMBUILDERDIALOG_H
