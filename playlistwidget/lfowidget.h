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

        void setLFOChange(Playlist::LFOChange* change);

    private:
        Ui::LFOWidget *ui;

        Playlist::LFOChange* _lfoChange;

    private slots:
        void modeComboBoxCurrentIndexChanged(const int index);
};

#endif // LFOWIDGET_H
