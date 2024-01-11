#ifndef SSGENVELOPESHAPEWIDGET_H
#define SSGENVELOPESHAPEWIDGET_H

#include <QWidget>

#include "ssg/ssgenvelopesettings.h"

namespace Ui {
class SSGEnvelopeShapeWidget;
}

class SSGEnvelopeShapeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SSGEnvelopeShapeWidget(QWidget *parent = nullptr);
    ~SSGEnvelopeShapeWidget();

private:
    Ui::SSGEnvelopeShapeWidget *ui;
    SSGEnvelopeSettings _settings;

private slots:
    void contToggled(const bool enabled);
    void attToggled(const bool enabled);
    void altToggled(const bool enabled);
    void holdToggled(const bool enabled);
};

#endif // SSGENVELOPESHAPEWIDGET_H
