#ifndef MIDICHUNK_H
#define MIDICHUNK_H

#include <QDataStream>
#include <QString>

#include "qtypes.h"

class MIDIChunk
{
public:
    MIDIChunk(quint32 offset, const QString& chunkType, quint32 length);
    virtual ~MIDIChunk();

    unsigned long offset() const;
    const QString& chunkType() const;
    quint32 length() const;

private:
    unsigned long _offset;
    QString _chunkType;
    quint32 _length;
};

#endif // MIDICHUNK_H
