#include "midichunk.h"

MIDIChunk::MIDIChunk(const QString& chunkType, quint32 length)
    : _length(length)
{

}

MIDIChunk::~MIDIChunk()
{

}


const QString& MIDIChunk::chunkType() const
{
    return _chunkType;
}

quint32 MIDIChunk::length() const
{
    return _length;
}
