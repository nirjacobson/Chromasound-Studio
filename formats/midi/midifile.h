#ifndef MIDIFILE_H
#define MIDIFILE_H

#include <QList>
#include <QFile>

#include "chunk/midichunk.h"
#include "chunk/midiheader.h"
#include "chunk/miditrack.h"

class MIDIFile
{
        friend class MIDI;

    public:
        MIDIFile();
        MIDIFile(QFile& file);
        ~MIDIFile();

        const QList<MIDIChunk*>& chunks() const;

        QByteArray encode() const;

        int tempo() const;

    private:
        QList<MIDIChunk*> _chunks;

        void readFile(QIODevice& device);
};

#endif // MIDIFILE_H
