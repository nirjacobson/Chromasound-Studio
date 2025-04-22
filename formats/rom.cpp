#include "rom.h"

ROM::ROM(const QString& path)
    : _path(path)
{
    if (path.isEmpty()) return;

    QFile file(path);
    file.open(QIODevice::ReadOnly);

    quint16 count;
    file.read((char*)&count, sizeof(count));
    for (int i = 0; i < count; i++) {
        quint32 offset;
        file.read((char*)&offset, sizeof(offset));

        QString name;
        char c;
        do {
            file.read(&c, 1);
            if (c != 0x00) {
                name.append(c);
            }
        } while (c != 0x00);

        _offsets.append(offset);
        _names.append(name);
    }

    file.close();

    _size = QFileInfo(path).size();
}

const QList<quint32>& ROM::offsets() const
{
    return _offsets;
}

const QList<QString>& ROM::names() const
{
    return _names;
}

quint16 ROM::size(const int index) const
{
    if (index < _names.size() - 1) {
        return _offsets[index + 1] - _offsets[index];
    } else {
        return _size - _offsets[index];
    }
}

void ROM::remove(const int index)
{
    QByteArray header1 = header();

    quint32 removeSize = size(index);
    _offsets.removeAt(index);
    _names.removeAt(index);

    for (int i = index; i < _offsets.size(); i++) {
        _offsets[i] -= removeSize;
    }

    QByteArray header2 = header();

    quint32 headerSizeDiff = header1.size() - header2.size();

    for (int i = 0; i < _offsets.size(); i++) {
        _offsets[i] -= headerSizeDiff;
    }

    _size -= removeSize + headerSizeDiff;
}

void ROM::save(const QString& path)
{
    ROM oldRom(_path);
    QFile oldRomFile(_path);
    oldRomFile.open(QIODevice::ReadOnly);
    QByteArray oldRomFileData = oldRomFile.readAll();
    oldRomFile.close();

    QFile newRomFile(path);
    newRomFile.open(QIODevice::WriteOnly);

    newRomFile.write(header());

    for (int i = 0; i < _names.size(); i++) {
        int j = oldRom.names().indexOf(_names[i]);

        QByteArray sampleData = oldRomFileData.mid(oldRom.offsets()[j], oldRom.size(j));
        newRomFile.write(sampleData);
    }

    newRomFile.close();

    _path = path;
}

QByteArray ROM::header() const
{
    QByteArray result;

    quint16 count = _names.size();

    result.append((char*)&count, sizeof(count));

    for (int i = 0; i < _names.size(); i++) {
        quint32 offset = _offsets[i];

        result.append((char*)&offset, sizeof(offset));
        result.append(_names[i].toStdString().c_str());
        result.append((quint8)0x00);
    }

    return result;
}
