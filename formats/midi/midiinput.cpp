#include "midiinput.h"

MIDIInput* MIDIInput::_instance = nullptr;

PmError MIDIInput::_pmError = pmNoError;
PmDeviceID MIDIInput::_pmDeviceId = 0;
PortMidiStream* MIDIInput::_pmStream = nullptr;

MIDIInput::MIDIInput()
    : _deviceIndex(-1)
{

}

MIDIInput* MIDIInput::instance()
{
    if (_instance == nullptr) {
        _instance = new MIDIInput;
    }

    return _instance;
}

void MIDIInput::init()
{
    _pmError = Pm_Initialize();
    if (_pmError != pmNoError) {
        fprintf(stderr, "[init] PortMIDI error: %s\n", Pm_GetErrorText(_pmError));
    }

    if (devices().size() > 0) {
        setDevice(0);
    }
}

void MIDIInput::destroy()
{
    _pmError = Pm_Close(_pmStream);
    if (_pmError != pmNoError) {
        fprintf(stderr, "[close] PortMIDI error: %s\n", Pm_GetErrorText(_pmError));
    }

    _pmError = Pm_Terminate();
    if (_pmError != pmNoError) {
        fprintf(stderr, "[terminate] PortMIDI error: %s\n", Pm_GetErrorText(_pmError));
    }

    delete _instance;
}

QList<QString> MIDIInput::devices() const
{
    QList<QString> inputDevices;

    int totalDevices = Pm_CountDevices();

    for (int i = 0; i < totalDevices; i++) {
        const PmDeviceInfo* info = Pm_GetDeviceInfo(i);
        if (info->input) {
            inputDevices.append(info->name);
        }
    }

    return inputDevices;
}

void MIDIInput::setDevice(const int index)
{
    int totalDevices = Pm_CountDevices();
    int inputDevices = 0;

    for (int i = 0; i < totalDevices; i++) {
        const PmDeviceInfo* info = Pm_GetDeviceInfo(i);

        if (info->input && inputDevices == index) {
            const PmDeviceInfo* currentInfo = Pm_GetDeviceInfo(_pmDeviceId);

            if (currentInfo->opened) {
                _pmError = Pm_Close(_pmStream);
                if (_pmError != pmNoError) {
                    fprintf(stderr, "[setDevice] PortMIDI error: %s\n", Pm_GetErrorText(_pmError));
                }
            }

            _pmDeviceId = i;
            _pmError = Pm_OpenInput(&_pmStream,
                                    _pmDeviceId,
                                    nullptr,
                                    128,
                                    nullptr,
                                    nullptr);
            if (_pmError != pmNoError) {
                fprintf(stderr, "[setDevice] PortMIDI error: %s\n", Pm_GetErrorText(_pmError));
            }

            _deviceIndex = index;

            return;
        }

        inputDevices += info->input;
    }
}

int MIDIInput::device() const
{
    return _deviceIndex;
}

QList<long> MIDIInput::read() const
{
    QList<long> messages;

    PmEvent buffer[128];
    int read = Pm_Read(_pmStream, buffer, 128);

    for (int i = 0; i < read; i++) {
        messages.append(buffer[i].message);
    }

    return messages;
}
