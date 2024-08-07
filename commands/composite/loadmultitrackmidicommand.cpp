#include "loadmultitrackmidicommand.h"
#include "mainwindow.h"

LoadMultiTrackMIDICommand::LoadMultiTrackMIDICommand(MainWindow* window, const QString& path)
    : _mainWindow(window)
    , _midiFilePath(path)
{}

LoadMultiTrackMIDICommand::~LoadMultiTrackMIDICommand()
{
    for (AddTrackCommand* atc : _addTrackCommands) {
        delete atc;
    }
}

void LoadMultiTrackMIDICommand::undo()
{
    for (auto it = _addTrackCommands.rbegin(); it != _addTrackCommands.rend(); ++it) {
        (*it)->undo();
    }
}

void LoadMultiTrackMIDICommand::redo()
{
    for (AddTrackCommand* atc : _addTrackCommands) {
        delete atc;
    }
    _addTrackCommands.clear();

    QFile file(_midiFilePath);
    MIDIFile midiFile(file);

    auto it1 = std::find_if(midiFile.chunks().begin(), midiFile.chunks().end(), [](const MIDIChunk* chunk) {
        return dynamic_cast<const MIDIHeader*>(chunk);
    });

    if (it1 != midiFile.chunks().end()) {
        const MIDIHeader* header = dynamic_cast<const MIDIHeader*>(*it1);

        int j = 1;
        if (header->format() == 1) {
            for (auto it2 = midiFile.chunks().begin(); it2 != midiFile.chunks().end(); ++it2) {
                const MIDITrack* track;
                if ((track = dynamic_cast<const MIDITrack*>(*it2))) {
                    QList<Track::Item*> items = MIDI::toTrackItems(*track, header->division());

                    if (!items.empty()) {
                        QString trackName = track->name();
                        AddTrackCommand* atc = new AddTrackCommand(_mainWindow, items, trackName.isEmpty() ? QString("Track #%1").arg(j++) : trackName);
                        _addTrackCommands.append(atc);
                        atc->redo();
                    }
                }
            }
        } else {
            const MIDITrack* track;
            if ((track = dynamic_cast<const MIDITrack*>(midiFile.chunks().at(1)))) {
                QList<QList<Track::Item*>> tracks = MIDI::toTracks(*track, header->division());

                for (int i = 0; i < tracks.size(); i++) {
                    const QList<Track::Item*>& items = tracks[i];

                    if (!items.empty()) {
                        QString trackName = track->name(i);
                        AddTrackCommand* atc = new AddTrackCommand(_mainWindow, items, trackName.isEmpty() ? QString("Track #%1").arg(j++) : trackName);
                        _addTrackCommands.append(atc);
                        atc->redo();
                    }
                }
            }
        }

        _mainWindow->app()->project().setTempo(midiFile.tempo());
    }
}
