#ifndef TRACKRENDERER_H
#define TRACKRENDERER_H

#include <QPainter>
#include <QPixmap>
#include <QRandomGenerator>
#include <QThread>

#include "project/track.h"

namespace TrackRenderer {
    class Worker : public QThread
    {
        Q_OBJECT
    public:
        Worker(QObject* parent = nullptr)
            : QThread(parent)
        { }
    };

    class RenderWorker : public Worker {
        Q_OBJECT

    public:
        void doRender(const Track* track, const int width, const bool wrap, const float scale = 1.0, const float startPos = 0);

        typedef QPixmap Result;

    signals:
        void finished(RenderWorker* worker, Result result);

    protected:
        void run() override {
            QPixmap pixmap = render(_track, _width, _wrap, _scale, _startPos);

            emit finished(this, pixmap);
        }

    private:
        const Track* _track;
        int _width;
        bool _wrap;
        float _scale;
        float _startPos;

        QPixmap render(const Track* track, const int width, const bool wrap, const float scale = 1.0, const float startPos = 0);
    };
}

#endif //TRACKRENDERER_H
