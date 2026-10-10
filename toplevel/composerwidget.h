#ifndef COMPOSERWIDGET_H
#define COMPOSERWIDGET_H

#include <QMainWindow>

namespace Ui {
class ComposerWidget;
}

class ComposerWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit ComposerWidget(QWidget *parent = nullptr);
    ~ComposerWidget();

private:
    Ui::ComposerWidget *ui;
};

#endif // COMPOSERWIDGET_H
