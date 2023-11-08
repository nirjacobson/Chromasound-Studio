#include "midichunk.h"

MIDIChunk::MIDIChunk(quint32 offset, const QString& chunkType, quint32 length)
    : _offset(offset)
    , _length(length)
    , _chunkType(chunkType)
{

}

MIDIChunk::~MIDIChunk()
{

}

unsigned long MIDIChunk::offset() const
{
    return _offset;
}

const QString& MIDIChunk::chunkType() const
{
    return _chunkType;
}

quint32 MIDIChunk::length() const
{
    return _length;
}
