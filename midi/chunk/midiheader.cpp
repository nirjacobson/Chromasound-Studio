#include "midiheader.h"

MIDIHeader::MIDIHeader(quint32 offset, const QString& chunkType, quint32 length)
    : MIDIChunk(offset, chunkType, length)
{

}

quint16 MIDIHeader::format() const
{
    return _format;
}

quint16 MIDIHeader::numTracks() const
{
    return _ntrks;
}

quint16 MIDIHeader::division() const
{
    return _division;
}

MIDIHeader& MIDIHeader::operator<<(QDataStream& stream)
{
    stream >> _format;
    stream >> _ntrks;
    stream >> _division;

    return *this;
}
