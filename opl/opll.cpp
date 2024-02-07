#include "opll.h"

QStringList OPLL::OPLL_PATCHES = {{
        "User tone",
        "Violin",
        "Guitar",
        "Piano",
        "Flute",
        "Clarinet",
        "Oboe",
        "Trumpet",
        "Organ",
        "Horn",
        "Synthesizer",
        "Harpsichord",
        "Vibraphone",
        "Synth Bass",
        "Wood Bass",
        "Electric Guitar"
    }
};

QStringList OPLL::OPLL_X_PATCHES = {{
        "User tone",
        "Strings",
        "Guitar",
        "Electric Guitar",
        "Electric Piano",
        "Flute",
        "Marimba",
        "Trumpet",
        "Harmonica",
        "Tuba",
        "Synth Brass",
        "Short Saw",
        "Vibraphone",
        "Electric Guitar 2",
        "Synth Bass",
        "Sitar"
    }
};

QStringList OPLL::OPLL_B_PATCHES = {{
        "User tone",
        "Electric String",
        "Bow Wow",
        "Piano",
        "Organ",
        "Clarinet",
        "Saxophone",
        "Trumpet",
        "Street Organ",
        "Synth Brass",
        "Electric Piano",
        "Bass",
        "Vibraphone",
        "Chime",
        "Tom Tom",
        "Noise"
    }
};

QString OPLL::typeToString(Type type)
{
    switch (type) {
        default:
        case STANDARD:
            return "STANDARD";
        case OPLL_X:
            return "OPLL-X";
        case OPLL_B:
            return "OPLL-B";
    }
}

OPLL::Type OPLL::typeFromString(const QString& str)
{
    if (str == "STANDARD") {
        return Type::STANDARD;
    }

    if (str == "OPLL-X") {
        return Type::OPLL_X;
    }

    if (str == "OPLL-B") {
        return Type::OPLL_B;
    }

    return Type::STANDARD;
}

QString OPLL::patchToString(const Type type, const int patch)
{
    switch (type) {
        default:
        case STANDARD:
            return OPLL_PATCHES[patch];
        case OPLL_X:
            return OPLL_X_PATCHES[patch];
        case OPLL_B:
            return OPLL_X_PATCHES[patch];
            break;
    }
}

int OPLL::patchFromString(const Type type, const QString& name)
{
    switch (type) {
        default:
        case STANDARD:
            return OPLL_PATCHES.indexOf(name);
        case OPLL_X:
            return OPLL_X_PATCHES.indexOf(name);
        case OPLL_B:
            return OPLL_B_PATCHES.indexOf(name);
    }
}

const QStringList& OPLL::patches(const Type type)
{
    switch (type) {
        default:
        case STANDARD:
            return OPLL_PATCHES;
        case OPLL_X:
            return OPLL_X_PATCHES;
        case OPLL_B:
            return OPLL_B_PATCHES;
    }
}
