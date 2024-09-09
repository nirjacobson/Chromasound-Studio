#ifndef CHROMASOUNDLAYOUTWIDGET_H
#define CHROMASOUNDLAYOUTWIDGET_H

#include <QWidget>
#include <QSettings>

#include "Chromasound_Studio.h"

namespace Ui {
    class ChromasoundLayoutWidget;
}

class ChromasoundLayoutWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit ChromasoundLayoutWidget(QWidget *parent = nullptr);
        ~ChromasoundLayoutWidget();

        int quantity() const;
        QString chromasound1() const;
        QString chromasound2() const;

    signals:
        void changed();

    private:
        Ui::ChromasoundLayoutWidget *ui;

    private slots:
        void quantityChanged(int index);
        void playbackChanged(int index);
        void interactiveChanged(int index);
        void implementationChanged(int index);
};

#endif // CHROMASOUNDLAYOUTWIDGET_H
