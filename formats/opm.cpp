#include "opm.h"

OPM::OPM() {}

std::map<std::string, FMChannelSettings> OPM::parseOPM(const QString &path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);

    std::map<std::string, FMChannelSettings> settingsMap;
    while (!file.atEnd()) {
        QString line = file.readLine().trimmed();

        if (line.isEmpty() || line.startsWith("//")) {
            continue;
        }

        FMChannelSettings settings;

        QString name = line.mid(line.indexOf(QRegularExpression("\\s+")));

        file.readLine();

        line = file.readLine().trimmed();

        QStringList lineParts = line.split(QRegularExpression("\\s+"));

        settings.algorithm().setFeedback(lineParts[2].toInt());
        settings.algorithm().setAlgorithm(lineParts[3].toInt());
        settings.lfo().setAMS(lineParts[4].toInt());
        settings.lfo().setFMS(lineParts[5].toInt());

        for (int i = 0; i < 4; i++) {
            line = file.readLine().trimmed();

            lineParts = line.split(QRegularExpression("\\s+"));

            settings.operators()[i].envelopeSettings().setAr(lineParts[1].toInt());
            settings.operators()[i].envelopeSettings().setD1r(lineParts[2].toInt());
            settings.operators()[i].envelopeSettings().setD2r(lineParts[3].toInt());
            settings.operators()[i].envelopeSettings().setRr(lineParts[4].toInt());
            settings.operators()[i].envelopeSettings().setT2l(lineParts[5].toInt());
            settings.operators()[i].envelopeSettings().setT1l(lineParts[6].toInt());
            settings.operators()[i].setMul(lineParts[8].toInt());
            settings.operators()[i].setAm(lineParts[11].toInt() != 0);
        }

        settingsMap[name.trimmed().toStdString()] = settings;
    }

    file.close();

    return settingsMap;
}
