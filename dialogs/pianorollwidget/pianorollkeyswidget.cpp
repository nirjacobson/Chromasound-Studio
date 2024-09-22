#include "pianorollkeyswidget.h"

PianoRollKeysWidget::PianoRollKeysWidget(QWidget* parent, Application* app)
    : GanttLeftWidget(parent)
    , _app(app)
    , _rows(0)
    , _top(0)
    , _rowHeight(16)
    , _outlineColor(QColor(Qt::gray))
    , _whiteKeyColor(QColor(Qt::white))
    , _blackKeyColor(QColor(Qt::black))
    , _activeKeyColor(QColor(255, 192, 192))
    , _romChannelSettings(nullptr)
{
    setMinimumWidth(128);
    setMaximumWidth(128);
}

float PianoRollKeysWidget::getScrollPercentage()
{
    int scrollHeight = qMax(0, length() - height());

    return (float)_top / (float)scrollHeight;
}
void PianoRollKeysWidget::setScrollPercentage(const float percent)
{
    int scrollHeight = qMax(0, length() - height());
    _top = percent * scrollHeight;

    setNeedsFullPaint();
    update();
}

void PianoRollKeysWidget::scrollBy(const int pixels)
{
    _top += pixels;
    setNeedsFullPaint();
    update();
}

void PianoRollKeysWidget::setRows(int rows)
{
    _rows = rows;
}

void PianoRollKeysWidget::setRowHeight(int height)
{
    _rowHeight = height;
}

void PianoRollKeysWidget::pressKey(const int key)
{
    _onKeys.append(key);
    update();
}

void PianoRollKeysWidget::releaseKey(const int key)
{
    _onKeys.removeAll(key);
    setNeedsFullPaint();
    update();
}

void PianoRollKeysWidget::setROMChannelSettings(const ROMChannelSettings* settings)
{
    _romChannelSettings = settings;
}

int PianoRollKeysWidget::length() const
{
    return _rowHeight * _rows;
}

void PianoRollKeysWidget::mousePressEvent(QMouseEvent* event)
{
    int absBottom = _rowHeight * _rows;
    int bottom = _top + height() + 1;

    int octaveHeight = _rowHeight * KEYS_PER_OCTAVE;
    int whiteKeyWidth = (float)octaveHeight / 7.0f;

    int bottomPosition = absBottom - bottom;

    int mousePosition = height() - event->pos().y();
    int mousePositionIntoOctave = (bottomPosition + mousePosition) % octaveHeight;
    int mouseOctave = (bottomPosition + mousePosition) / octaveHeight;

    int key;
    if (event->pos().x() >= width()/2) {
        key = (mousePositionIntoOctave / whiteKeyWidth);
        key *= 2;
        if (key >= 6) {
            key--;
        }
        key = (mouseOctave * 12) + key;
    } else {
        key = (mouseOctave * 12) + (mousePositionIntoOctave / _rowHeight);
    }

    _onKeys.append(key);

    update();

    emit keyOn(key, 100);
}

void PianoRollKeysWidget::mouseReleaseEvent(QMouseEvent* event)
{
    int absBottom = _rowHeight * _rows;
    int bottom = _top + height() + 1;

    int octaveHeight = _rowHeight * KEYS_PER_OCTAVE;
    int whiteKeyWidth = (float)octaveHeight / 7.0f;

    int bottomPosition = absBottom - bottom;

    int mousePosition = height() - event->pos().y();
    int mousePositionIntoOctave = (bottomPosition + mousePosition) % octaveHeight;
    int mouseOctave = (bottomPosition + mousePosition) / octaveHeight;

    int key;
    if (event->pos().x() >= width()/2) {
        key = (mousePositionIntoOctave / whiteKeyWidth);
        key *= 2;
        if (key >= 6) {
            key--;
        }
        key = (mouseOctave * 12) + key;
    } else {
        key = (mouseOctave * 12) + (mousePositionIntoOctave / _rowHeight);
    }

    _onKeys.removeAll(key);

    setNeedsFullPaint();
    update();

    emit keyOff(key);
}

const QColor& PianoRollKeysWidget::outlineColor() const
{
    return _outlineColor;
}

const QColor& PianoRollKeysWidget::whiteKeyColor() const
{
    return _whiteKeyColor;
}

const QColor& PianoRollKeysWidget::blackKeyColor() const
{
    return _blackKeyColor;
}

const QColor& PianoRollKeysWidget::activeKeyColor() const
{
    return _activeKeyColor;
}

void PianoRollKeysWidget::setOutlineColor(const QColor& color)
{
    _outlineColor = color;
}

void PianoRollKeysWidget::setWhiteKeyColor(const QColor& color)
{
    _whiteKeyColor = color;
}

void PianoRollKeysWidget::setBlackKeyColor(const QColor& color)
{
    _blackKeyColor = color;
}

void PianoRollKeysWidget::setActiveKeyColor(const QColor& color)
{
    _activeKeyColor = color;
}

void PianoRollKeysWidget::paintFull(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(_outlineColor);

    int absBottom = _rowHeight * _rows;
    int bottom = _top + height() + 1;

    int octaveHeight = _rowHeight * KEYS_PER_OCTAVE;
    int whiteKeyWidth = (float)octaveHeight / 7.0f;

    int bottomPosition = absBottom - bottom;
    int bottomPositionIntoOctave = bottomPosition % octaveHeight;
    int bottomOctave = bottomPosition / octaveHeight;

    int octaveStart = height() + bottomPositionIntoOctave;
    int numOctavesAcrossHeight = qCeil((float)height()/(octaveHeight)) + 1;

    QPoint topLeft = QPoint(0, octaveStart - whiteKeyWidth);

    for (int i = 0; i < numOctavesAcrossHeight; i++) {
        for (int j = 0; j < WHITE_KEYS_PER_OCTAVE; j++) {
            int octave = bottomOctave + i;
            int key = j;
            key *= 2;
            if (key >= 6) key--;
            key = (octave * 12) + key;

            if (_onKeys.contains(key)) {
                painter.setBrush(_activeKeyColor);
            } else {
                painter.setBrush(_whiteKeyColor);
            }

            QPoint thisTopLeft = topLeft - QPoint(0, (j * whiteKeyWidth));
            QRect rect(thisTopLeft, thisTopLeft + QPoint(width() - 2, whiteKeyWidth));
            painter.fillRect(rect, painter.brush());
            painter.drawRect(rect);

            if (j == 0) {
                QTextOption textOption;
                textOption.setAlignment(Qt::AlignRight);
                painter.drawText(rect.adjusted(0, 8, -4, 0), QString("C%1").arg(bottomOctave + i), textOption);
            }
        }

        topLeft -= QPoint(0, octaveHeight);
    }

    painter.setPen(_blackKeyColor);

    topLeft = QPoint(0, octaveStart - 2 * _rowHeight);

    for (int i = 0; i < numOctavesAcrossHeight; i++) {
        for (int j = 0; j < BLACK_KEYS_PER_OCTAVE; j++) {
            int octave = bottomOctave + i;
            int key = j;
            key = (key * 2) + 1;
            if (key > 3) key++;
            key = (octave * 12) + key;

            if (_onKeys.contains(key)) {
                painter.setBrush(_activeKeyColor);
            } else {
                painter.setBrush(_blackKeyColor);
            }

            int offset = j > 1 ? 1 : 0;
            QPoint thisTopLeft = topLeft - QPoint(0, (offset + (j * 2)) * _rowHeight);
            QRect rect(thisTopLeft, thisTopLeft + QPoint(width()/2, _rowHeight));
            painter.fillRect(rect, painter.brush());
            painter.drawRect(rect);
        }

        topLeft -= QPoint(0, octaveHeight);
    }

    painter.setPen(_outlineColor);

    topLeft = QPoint(0, octaveStart - _rowHeight);

    for (int i = 0; i < numOctavesAcrossHeight; i++) {
        for (int j = 0; j < 12; j++) {
            int octave = bottomOctave + i;
            int key = octave * 12 + j;

            QPoint thisTopLeft = topLeft - QPoint(0, j * _rowHeight);
            QRect rect(thisTopLeft, thisTopLeft + QPoint(width()/2, _rowHeight));

            ROM rom(_app->project().resolve(_app->project().pcmFile()));

            QFont font = painter.font();

            if (_romChannelSettings
                && !rom.names().empty()
                && _romChannelSettings->keySampleMappings().contains(key)) {

                QFont newFont = font;
                font.setPointSize(8);

                painter.setFont(newFont);

                QString sampleName = rom.names()[_romChannelSettings->keySampleMappings()[key]];
                painter.drawText(rect.adjusted(4, 0, 0, 0), sampleName);

                painter.setFont(font);
            }
        }

        topLeft -= QPoint(0, octaveHeight);
    }
}

void PianoRollKeysWidget::paintPartial(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(_outlineColor);

    int absBottom = _rowHeight * _rows;
    int bottom = _top + height() + 1;

    int octaveHeight = _rowHeight * KEYS_PER_OCTAVE;
    int whiteKeyWidth = (float)octaveHeight / 7.0f;

    int bottomPosition = absBottom - bottom;
    int bottomPositionIntoOctave = bottomPosition % octaveHeight;
    int bottomOctave = bottomPosition / octaveHeight;

    int octaveStart = height() + bottomPositionIntoOctave;

    auto drawKey = [&](int onKey){
        int octave = onKey / 12;
        int interval = onKey % 12;

        QPoint topLeft;

        QPoint thisTopLeft;
        QRect rect;
        int whiteKey;
        int blackKey;
        switch (interval) {
            case 0:
            case 2:
            case 4:
            case 5:
            case 7:
            case 9:
            case 11:
                topLeft = QPoint(0, octaveStart - whiteKeyWidth - (octave - bottomOctave) * octaveHeight);
                whiteKey = interval;
                if (whiteKey > 4) whiteKey++;
                whiteKey /= 2;

                thisTopLeft = topLeft - QPoint(0, (whiteKey * whiteKeyWidth));
                rect = QRect(thisTopLeft, thisTopLeft + QPoint(width() - 2, whiteKeyWidth));
                painter.fillRect(rect, painter.brush());
                painter.drawRect(rect);

                if (onKey % 12 == 0) {
                    QTextOption textOption;
                    textOption.setAlignment(Qt::AlignRight);
                    painter.drawText(rect.adjusted(0, 8, -4, 0), QString("C%1").arg(octave), textOption);
                }
                break;
            case 1:
            case 3:
            case 6:
            case 8:
            case 10:
                topLeft = QPoint(0, octaveStart - (_rowHeight * 2) - (octave - bottomOctave) * octaveHeight);
                blackKey = interval;
                if (blackKey > 3) blackKey--;
                blackKey /= 2;

                int offset = blackKey > 1 ? 1 : 0;
                QPoint thisTopLeft = topLeft - QPoint(0, (offset + (blackKey * 2)) * _rowHeight);
                QRect rect(thisTopLeft, thisTopLeft + QPoint(width()/2, _rowHeight));
                painter.fillRect(rect, painter.brush());
                painter.drawRect(rect);
                break;
        }
    };

    for (int onKey : _onKeys) {
        int interval = onKey % 12;

        QPoint topLeft;

        QPoint thisTopLeft;
        QRect rect;
        int whiteKey;
        int blackKey;
        switch (interval) {
            case 0:
            case 5:
                painter.setBrush(_activeKeyColor);
                drawKey(onKey);
                painter.setBrush(_blackKeyColor);
                drawKey(onKey+1);
                break;
            case 2:
            case 7:
            case 9:
                painter.setBrush(_activeKeyColor);
                drawKey(onKey);
                painter.setBrush(_blackKeyColor);
                drawKey(onKey-1);
                drawKey(onKey+1);
                break;
            case 4:
            case 11:
                painter.setBrush(_activeKeyColor);
                drawKey(onKey);
                painter.setBrush(_blackKeyColor);
                drawKey(onKey-1);
                break;
            default:
                painter.setBrush(_activeKeyColor);
                drawKey(onKey);
                break;
        }
    }
}
