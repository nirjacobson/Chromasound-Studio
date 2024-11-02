#ifndef PIANOROLLKEYSWIDGET_H
#define PIANOROLLKEYSWIDGET_H

#include <QtMath>
#include <QPainter>
#include <QMouseEvent>

#include "application.h"
#include "common/ganttwidget/ganttleftwidget.h"

class PianoRollKeysWidget : public GanttLeftWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor outlineColor READ outlineColor WRITE setOutlineColor)
        Q_PROPERTY(QColor whiteKeyColor READ whiteKeyColor WRITE setWhiteKeyColor)
        Q_PROPERTY(QColor blackKeyColor READ blackKeyColor WRITE setBlackKeyColor)
        Q_PROPERTY(QColor activeKeyColor READ activeKeyColor WRITE setActiveKeyColor)

    public:
        explicit PianoRollKeysWidget(QWidget *parent = nullptr, Application* app = nullptr);

        float getScrollPercentage();
        void setScrollPercentage(const float percent);
        void scrollBy(const int pixels);

        void setRows(int rows);
        void setRowHeight(int height);

        void pressKey(const int key);
        void releaseKey(const int key);

        void setROMChannelSettings(const PCMChannelSettings* settings);

    signals:
        void keyOn(const int key, const int velocity);
        void keyOff(const int key);

    protected:
        int length() const;

        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent*event);
    private:
        static constexpr int KEYS_PER_OCTAVE = 12;
        static constexpr int WHITE_KEYS_PER_OCTAVE = 7;
        static constexpr int BLACK_KEYS_PER_OCTAVE = 5;

        Application* _app;

        const PCMChannelSettings* _romChannelSettings;

        int _rows;
        int _top;
        int _rowHeight;

        QList<int> _onKeys;

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

        // DamageWidget interface
    private:
        void paintFull(QPaintEvent* event);
        void paintPartial(QPaintEvent* event);
};

#endif // PIANOROLLKEYSWIDGET_H
