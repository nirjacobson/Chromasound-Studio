#ifndef CHROMASOUND_STUDIO_H
#define CHROMASOUND_STUDIO_H

#include <QString>

class Chromasound_Studio {
    public:
        enum PCMStrategy {
            NONE,
            INLINE,
            SEQUENTIAL,
            RANDOM
        };

        class Profile {
            public:
                Profile(const PCMStrategy pcmStrategy, const bool isChromasound, const bool discretePCM, const bool usePCMSRAM);

                bool isChromasound() const;
                PCMStrategy pcmStrategy() const;
                bool discretePCM() const;
                bool usePCMSRAM() const;

            private:
                bool _isChromasound;
                PCMStrategy _pcmStrategy;
                bool _discretePCM;
                bool _usePCMSRAM;

        };

        static PCMStrategy pcmStrategyFromString(const QString& str);
        static QString pcmStrategyToString(const PCMStrategy stra);

        static const QString Organization;
        static const QString Application;

        static const QString SettingsFile;

        static const QString DeviceKey;
        static const QString IsChromasoundKey;
        static const QString PCMStrategyKey;
        static const QString DiscretePCMKey;
        static const QString UsePCMSRAMKey;
        static const QString OutputDeviceKey;
        static const QString EqualizerBassKey;
        static const QString EqualizerTrebleKey;
        static const QString AudioBufferSizeKey;
        static const QString PlaybackReadBufferSizeKey;
        static const QString InteractiveReadBufferSizeKey;
        static const QString NumberOfChromasoundsKey;
        static const QString Chromasound1Key;
        static const QString Chromasound2Key;

        static const QString ChromasoundNova;
        static const QString ChromasoundNovaDirect;
        static const QString ChromasoundPro;
        static const QString ChromasoundProDirect;
        static const QString Genesis;
        static const QString MSX2;

        static const QString None;
        static const QString Inline;
        static const QString Sequential;
        static const QString Random;

        static const QString Emulator;
        static const QString ChromasoundDirect;

        static const Profile ChromasoundNovaPreset;
        static const Profile ChromasoundNovaDirectPreset;
        static const Profile ChromasoundProPreset;
        static const Profile ChromasoundProDirectPreset;
        static const Profile GenesisPreset;
        static const Profile MSX2Preset;
};

#endif // CHROMASOUND_STUDIO_H
