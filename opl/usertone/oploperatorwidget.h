#ifndef OPLOPERATORWIDGET_H
#define OPLOPERATORWIDGET_H

#include <QWidget>

#include "application.h"
#include "oploperatorsettings.h"

namespace Ui {
    class OPLOperatorWidget;
}

class OPLOperatorWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit OPLOperatorWidget(QWidget *parent = nullptr);
        ~OPLOperatorWidget();

        const OPLOperatorSettings& settings() const;
        void set(const OPLOperatorSettings& settings);

    signals:
        void changed();

    private:
        Ui::OPLOperatorWidget *ui;

        OPLOperatorSettings _settings;

    private slots:
        void envTypeChanged(const int index);
        void arDialChanged(const int value);
        void drDialChanged(const int value);
        void slDialChanged(const int value);
        void rrDialChanged(const int value);
        void multiDialChanged(const int value);
        void kslDialChanged(const int value);
        void amChanged();
        void vibChanged();
        void ksrChanged();
        void dChanged();
};

#endif // OPLOPERATORWIDGET_H
