#ifndef PROJECTINFOSCREENDIALOG_H
#define PROJECTINFOSCREENDIALOG_H

#include <QDialog>
#include "project/project.h"

namespace Ui {
class ProjectInfoScreenDialog;
}

class ProjectInfoScreenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectInfoScreenDialog(QWidget *parent, const Project::Info& info);
    ~ProjectInfoScreenDialog();

private:
    Ui::ProjectInfoScreenDialog *ui;
};

#endif // PROJECTINFOSCREENDIALOG_H
