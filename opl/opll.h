#ifndef OPLL_H
#define OPLL_H

#include <QStringList>

class OPLL
{
    public:
        enum Type {
            STANDARD,
            OPLL_X,
            OPLL_B
        };

        static QStringList OPLL_PATCHES;
        static QStringList OPLL_X_PATCHES;
        static QStringList OPLL_B_PATCHES;

        QString typeToString(Type type);
        Type typeFromString(const QString& str);

        static QString patchToString(const Type type, const int patch);
        static int patchFromString(const Type type, const QString& name);

        static const QStringList& patches(const Type type);
};

#endif // OPLL_H
