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
    pixmap.fill(Qt::transparent);

    QSvgRenderer svgRenderer(QStringLiteral(":/icons/notation/notes/sixteenth.svg"));

    QRandomGenerator random;
    random.seed((long)this * QRandomGenerator::global()->generate());
    for (int i = 0; i < 1000; i++) {

        QRect rect = QRect(QPoint(0, 0), QSize(random.generateDouble() * 200, random.generateDouble() * 200));
        QPixmap notePixmap(rect.size());
        notePixmap.fill(Qt::transparent);
        QPainter painter(&notePixmap);

        svgRenderer.render(&painter, rect);
        painter.end();

        Qt::GlobalColor color = Qt::white;
        while (color == Qt::white) {
            color = static_cast<Qt::GlobalColor>(random.generateDouble() * Qt::transparent);
        }

        QPainter painter2(&notePixmap);
        painter2.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter2.fillRect(notePixmap.rect(), color);
        painter2.end();

        QPainter painter3(&pixmap);
        if (i == 0) {
            painter3.fillRect(QRect(QPoint(0,0), QSize(width, 128)), Qt::white);
        }
        rect.translate(random.generateDouble() * width, random.generateDouble() * 128);
        painter3.drawPixmap(rect, notePixmap);
        painter3.end();
    }


    return pixmap;
}
