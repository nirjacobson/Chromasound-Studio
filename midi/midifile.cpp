#include "midifile.h"
#include <QtDebug>
MIDIFile::MIDIFile(QFile& file)
{
    readFile(file);
}

MIDIFile::MIDIFile(QBuffer& data)
{
    readFile(data);
}

MIDIFile::~MIDIFile()
{
    for (const MIDIChunk* chunk : _chunks) {
        delete chunk;
    }
}

const QList<MIDIChunk*>& MIDIFile::chunks() const
{
    return _chunks;
}

void MIDIFile::readFile(QIODevice& device)
{
    device.open(QIODevice::ReadOnly);

    QDataStream stream(&device);
    stream.setByteOrder(QDataStream::BigEndian);

    quint32 offset;
    MIDIChunk* chunk = nullptr;
    while (!stream.atEnd()) {
        offset = device.pos();

        char chunkType[5];
        quint32 length;

        stream >> chunkType[0];
        stream >> chunkType[1];
        stream >> chunkType[2];
        stream >> chunkType[3];
        chunkType[4] = 0;

        stream >> length;

        if (QString(chunkType) == "MThd") {
            MIDIHeader* header = new MIDIHeader(offset, chunkType, length);
            *header << stream;
            chunk = header;
        } else if (QString(chunkType) == "MTrk") {
            MIDITrack* track = new MIDITrack(offset, chunkType, length, [&](){ return device.pos(); });
            *track << stream;
            chunk = track;
        }

        _chunks.append(chunk);
    }

    device.close();
}
