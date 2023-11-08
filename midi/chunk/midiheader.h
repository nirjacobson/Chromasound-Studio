#ifndef MIDIHEADER_H
#define MIDIHEADER_H

#include <QDataStream>

#include "midichunk.h"

class MIDIHeader : public MIDIChunk
{
public:
    MIDIHeader(quint32 offset, const QString& chunkType, quint32 length);

    quint16 format() const;
    quint16 numTracks() const;
    quint16 division() const;

    MIDIHeader& operator<<(QDataStream& stream);

private:
    quint16 _format;
    quint16 _ntrks;
    quint16 _division;
};

#endif // MIDIHEADER_H
