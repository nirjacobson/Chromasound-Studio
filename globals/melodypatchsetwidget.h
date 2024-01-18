#ifndef MELODYPATCHSETWIDGET_H
#define MELODYPATCHSETWIDGET_H

#include <QWidget>

#include "application.h"
#include "opl/opll.h"
#include "unselectablestringlistmodel.h"

namespace Ui {
class MelodyPatchsetWidget;
}

class MelodyPatchsetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MelodyPatchsetWidget(QWidget *parent = nullptr, Application* app = nullptr);
    ~MelodyPatchsetWidget();

    void setApplication(Application* app);

    OPLL::Type selectedPatchset() const;
    void setPatchset(const OPLL::Type type);

signals:
    void changed();

private:
    Ui::MelodyPatchsetWidget *ui;
    Application* _app;

    UnselectableStringListModel _listModel;

private slots:
    void patchsetChanged(const int index);
};

#endif // MELODYPATCHSETWIDGET_H
