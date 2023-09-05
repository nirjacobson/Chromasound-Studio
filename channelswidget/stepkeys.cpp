#include "stepkeys.h"

constexpr QColor StepKeys::HIGHLIGHT_COLOR;

StepKeys::StepKeys(QWidget *parent, Application* app)
    : QWidget{parent}
    , _top(2 * ROW_HEIGHT * KEYS_PER_OCTAVE)
    , _app(app)
    , _index(0)
{

}

void StepKeys::setChannel(const int idx)
{
    _index = idx;
}

void StepKeys::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    Pattern& pattern = _app->project().getPattern(_app->activePattern());

    float beatsPerStep = 0.25;

    int absBottom = ROW_HEIGHT * ROWS;
    int bottom = _top + height() + 1;

    int octaveHeight = ROW_HEIGHT * KEYS_PER_OCTAVE;
    int whiteKeyWidth = (float)octaveHeight / 7.0f;

    int bottomPosition = absBottom - bottom;
    int bottomPositionIntoOctave = bottomPosition % octaveHeight;
    int bottomOctave = bottomPosition / octaveHeight;

    int octaveStart = height() + bottomPositionIntoOctave;
    int numOctavesAcrossHeight = qCeil((float)height()/octaveHeight) + 1;

    int width = StepSequencerWidget::StepWidth + StepSequencerWidget::StepSpacing;

    int whiteKeys[] = { 0, 2, 4, 5, 7, 9, 11 };
    int blackKeys[] = { 1, 3, 6, 8, 10 };

    QFont font = painter.font();
    font.setPixelSize(8);
    painter.setFont(font);

    int numSteps = _app->project().beatsPerBar() * 4;

    int steps[numSteps];
    if (pattern.hasTrack(_index)) {
        Track& track = pattern.getTrack(_index);

        for (int i = 0; i < numSteps; i++) {
            auto it = std::find_if(track.items().begin(), track.items().end(), [=](Track::Item* const item){ return item->time() == beatsPerStep * i; });
            if (it != track.items().end()) {
                steps[i] = (*it)->note().key();
            } else {
                steps[i] = -1;
            }
        }
    } else {
        for (int i = 0; i < numSteps; i++) {
            steps[i] = -1;
        }
    }

    for (int x = 0; x < numSteps; x++) {
        painter.setPen(QColor(Qt::gray));
        painter.setBrush(QColor(Qt::white));

        QPoint topLeft = QPoint(x*width, octaveStart - whiteKeyWidth);

        for (int i = 0; i < numOctavesAcrossHeight; i++) {
            for (int j = 0; j < WHITE_KEYS_PER_OCTAVE; j++) {
                int key = ((bottomOctave + i) * KEYS_PER_OCTAVE) + whiteKeys[j];
                QPoint thisTopLeft = topLeft - QPoint(0, (j*whiteKeyWidth));
                QRect rect(thisTopLeft, thisTopLeft + QPoint(width, whiteKeyWidth));
                painter.setBrush(steps[x] == key ? HIGHLIGHT_COLOR : QColor(Qt::white));
                painter.fillRect(rect, painter.brush());
                painter.drawRect(rect);
                if (x == 0 && j == 0) {
                    QTextOption textOption;
                    textOption.setAlignment(Qt::AlignRight);
                    painter.drawText(rect.adjusted(0, 4, -2, 0), QString("C%1").arg(bottomOctave + i), textOption);
                }
            }

            topLeft -= QPoint(0, octaveHeight);
        }

        painter.setPen(QColor(Qt::black));
        painter.setBrush(QColor(Qt::black));

        topLeft = QPoint(x*width, octaveStart - 2 * ROW_HEIGHT);

        for (int i = 0; i < numOctavesAcrossHeight; i++) {
            for (int j = 0; j < BLACK_KEYS_PER_OCTAVE; j++) {
                int key = ((bottomOctave + i) * KEYS_PER_OCTAVE) + blackKeys[j];
                int offset = j > 1 ? 1 : 0;
                QPoint thisTopLeft = topLeft - QPoint(0, (offset + (j * 2)) * ROW_HEIGHT);
                QRect rect(thisTopLeft, thisTopLeft + QPoint(width/2, ROW_HEIGHT));
                painter.setBrush(steps[x] == key ? HIGHLIGHT_COLOR : QColor(Qt::black));
                painter.fillRect(rect, painter.brush());
                painter.drawRect(rect);
            }

            topLeft -= QPoint(0, octaveHeight);
        }
    }

}

void StepKeys::wheelEvent(QWheelEvent* event)
{
    if (event->pixelDelta().isNull()) {
        _top -= event->angleDelta().y() / 8 / 5;
    } else {
        _top -= event->pixelDelta().y();
    }
    _top = qMin(_top, ROW_HEIGHT * ROWS - height());
    _top = qMax(_top, 0);
    update();
}

void StepKeys::mousePressEvent(QMouseEvent* event)
{
    int absBottom = ROW_HEIGHT * ROWS;

    int octaveHeight = ROW_HEIGHT * KEYS_PER_OCTAVE;
    int whiteKeyWidth = (float)octaveHeight / 7.0f;

    int width = StepSequencerWidget::StepWidth + StepSequencerWidget::StepSpacing;

    int whiteKeys[] = { 0, 2, 4, 5, 7, 9, 11 };

    int stepClicked = event->x() / width;

    int mousePositionY = _top + event->y();
    int mousePosition = absBottom - mousePositionY;
    int mouseKey = -1;
    if ((event->x() % width) < width/2) {
        mouseKey = mousePosition / ROW_HEIGHT;
    } else {
        int octave = mousePosition / octaveHeight;
        int whiteKey = (mousePosition % octaveHeight) / whiteKeyWidth;
        mouseKey = (octave * KEYS_PER_OCTAVE) + whiteKeys[whiteKey];
    }

    emit clicked(event->button(), stepClicked, mouseKey);
}
