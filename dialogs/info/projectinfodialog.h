#ifndef PROJECTINFODIALOG_H
#define PROJECTINFODIALOG_H

#include <QDialog>

#include "application.h"
#include "commands/setprojectinfocommand.h"

namespace Ui {
    class ProjectInfoWidget;
}

class ProjectInfoDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit ProjectInfoDialog(QWidget *parent = nullptr, Application* app = nullptr);
        ~ProjectInfoDialog();

        void doUpdate();

    private:
        Ui::ProjectInfoWidget *ui;
        Application* _app;

    private slots:
        void accepted();
};

#endif // PROJECTINFODIALOG_H
