#include "gd3.h"
QByteArray GD3::generateGd3(const Project& project)
{
    QByteArray gd3;

    // Header
    gd3.append('G');
    gd3.append('d');
    gd3.append('3');
    gd3.append(' ');

    // Version
    gd3.append((quint8)0x00);
    gd3.append((quint8)0x01);
    gd3.append((quint8)0x00);
    gd3.append((quint8)0x00);

    // Data
    QByteArray data;

    // Title
    data.append(QByteArray((const char*)project.info().title().utf16(), 2*(project.info().title().size() + 1)));

    data.append((quint8)0x00);
    data.append((quint8)0x00);

    // Game
    data.append(QByteArray((const char*)project.info().game().utf16(), 2*(project.info().game().size() + 1)));

    data.append((quint8)0x00);
    data.append((quint8)0x00);

    // System
    QString system = "Sega Mega Drive / Genesis";
    data.append(QByteArray((const char*)system.utf16(), 2*(system.size() + 1)));

    data.append((quint8)0x00);
    data.append((quint8)0x00);

    // Author
    data.append(QByteArray((const char*)project.info().author().utf16(), 2*(project.info().author().size() + 1)));

    data.append((quint8)0x00);
    data.append((quint8)0x00);

    // Release date
    QString releaseDate = project.info().releaseDate().toString("yyyy/MM/dd");
    data.append(QByteArray((const char*)releaseDate.utf16(), 2*(releaseDate.size() + 1)));

    // Arranger
    data.append((quint8)0x00);
    data.append((quint8)0x00);

    // Notes
    data.append(QByteArray((const char*)project.info().notes().utf16(), 2*(project.info().notes().size() + 1)));

    // Data size
    quint32 dataSize = data.size();
    gd3.append((char*)&dataSize, sizeof(dataSize));

    // Data
    gd3.append(data);

    return gd3;
}

GD3 GD3::parseGd3(const QString& path)
{
    QFile vgmFile(path);
    vgmFile.open(QIODevice::ReadOnly);

    QDataStream vgm(&vgmFile);
    vgm.setByteOrder(QDataStream::LittleEndian);

    vgm.skipRawData(0x14);
    quint32 gd3Offset;
    vgm >> gd3Offset;

    vgm.skipRawData(gd3Offset + 8);

    QByteArray title;
    QByteArray game;
    QByteArray author;
    QByteArray release;

    // Title
    quint16 ch = 0xFF;
    while (ch != 0x00) {
        vgm >> ch;
        title.append((const char*)&ch, sizeof(ch));
    }

    ch = 0xFF;
    while (ch != 0x00) {
        vgm >> ch;
    }

    // Game
    ch = 0xFF;
    while (ch != 0x00) {
        vgm >> ch;
        game.append((const char*)&ch, sizeof(ch));
    }

    ch = 0xFF;
    while (ch != 0x00) {
        vgm >> ch;
    }

    // System
    ch = 0xFF;
    while (ch != 0x00) {
        vgm >> ch;
    }

    ch = 0xFF;
    while (ch != 0x00) {
        vgm >> ch;
    }

    // Author
    ch = 0xFF;
    while (ch != 0x00) {
        vgm >> ch;
        author.append((const char*)&ch, sizeof(ch));
    }

    ch = 0xFF;
    while (ch != 0x00) {
        vgm >> ch;
    }

    // Release
    ch = 0xFF;
    while (ch != 0x00) {
        vgm >> ch;
        release.append((const char*)&ch, sizeof(ch));
    }

    GD3 gd3;

    gd3._title = QString::fromUtf16(reinterpret_cast<const char16_t*>(title.constData()));
    gd3._game = QString::fromUtf16(reinterpret_cast<const char16_t*>(game.constData()));
    gd3._author = QString::fromUtf16(reinterpret_cast<const char16_t*>(author.constData()));
    gd3._releaseDate = QDate::fromString(QString::fromUtf16(reinterpret_cast<const char16_t*>(release.constData())), "yyyy/MM/dd");

    vgmFile.close();

    return gd3;
}

const QString& GD3::title() const
{
    return _title;
}

const QString& GD3::game() const
{
    return _game;
}

const QString& GD3::author() const
{
    return _author;
}

const QDate& GD3::releaseDate() const
{
    return _releaseDate;
}
