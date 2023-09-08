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

        float getScrollPercentage();
        void setScrollPercentage(const float percent);
        void scrollBy(const int pixels);

        void setRows(int rows);
        void setRowHeight(int height);

    protected:
        int length() const;
        void paintEvent(QPaintEvent*);

    private:
        static constexpr int KEYS_PER_OCTAVE = 12;
        static constexpr int WHITE_KEYS_PER_OCTAVE = 7;
        static constexpr int BLACK_KEYS_PER_OCTAVE = 5;

        int _rows;
        int _top;
        int _rowHeight;
};

#endif // PIANOROLLKEYSWIDGET_H
