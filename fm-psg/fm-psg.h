#ifndef FM_PSG_H
#define FM_PSG_H

#include <stdio.h>
#include <cstdlib>

#include <QObject>

#include "vgmplayer.h"

class FM_PSG : public QObject
{
        Q_OBJECT

    public:
        virtual ~FM_PSG() {};

        virtual float position() = 0;

        virtual void play(const QByteArray& vgm, const bool loop) = 0;
        virtual void play() = 0;
        virtual void pause() = 0;
        virtual void stop() = 0;

        virtual bool isPlaying() const = 0;
};

#endif // FM_PSG_H
