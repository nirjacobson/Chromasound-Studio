#ifndef LFOWIDGET_H
#define LFOWIDGET_H

#include <QWidget>

#include "project/playlist.h"

namespace Ui {
    class LFOWidget;
}

class LFOWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit LFOWidget(QWidget *parent = nullptr);
        ~LFOWidget();

        int setting() const;
        void set(const int mode);

    signals:
        void changed();

    private:
        Ui::LFOWidget *ui;

    private slots:
        void modeComboBoxCurrentIndexChanged(const int index);
};

#endif // LFOWIDGET_H
