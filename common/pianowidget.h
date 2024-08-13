#ifndef PIANOWIDGET_H
#define PIANOWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

#include "damagewidget/damagewidget.h"

class PianoWidget : public DamageWidget
{
        Q_OBJECT
        Q_PROPERTY(QColor outlineColor READ outlineColor WRITE setOutlineColor)
        Q_PROPERTY(QColor whiteKeyColor READ whiteKeyColor WRITE setWhiteKeyColor)
        Q_PROPERTY(QColor blackKeyColor READ blackKeyColor WRITE setBlackKeyColor)
        Q_PROPERTY(QColor activeKeyColor READ activeKeyColor WRITE setActiveKeyColor)
        Q_PROPERTY(QColor headerColor READ headerColor WRITE setHeaderColor)
        Q_PROPERTY(QColor headerTextColor READ headerTextColor WRITE setHeaderTextColor)

    public:
        explicit PianoWidget(QWidget *parent = nullptr);

        void pressKey(const int key);
        void releaseKey(const int key);

    signals:
        void keyPressed(const char key, const int velocity);
        void keyReleased(const char key);

    protected:
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);

    private:

        static int WHITE_KEY_INTERVALS[7];
        static int BLACK_KEY_INTERVALS[5];

        static constexpr float WHITE_WIDTH_RATIO = 21.0f / 140.0f;
        static constexpr float BLACK_WIDTH_RATIO = 15.0f / 140.0f;
        static constexpr float BLACK_HEIGHT_RATIO = 90.0f / 140.0f;
        static constexpr int DEFAULT_BASE_OCTAVE = 2;
        static constexpr int KEYS_PER_OCTAVE = 12;
        static constexpr int WHITE_KEYS_PER_OCTAVE = 7;
        static constexpr int BLACK_KEYS_PER_OCTAVE = 5;
        static constexpr int HEADER_HEIGHT = 24;

        int _baseOctave;
        QList<int> _pressedKeys;

        void drawBlackKey(const int octave, const int key, QPaintEvent* event, QPainter& painter);
        void drawBlackKeys(const int octave, QPaintEvent* event, QPainter& painter);
        void drawWhiteKeys(const int octave, QPaintEvent* event, QPainter& painter);
        void drawOctaveHeader(const int octave, QPaintEvent* event, QPainter& painter);
        void drawOctave(const int octave, QPaintEvent* event, QPainter& painter);
        void drawOctaves(QPaintEvent* event, QPainter& painter);

        int keyAt(const QPoint& pos) const;

        QColor _outlineColor;
        QColor _whiteKeyColor;
        QColor _blackKeyColor;
        QColor _activeKeyColor;
        QColor _headerColor;
        QColor _headerTextColor;

        const QColor& outlineColor() const;
        const QColor& whiteKeyColor() const;
        const QColor& blackKeyColor() const;
        const QColor& activeKeyColor() const;
        const QColor& headerColor() const;
        const QColor& headerTextColor() const;

        void setOutlineColor(const QColor& color);
        void setWhiteKeyColor(const QColor& color);
        void setBlackKeyColor(const QColor& color);
        void setActiveKeyColor(const QColor& color);
        void setHeaderColor(const QColor& color);
        void setHeaderTextColor(const QColor& color);

        // DamageWidget interface
    private:
        void paintFull(QPaintEvent* event);
        void paintPartial(QPaintEvent* event);
};

#endif // PIANOWIDGET_H
