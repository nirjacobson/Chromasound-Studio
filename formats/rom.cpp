#include "rom.h"

ROM::ROM(const QString& path) {
    if (path.isEmpty()) return;

    QFile file(path);
    file.open(QIODevice::ReadOnly);

    quint16 count;
    file.read((char*)&count, sizeof(count));
    for (int i = 0; i < count; i++) {
        quint32 offset;
        file.read((char*)&offset, sizeof(offset));

        QString name;
        char c = 0xFF;
        while (c != 0x00) {
            file.read(&c, 1);
            if (c != 0x00) {
                name.append(c);
            }
        }

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

quint32 ROM::size(const int index) const
{
    if (index < _names.size() - 1) {
        return _offsets[index + 1] - _offsets[index];
    } else {
        return _size - _offsets[index];
    }
}
