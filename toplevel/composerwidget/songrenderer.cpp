#include "songrenderer.h"

#include <QPainter>

#include "commands/composite/addfmchannelcommand.h"

SongRenderer::SongRenderer::SongRenderer()
    : _renderWorker(new RenderWorker())
{
    connect(_renderWorker, &RenderWorker::finished, this, &SongRenderer::finished);
}

void SongRenderer::SongRenderer::render(const Project *song, const int width, int height, const float scale, const float startPos,
                             const float vertScroll) {
    _renderWorker->doRender(song, width, height, scale, startPos, vertScroll);
}

SongRenderer::RenderWorker::RenderWorker(QObject *parent)
    : _tracksDone(0)
{

}

void SongRenderer::RenderWorker::doRender(const Project *song, const int width, int height, const float scale,
                                          const float startPos, const float vertScroll) {
    _song = song;
    _width = width;
    _height = height;
    _scale = scale;
    _startPos = startPos;
    _vertScroll = vertScroll;

    start();
}

void SongRenderer::RenderWorker::renderTracks(const Project *song, const int width, const float scale,
    const float startPos) {
    int origSize = _trackRendererWorkers.size();
    int newSize = song->getPattern(0).tracks().values().size();

    for (int i = origSize-1; i >= newSize; i--) {
        delete _trackRendererWorkers.last();
        _trackRendererWorkers.removeLast();
    }

    _trackRenders.resize(newSize);

    for (int i = origSize; i < newSize; i++) {
        _trackRendererWorkers.push_back(new TrackRenderer::RenderWorker());
        connect(_trackRendererWorkers.at(i), &TrackRenderer::RenderWorker::finished, this, &RenderWorker::onTrackFinished);
    }

    _tracksDone = 0;
    for (int i = 0; i < _trackRendererWorkers.size(); i++) {
        _trackRendererWorkers.at(i)->doRender(song->getPattern(0).tracks().values().at(i), width, false, scale, startPos);
    }
}

void SongRenderer::RenderWorker::render(const Project* song, const int width, const float scale, const float startPos, const float vertScroll) {
    int height = 0;
    for (int i = 0; i < _trackRenders.size(); i++) {
        height += _trackRenders.at(i).height();
    }

    QPixmap result(width, height);
    QPainter painter(&result);

    height = _trackRenders[0].height();
    for (int i = 0; i < _trackRenders.size(); i++) {
        if (i == 0) {
            painter.drawPixmap(0, 0, _trackRenders.at(i));
        } else {
            painter.drawPixmap(0, height, _trackRenders.at(i));
            height += _trackRenders.at(i).height();
        }
    }

    painter.end();

    emit finished(result);
}

void SongRenderer::RenderWorker::onTrackFinished(TrackRenderer::RenderWorker *worker, QPixmap result) {
    for (int i = 0; i < _trackRendererWorkers.size(); i++) {
        if (_trackRendererWorkers.at(i) == worker) {
            _trackRenders[i] = result;
            break;
        }
    }
    _tracksDone++;

    if (_tracksDone == _trackRendererWorkers.size()) {
        emit tracksDone();
    }
}

void SongRenderer::RenderWorker::onRenderFinished(QPixmap result) {
    emit finished(result);
}

