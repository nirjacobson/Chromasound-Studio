#include "recordiconengine.h"

RecordIconEngine::RecordIconEngine() {

}

void RecordIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    if (mode == QIcon::Mode::Disabled) {
        painter->setBrush(QColor(128, 128, 128));
    } else {
        painter->setBrush(QColor(0, 0, 0));
    }
    painter->fillRect(rect, QPushButton().palette().color(QPalette::Button));
    painter->drawEllipse(rect.center(), rect.width() / 3, rect.height() / 3);
}

QIconEngine *RecordIconEngine::clone() const
{
    return new RecordIconEngine(*this);
}
