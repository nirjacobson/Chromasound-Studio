#ifndef PIANOROLLKEYSWIDGET_H
#define PIANOROLLKEYSWIDGET_H

#include "ganttwidget/ganttleftwidget.h"
#include <QtMath>
#include <QPainter>

class PianoRollKeysWidget : public GanttLeftWidget
{
        Q_OBJECT
    public:
        explicit PianoRollKeysWidget(QWidget *parent = nullptr);

        void setScrollPercentage(const float percent);

        void setRows(int rows);
        void setRowHeight(int height);

    protected:
        void paintEvent(QPaintEvent* event);

    private:
        static constexpr int KEYS_PER_OCTAVE = 12;
        static constexpr int WHITE_KEYS_PER_OCTAVE = 7;
        static constexpr int BLACK_KEYS_PER_OCTAVE = 5;

        int _rows;
        int _top;
        int _rowHeight;
};

#endif // PIANOROLLKEYSWIDGET_H
