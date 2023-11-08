#ifndef MIDIFILE_H
#define MIDIFILE_H

#include <QList>
#include <QFile>
#include <QBuffer>

#include "chunk/midichunk.h"
#include "chunk/midiheader.h"
#include "chunk/miditrack.h"

class MIDIFile
{
public:
    MIDIFile(QFile& file);
    MIDIFile(QBuffer& data);
    ~MIDIFile();

    const QList<MIDIChunk*>& chunks() const;

private:
    QList<MIDIChunk*> _chunks;

    void readFile(QIODevice& device);
};

#endif // MIDIFILE_H
