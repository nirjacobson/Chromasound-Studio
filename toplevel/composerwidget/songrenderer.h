#ifndef SONGRENDERER_H
#define SONGRENDERER_H

#include <QPixmap>

#include "trackrenderer.h"
#include "formats/bson.h"
#include "project/project.h"

namespace SongRenderer {

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
        RenderWorker(QObject* parent = nullptr);
        void doRender(const Project* song, const int width, int height, const float scale = 1.0, const float startPos = 0, const float vertScroll = 0);

        typedef QPixmap Result;

        signals:
            void finished(Result result);

    protected:
        void run() override {
            renderTracks(_song, _width, _scale, _startPos);

            QEventLoop loop;
            connect(this, &RenderWorker::tracksDone, &loop, &QEventLoop::quit);
            loop.exec();

            render(_song, _width, _scale, _startPos, _vertScroll);
        }

    private:
        QList<TrackRenderer::RenderWorker*> _trackRendererWorkers;
        QList<QPixmap> _trackRenders;

        const Project* _song;
        int _width;
        int _height;
        float _scale;
        float _startPos;
        float _vertScroll;

        int _tracksDone;

        void renderTracks(const Project* song, const int width, const float scale = 1.0, const float startPos = 0);
        void render(const Project* song, const int width, const float scale = 1.0, const float startPos = 0, const float vertScroll = 0);

        signals:
            void tracksDone();

    private slots:
        void onTrackFinished(TrackRenderer::RenderWorker* worker, QPixmap result);
        void onRenderFinished(QPixmap result);
    };

    class SongRenderer : public QObject {
        Q_OBJECT

    public:
        SongRenderer();

        void render(const Project* song, const int width, int height, const float scale = 1.0, const float startPos = 0, const float vertScroll = 0);

        signals:
        void finished(QPixmap result);

    private:
        RenderWorker* _renderWorker;
    };
};


#endif //SONGRENDERER_H
