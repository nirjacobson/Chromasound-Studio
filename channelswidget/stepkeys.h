#ifndef STEPKEYS_H
#define STEPKEYS_H

#include <QWidget>
#include <QPainter>
#include <QtMath>
#include <QWheelEvent>

#include "application.h"
#include "stepsequencerwidget.h"

class StepKeys : public QWidget
{
        Q_OBJECT
    public:
        explicit StepKeys(QWidget *parent = nullptr, Application* app = nullptr);

        void setChannel(const int idx);

    signals:
        void clicked(const Qt::MouseButton button, const int step, const int key);

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent*);

    private:
        static constexpr int KEYS_PER_OCTAVE = 12;
        static constexpr int WHITE_KEYS_PER_OCTAVE = 7;
        static constexpr int BLACK_KEYS_PER_OCTAVE = 5;

        static constexpr int ROWS = 12 * 8;
        static constexpr int ROW_HEIGHT = 7;

        static constexpr QColor HIGHLIGHT_COLOR = QColor(255, 192, 192);

        int _top;

        Application* _app;

        int _index;

    protected:
        void wheelEvent(QWheelEvent* event);
        void mousePressEvent(QMouseEvent* event);
};

#endif // STEPKEYS_H
