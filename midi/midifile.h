#ifndef MIDIFILE_H
#define MIDIFILE_H

#include <QList>
#include <QFile>

#include "chunk/midichunk.h"
#include "chunk/midiheader.h"
#include "chunk/miditrack.h"

class MIDIFile
{
public:
    MIDIFile(QFile& file);
    ~MIDIFile();

    const QList<MIDIChunk*>& chunks() const;

private:
    QList<MIDIChunk*> _chunks;
};

#endif // MIDIFILE_H
