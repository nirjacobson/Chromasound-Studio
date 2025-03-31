#include "Chromasound_Studio.h"

const QString Chromasound_Studio::Organization = "Nir Jacobson";
const QString Chromasound_Studio::Application = "Chromasound Studio";
const QString Chromasound_Studio::SettingsFile = "settings.ini";
const QString Chromasound_Studio::DeviceKey = "device";
const QString Chromasound_Studio::IsChromasoundKey = "isChromasound";
const QString Chromasound_Studio::PCMStrategyKey = "pcmStrategy";
const QString Chromasound_Studio::DiscretePCMKey = "discretePCM";
const QString Chromasound_Studio::UsePCMSRAMKey = "usePCMSRAM";
const QString Chromasound_Studio::OutputDeviceKey = "outputDevice";
const QString Chromasound_Studio::EqualizerBassKey = "equalizer/bass";
const QString Chromasound_Studio::EqualizerTrebleKey = "equalizer/treble";
const QString Chromasound_Studio::AudioBufferSizeKey = "buffserSize/audio";
const QString Chromasound_Studio::PlaybackReadBufferSizeKey = "buffserSize/read/playback";
const QString Chromasound_Studio::InteractiveReadBufferSizeKey = "buffserSize/read/interactive";
const QString Chromasound_Studio::NumberOfChromasoundsKey = "layout/quantity";
const QString Chromasound_Studio::Chromasound1Key = "layout/chromasound1";
const QString Chromasound_Studio::Chromasound2Key = "layout/chromasound2";
const QString Chromasound_Studio::ChromasoundNova = "nova";
const QString Chromasound_Studio::ChromasoundNovaDirect = "novaDirect";
const QString Chromasound_Studio::ChromasoundPro = "pro";
const QString Chromasound_Studio::ChromasoundProDirect = "proDirect";
const QString Chromasound_Studio::Genesis = "genesis";
const QString Chromasound_Studio::MSX2 = "msx2";
const QString Chromasound_Studio::None = "none";
const QString Chromasound_Studio::Inline = "inline";
const QString Chromasound_Studio::Sequential = "sequential";
const QString Chromasound_Studio::Random = "random";
const QString Chromasound_Studio::Emulator = "emulator";
const QString Chromasound_Studio::ChromasoundDirect = "chromasoundDirect";

const Chromasound_Studio::Profile Chromasound_Studio::ChromasoundNovaPreset(PCMStrategy::SEQUENTIAL, true, true, false);
const Chromasound_Studio::Profile Chromasound_Studio::ChromasoundNovaDirectPreset(PCMStrategy::INLINE, true, true, false);
const Chromasound_Studio::Profile Chromasound_Studio::ChromasoundProPreset(PCMStrategy::RANDOM, true, true, true);
const Chromasound_Studio::Profile Chromasound_Studio::ChromasoundProDirectPreset(PCMStrategy::RANDOM, true, true, true);
const Chromasound_Studio::Profile Chromasound_Studio::GenesisPreset(PCMStrategy::SEQUENTIAL, false, false, false);
const Chromasound_Studio::Profile Chromasound_Studio::MSX2Preset(PCMStrategy::NONE, false, false, false);

Chromasound_Studio::Profile::Profile(const PCMStrategy pcmStrategy, const bool isChromasound, const bool discretePCM, const bool usePCMSRAM)
    : _isChromasound(isChromasound)
    , _pcmStrategy(pcmStrategy)
    , _discretePCM(discretePCM)
    , _usePCMSRAM(usePCMSRAM)
{

}

bool Chromasound_Studio::Profile::isChromasound() const
{
    return _isChromasound;
}

Chromasound_Studio::PCMStrategy Chromasound_Studio::Profile::pcmStrategy() const
{
    return _pcmStrategy;
}

bool Chromasound_Studio::Profile::discretePCM() const
{
    return _discretePCM;
}

bool Chromasound_Studio::Profile::usePCMSRAM() const
{
    return _usePCMSRAM;
}

Chromasound_Studio::PCMStrategy Chromasound_Studio::pcmStrategyFromString(const QString& str)
{
    if (str == None) {
        return PCMStrategy::NONE;
    }

    if (str == Inline) {
        return PCMStrategy::INLINE;
    }

    if (str == Sequential) {
        return PCMStrategy::SEQUENTIAL;
    }

    if (str == Random) {
        return PCMStrategy::RANDOM;
    }

    return PCMStrategy::RANDOM;
}

QString Chromasound_Studio::pcmStrategyToString(const PCMStrategy stra)
{
    if (stra == PCMStrategy::NONE) {
        return None;
    }

    if (stra == PCMStrategy::INLINE) {
        return Inline;
    }

    if (stra == PCMStrategy::SEQUENTIAL) {
        return Sequential;
    }

    if (stra == PCMStrategy::RANDOM) {
        return Random;
    }

    return Random;
}
