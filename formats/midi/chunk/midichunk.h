#ifndef MIDICHUNK_H
#define MIDICHUNK_H

#include <QDataStream>
#include <QString>

#include "qtypes.h"

class MIDIChunk
{
    public:
        MIDIChunk(const QString& chunkType, quint32 length);
        virtual ~MIDIChunk();

        const QString& chunkType() const;
        quint32 length() const;

        virtual QByteArray encode() const = 0;

    private:
        QString _chunkType;
        quint32 _length;
};

#endif // MIDICHUNK_H
