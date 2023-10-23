#ifndef STEPKEYSWIDGET_H
#define STEPKEYSWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QtMath>
#include <QWheelEvent>

#include "application.h"
#include "stepsequencerwidget.h"

class StepKeysWidget : public QWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor outlineColor READ outlineColor WRITE setOutlineColor)
        Q_PROPERTY(QColor whiteKeyColor READ whiteKeyColor WRITE setWhiteKeyColor)
        Q_PROPERTY(QColor blackKeyColor READ blackKeyColor WRITE setBlackKeyColor)
        Q_PROPERTY(QColor activeKeyColor READ activeKeyColor WRITE setActiveKeyColor)

    public:
        explicit StepKeysWidget(QWidget *parent = nullptr, Application* app = nullptr);

        void setApplication(Application* app);

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

        int _top;

        Application* _app;

        int _index;

        QColor _outlineColor;
        QColor _whiteKeyColor;
        QColor _blackKeyColor;
        QColor _activeKeyColor;

        const QColor& outlineColor() const;
        const QColor& whiteKeyColor() const;
        const QColor& blackKeyColor() const;
        const QColor& activeKeyColor() const;

        void setOutlineColor(const QColor& color);
        void setWhiteKeyColor(const QColor& color);
        void setBlackKeyColor(const QColor& color);
        void setActiveKeyColor(const QColor& color);

    protected:
        void wheelEvent(QWheelEvent* event);
        void mousePressEvent(QMouseEvent* event);
};

#endif // STEPKEYSWIDGET_H
