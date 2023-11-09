#include "midiheader.h"

MIDIHeader::MIDIHeader(const QString& chunkType, quint32 length)
    : MIDIChunk(chunkType, length)
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

QByteArray MIDIHeader::encode() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << 'M';
    stream << 'T';
    stream << 'h';
    stream << 'd';
    stream << (quint32)6;
    stream << _format;
    stream << _ntrks;
    stream << _division;

    return data;
}
