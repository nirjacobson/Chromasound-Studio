#include "trackrenderer.h"

void TrackRenderer::RenderWorker::doRender(const Track *track, const int width, const bool wrap,
                                           const float scale, const float startPos) {
    _track = track;
    _width = width;
    _wrap = wrap;
    _scale = scale;
    _startPos = startPos;

    start();
}

QPixmap TrackRenderer::RenderWorker::render(const Track *track, const int width, const bool wrap, const float scale,
    const float startPos) {
    QPixmap pixmap(width, 128);
    QPainter painter(&pixmap);

    Qt::GlobalColor color = static_cast<Qt::GlobalColor>(QRandomGenerator::global()->generateDouble() * Qt::transparent);

    painter.fillRect(pixmap.rect(), color);
    painter.end();

    return pixmap;
}
