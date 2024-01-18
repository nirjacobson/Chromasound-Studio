#include "editoplsettingscommand.h"
#include "mainwindow.h"

EditOPLSettingsCommand::EditOPLSettingsCommand(MainWindow* window, OPLSettings& settings, const OPLSettings& update)
    : _mainWindow(window)
    , _settings(settings)
    , _settingsBefore(settings)
    , _settingsAfter(update)
    , _type(NONE)
    , _operator(-1)
{
    setText("edit user tone");
}

void EditOPLSettingsCommand::undo()
{
    _settings = _settingsBefore;

    _mainWindow->doUpdate();
}

void EditOPLSettingsCommand::redo()
{
    _settings = _settingsAfter;

    _mainWindow->doUpdate();
}

int EditOPLSettingsCommand::id() const
{
    return 'Q';
}

bool EditOPLSettingsCommand::mergeWith(const QUndoCommand* other)
{
    const EditOPLSettingsCommand* esc;
    if ((esc = dynamic_cast<const EditOPLSettingsCommand*>(other))) {

        // Operator 1
        OPLSettings settings = _settingsAfter;
        settings.operators()[0].envelopeSettings().setAr(esc->_settingsAfter.operators()[0].envelopeSettings().ar());
        if (esc->_settingsAfter == settings) {
            if ((_type == NONE || _type == AR) && (_operator == -1 || _operator == 0)) {
                _type = AR;
                _operator = 0;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.operators()[0].envelopeSettings().setDr(esc->_settingsAfter.operators()[0].envelopeSettings().dr());
        if (esc->_settingsAfter == settings) {
            if ((_type == NONE || _type == DR) && (_operator == -1 || _operator == 0)) {
                _type = DR;
                _operator = 0;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.operators()[0].envelopeSettings().setSl(esc->_settingsAfter.operators()[0].envelopeSettings().sl());
        if (esc->_settingsAfter == settings) {
            if ((_type == NONE || _type == SL) && (_operator == -1 || _operator == 0)) {
                _type = SL;
                _operator = 0;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.operators()[0].envelopeSettings().setRr(esc->_settingsAfter.operators()[0].envelopeSettings().rr());
        if (esc->_settingsAfter == settings) {
            if ((_type == NONE || _type == RR) && (_operator == -1 || _operator == 0)) {
                _type = RR;
                _operator = 0;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.operators()[0].setMulti(esc->_settingsAfter.operators()[0].multi());
        if (esc->_settingsAfter == settings) {
            if ((_type == NONE || _type == MULTI) && (_operator == -1 || _operator == 0)) {
                _type = MULTI;
                _operator = 0;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.operators()[0].setKsl(esc->_settingsAfter.operators()[0].ksl());
        if (esc->_settingsAfter == settings) {
            if ((_type == NONE || _type == KSL) && (_operator == -1 || _operator == 0)) {
                _type = KSL;
                _operator = 0;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        // Operator 2
        settings = _settingsAfter;
        settings.operators()[1].envelopeSettings().setAr(esc->_settingsAfter.operators()[1].envelopeSettings().ar());
        if (esc->_settingsAfter == settings) {
            if ((_type == NONE || _type == AR) && (_operator == -1 || _operator == 1)) {
                _type = AR;
                _operator = 1;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.operators()[1].envelopeSettings().setDr(esc->_settingsAfter.operators()[1].envelopeSettings().dr());
        if (esc->_settingsAfter == settings) {
            if ((_type == NONE || _type == DR) && (_operator == -1 || _operator == 1)) {
                _type = DR;
                _operator = 1;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.operators()[1].envelopeSettings().setSl(esc->_settingsAfter.operators()[1].envelopeSettings().sl());
        if (esc->_settingsAfter == settings) {
            if ((_type == NONE || _type == SL) && (_operator == -1 || _operator == 1)) {
                _type = SL;
                _operator = 1;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.operators()[1].envelopeSettings().setRr(esc->_settingsAfter.operators()[1].envelopeSettings().rr());
        if (esc->_settingsAfter == settings) {
            if ((_type == NONE || _type == RR) && (_operator == -1 || _operator == 1)) {
                _type = RR;
                _operator = 1;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.operators()[1].setMulti(esc->_settingsAfter.operators()[1].multi());
        if (esc->_settingsAfter == settings) {
            if ((_type == NONE || _type == MULTI) && (_operator == -1 || _operator == 1)) {
                _type = MULTI;
                _operator = 1;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.operators()[1].setKsl(esc->_settingsAfter.operators()[1].ksl());
        if (esc->_settingsAfter == settings) {
            if ((_type == NONE || _type == KSL) && (_operator == -1 || _operator == 1)) {
                _type = KSL;
                _operator = 1;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        // Global
        settings = _settingsAfter;
        settings.setTl(esc->_settingsAfter.tl());
        if (esc->_settingsAfter == settings) {
            if (_type == NONE || _type == TL) {
                _type = TL;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }

        settings = _settingsAfter;
        settings.setFb(esc->_settingsAfter.fb());
        if (esc->_settingsAfter == settings) {
            if (_type == NONE || _type == FB) {
                _type = FB;
                _settingsAfter = esc->_settingsAfter;
                return true;
            } else {
                return false;
            }
        }
    }

    return false;
}
