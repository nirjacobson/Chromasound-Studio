#include "composerwidget.h"

#include "mainwindow.h"
#include "ui_composerwidget.h"

ComposerWidget::ComposerWidget(QWidget *parent, Application* app) :
    QMainWindow(parent),
    ui(new Ui::ComposerWidget),
    _app(app),
    _songRenderer(new SongRenderer::SongRenderer()),
    _ackDialog(nullptr),
    _instrumentsDialog(nullptr),
    _clefMenu(nullptr),
    _keySigMenu(nullptr),
    _dynamicMenu(nullptr),
    _wedgeMenu(nullptr),
    _accMenu(nullptr),
    _noteMenu(nullptr),
    _restMenu(nullptr),
    _mode(Mode::None),
    _accActive(false),
    _accIsSharp(true),
    _dotActive(false),
    _mezzoActive(false),
    _shouldNotReleaseDynamic(false),
    _shouldNotReleaseWedge(false),
    _shouldNotReleaseClef(false),
    _shouldNotReleaseKeySig(false),
    _shouldNotReleaseAcc(false),
    _shouldNotReleaseNote(false),
    _shouldNotReleaseBeam(false),
    _shouldNotReleaseRest(false)
{
    ui->setupUi(this);

    createMenus();

    connect(_songRenderer, &SongRenderer::SongRenderer::finished, this, &ComposerWidget::songRendered);

    connect(ui->clefToolButton, &QToolButton::clicked, this, &ComposerWidget::clefToolButtonClicked);
    connect(ui->keySigToolButton, &QToolButton::clicked, this, &ComposerWidget::keySigToolButtonClicked);
    connect(ui->mezzoToolButton, &QToolButton::clicked, this, &ComposerWidget::mezzoToolButtonClicked);
    connect(ui->dynamicToolButton, &QToolButton::clicked, this, &ComposerWidget::dynamicToolButtonClicked);
    connect(ui->wedgeToolButton, &QToolButton::clicked, this, &ComposerWidget::wedgeToolButtonClicked);
    connect(ui->accToolButton, &QToolButton::clicked, this, &ComposerWidget::accToolButtonClicked);
    connect(ui->noteToolButton, &QToolButton::clicked, this, &ComposerWidget::noteToolButtonClicked);
    connect(ui->beamToolButton, &QToolButton::clicked, this, &ComposerWidget::beamToolButtonClicked);
    connect(ui->restToolButton, &QToolButton::clicked, this, &ComposerWidget::restToolButtonClicked);
    connect(ui->dotToolButton, &QToolButton::clicked, this, &ComposerWidget::dotToolButtonClicked);

    for (int i = 0, count = ui->toolPanel->layout()->count(); i < count; i++) {
        QWidget* widget = ui->toolPanel->layout()->itemAt(0)->widget();
        ui->toolPanel->layout()->removeWidget(widget);
        ui->toolBar->addWidget(widget);
    }
    ui->toolPanel->deleteLater();

    connect(ui->actionInstruments, &QAction::triggered, this, &ComposerWidget::showInstruments);
    connect(ui->actionAcknowledgements, &QAction::triggered, this, &ComposerWidget::showAcknowledgements);
}

ComposerWidget::~ComposerWidget()
{
    delete ui;
}

QIcon ComposerWidget::createTextIcon(const QString &text, int size) {
     QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.drawText(pixmap.rect(), Qt::AlignCenter, text);
    painter.end();

    return QIcon(pixmap);
}

void ComposerWidget::createMenus() {
    _crescendoIcon = createTextIcon("<");
    _diminuendoIcon = createTextIcon(">");

    ui->wedgeToolButton->setIcon(_crescendoIcon);

    _clefMenu = new QMenu(this);
    QAction* trebleAction = new QAction(tr("Treble"), this);
    trebleAction->setIcon(QIcon(":/icons/notation/clefs/treble.svg"));
    connect(trebleAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseClef = true;
        ui->clefToolButton->setIcon(QIcon(":/icons/notation/clefs/treble.svg"));
        ui->clefToolButton->click();
    });
    _clefMenu->addAction(trebleAction);
    QAction* bassAction = new QAction(tr("Bass"), this);
    bassAction->setIcon(QIcon(":/icons/notation/clefs/bass.svg"));
    connect(bassAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseClef = true;
        ui->clefToolButton->setIcon(QIcon(":/icons/notation/clefs/bass.svg"));
        ui->clefToolButton->click();
    });
    _clefMenu->addAction(bassAction);
    ui->clefToolButton->setMenu(_clefMenu);

    _keySigMenu = new QMenu(this);
    QAction* flatAction = new QAction(tr("Flat"), this);
    flatAction->setIcon(QIcon(":/icons/notation/accidentals/flat.svg"));
    connect(flatAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseKeySig = true;
        ui->keySigToolButton->setIcon(QIcon(":/icons/notation/accidentals/flat.svg"));
        ui->keySigToolButton->click();

    });
    _keySigMenu->addAction(flatAction);
    QAction* sharpAction = new QAction(tr("Sharp"), this);
    sharpAction->setIcon(QIcon(":/icons/notation/accidentals/sharp.svg"));
    connect(sharpAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseKeySig = true;
        ui->keySigToolButton->setIcon(QIcon(":/icons/notation/accidentals/sharp.svg"));
        ui->keySigToolButton->click();
    });
    _keySigMenu->addAction(sharpAction);
    ui->keySigToolButton->setMenu(_keySigMenu);

    _dynamicMenu = new QMenu(this);
    QAction* pianoAction = new QAction(tr("Piano"), this);
    pianoAction->setIcon(QIcon(":/icons/notation/dynamics/piano.svg"));
    connect(pianoAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseDynamic = true;
        ui->dynamicToolButton->setIcon(QIcon(":/icons/notation/dynamics/piano.svg"));
        ui->dynamicToolButton->click();
    });
    _dynamicMenu->addAction(pianoAction);
    QAction* forteAction = new QAction(tr("Forte"), this);
    forteAction->setIcon(QIcon(":/icons/notation/dynamics/forte.svg"));
    connect(forteAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseDynamic = true;
        ui->dynamicToolButton->setIcon(QIcon(":/icons/notation/dynamics/forte.svg"));
        ui->dynamicToolButton->click();
    });
    _dynamicMenu->addAction(forteAction);
    ui->dynamicToolButton->setMenu(_dynamicMenu);

    _wedgeMenu = new QMenu(this);
    QAction* crescendoAction = new QAction(tr("Crescendo"), this);
    crescendoAction->setIcon(_crescendoIcon);
    connect(crescendoAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseWedge = true;
        ui->wedgeToolButton->setIcon(_crescendoIcon);
        ui->wedgeToolButton->click();
    });
    _wedgeMenu->addAction(crescendoAction);
    QAction* diminuendoAction = new QAction(tr("Diminuendo"), this);
    diminuendoAction->setIcon(_diminuendoIcon);
    connect(diminuendoAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseWedge = true;
        ui->wedgeToolButton->setIcon(_diminuendoIcon);
        ui->wedgeToolButton->click();
    });
    _wedgeMenu->addAction(diminuendoAction);
    ui->wedgeToolButton->setMenu(_wedgeMenu);

    _accMenu = new QMenu(this);
    QAction* accFlatAction = new QAction(tr("Flat"), this);
    accFlatAction->setIcon(QIcon(":/icons/notation/accidentals/flat.svg"));
    connect(accFlatAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseAcc = true;
        _accIsSharp = false;
        ui->accToolButton->setIcon(QIcon(":/icons/notation/accidentals/flat.svg"));
        ui->accToolButton->click();
    });
    _accMenu->addAction(accFlatAction);
    QAction* accSharpAction = new QAction(tr("Sharp"), this);
    accSharpAction->setIcon(QIcon(":/icons/notation/accidentals/sharp.svg"));
    connect(accSharpAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseAcc = true;
        _accIsSharp = true;
        ui->accToolButton->setIcon(QIcon(":/icons/notation/accidentals/sharp.svg"));
        ui->accToolButton->click();
    });
    _accMenu->addAction(accSharpAction);
    ui->accToolButton->setMenu(_accMenu);

    _noteMenu = new QMenu(this);
    QAction* wholeNoteAction = new QAction(tr("Whole"), this);
    wholeNoteAction->setIcon(QIcon(":/icons/notation/notes/whole.svg"));
    connect(wholeNoteAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseNote = true;
        ui->noteToolButton->setIcon(QIcon(":/icons/notation/notes/whole.svg"));
        ui->noteToolButton->click();
    });
    _noteMenu->addAction(wholeNoteAction);
    QAction* halfNoteAction = new QAction(tr("Half"), this);
    halfNoteAction->setIcon(QIcon(":/icons/notation/notes/half.svg"));
    connect(halfNoteAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseNote = true;
        ui->noteToolButton->setIcon(QIcon(":/icons/notation/notes/half.svg"));
        ui->noteToolButton->click();
    });
    _noteMenu->addAction(halfNoteAction);
    QAction* quarterNoteAction = new QAction(tr("Quarter"), this);
    quarterNoteAction->setIcon(QIcon(":/icons/notation/notes/quarter.svg"));
    connect(quarterNoteAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseNote = true;
        ui->noteToolButton->setIcon(QIcon(":/icons/notation/notes/quarter.svg"));
        ui->noteToolButton->click();
    });
    _noteMenu->addAction(quarterNoteAction);
    QAction* eigthNoteAction = new QAction(tr("Eighth"), this);
    eigthNoteAction->setIcon(QIcon(":/icons/notation/notes/eighth.svg"));
    connect(eigthNoteAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseNote = true;
        ui->beamToolButton->setIcon(QIcon(":/icons/notation/notes/eighths_beamed.svg"));
        ui->noteToolButton->setIcon(QIcon(":/icons/notation/notes/eighth.svg"));
        ui->noteToolButton->click();
    });
    _noteMenu->addAction(eigthNoteAction);
    QAction* sixtnthNoteAction = new QAction(tr("Sixteenth"), this);
    sixtnthNoteAction->setIcon(QIcon(":/icons/notation/notes/sixteenth.svg"));
    connect(sixtnthNoteAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseNote = true;
        ui->beamToolButton->setIcon(QIcon(":/icons/notation/notes/sixteenths_beamed.svg"));
        ui->noteToolButton->setIcon(QIcon(":/icons/notation/notes/sixteenth.svg"));
        ui->noteToolButton->click();
    });
    _noteMenu->addAction(sixtnthNoteAction);
    ui->noteToolButton->setMenu(_noteMenu);

    _restMenu = new QMenu(this);
    QAction* wholeRestAction = new QAction(tr("Whole"), this);
    wholeRestAction->setIcon(QIcon(":/icons/notation/rests/whole.svg"));
    connect(wholeRestAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseRest = true;
        ui->restToolButton->setIcon(QIcon(":/icons/notation/rests/whole.svg"));
        ui->restToolButton->click();
    });
    _restMenu->addAction(wholeRestAction);
    QAction* halfRestAction = new QAction(tr("Half"), this);
    halfRestAction->setIcon(QIcon(":/icons/notation/rests/half.svg"));
    connect(halfRestAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseRest = true;
        ui->restToolButton->setIcon(QIcon(":/icons/notation/rests/half.svg"));
        ui->restToolButton->click();
    });
    _restMenu->addAction(halfRestAction);
    QAction* quarterRestAction = new QAction(tr("Quarter"), this);
    quarterRestAction->setIcon(QIcon(":/icons/notation/rests/quarter.svg"));
    connect(quarterRestAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseRest = true;
        ui->restToolButton->setIcon(QIcon(":/icons/notation/rests/quarter.svg"));
        ui->restToolButton->click();
    });
    _restMenu->addAction(quarterRestAction);
    QAction* eigthRestAction = new QAction(tr("Eighth"), this);
    eigthRestAction->setIcon(QIcon(":/icons/notation/rests/eighth.svg"));
    connect(eigthRestAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseRest = true;
        ui->restToolButton->setIcon(QIcon(":/icons/notation/rests/eighth.svg"));
        ui->restToolButton->click();
    });
    _restMenu->addAction(eigthRestAction);
    QAction* sixtnthRestAction = new QAction(tr("Sixteenth"), this);
    sixtnthRestAction->setIcon(QIcon(":/icons/notation/rests/sixteenth.svg"));
    connect(sixtnthRestAction, &QAction::triggered, this, [this]() {
        _shouldNotReleaseRest = true;
        ui->restToolButton->setIcon(QIcon(":/icons/notation/rests/sixteenth.svg"));
        ui->restToolButton->click();
    });
    _restMenu->addAction(sixtnthRestAction);
    ui->restToolButton->setMenu(_restMenu);
}

void ComposerWidget::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);

    QTimer::singleShot(0, this, [this]() {
        _songRenderer->render(&_app->project(), ui->composerDisplayWidget->width(), ui->composerDisplayWidget->height());
    });
}

void ComposerWidget::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    QTimer::singleShot(0, this, [this]() {
        _songRenderer->render(&_app->project(), ui->composerDisplayWidget->width(), ui->composerDisplayWidget->height());
    });
}

void ComposerWidget::showInstruments() {
    if (_instrumentsDialog == nullptr) {
        _instrumentsDialog = new ComposerInstrumentsDialog(this);
    }
    _app->window()->showWindow(_instrumentsDialog);

}

void ComposerWidget::showAcknowledgements() {
    if (_ackDialog == nullptr) {
        _ackDialog = new ComposerAckDialog(this);
    }
    _app->window()->showWindow(_ackDialog, false);
}

void ComposerWidget::clefToolButtonClicked() {
    if (ui->clefToolButton->isChecked()) {
        if (ui->keySigToolButton->isChecked()) {
            ui->keySigToolButton->click();
        }
        if (ui->noteToolButton->isChecked()) {
            ui->noteToolButton->click();
        }
        if (ui->restToolButton->isChecked()) {
            ui->restToolButton->click();
        }
        if (ui->beamToolButton->isChecked()) {
            ui->beamToolButton->click();
        }
        _mode = Mode::Clef;
    } else {
        _mode = Mode::None;
    }
    if (_shouldNotReleaseClef) {
        ui->clefToolButton->setChecked(true);
        _shouldNotReleaseClef = false;
    }
}

void ComposerWidget::keySigToolButtonClicked() {
    if (ui->keySigToolButton->isChecked()) {
        if (ui->clefToolButton->isChecked()) {
            ui->clefToolButton->click();
        }
        if (ui->noteToolButton->isChecked()) {
            ui->noteToolButton->click();
        }
        if (ui->dynamicToolButton->isChecked()) {
            ui->dynamicToolButton->click();
        }
        if (ui->wedgeToolButton->isChecked()) {
            ui->wedgeToolButton->click();
        }
        if (ui->restToolButton->isChecked()) {
            ui->restToolButton->click();
        }
        if (ui->beamToolButton->isChecked()) {
            ui->beamToolButton->click();
        }
        _mode = Mode::KeySignature;
    } else {
        _mode = Mode::None;
    }
    if (_shouldNotReleaseKeySig) {
        ui->keySigToolButton->setChecked(true);
        _shouldNotReleaseKeySig = false;
    }
}

void ComposerWidget::mezzoToolButtonClicked() {
    _mezzoActive = ui->mezzoToolButton->isChecked();
}

void ComposerWidget::dynamicToolButtonClicked() {
    if (ui->dynamicToolButton->isChecked()) {
        if (ui->clefToolButton->isChecked()) {
            ui->clefToolButton->click();
        }
        if (ui->keySigToolButton->isChecked()) {
            ui->keySigToolButton->click();
        }
        if (ui->noteToolButton->isChecked()) {
            ui->noteToolButton->click();
        }
        if (ui->wedgeToolButton->isChecked()) {
            ui->wedgeToolButton->click();
        }
        if (ui->restToolButton->isChecked()) {
            ui->restToolButton->click();
        }
        if (ui->beamToolButton->isChecked()) {
            ui->beamToolButton->click();
        }
        _mode = Mode::Dynamic;
    } else {
        _mode = Mode::None;
    }
    if (_shouldNotReleaseDynamic) {
        ui->dynamicToolButton->setChecked(true);
        _shouldNotReleaseDynamic = false;
    }
}

void ComposerWidget::wedgeToolButtonClicked() {
    if (ui->wedgeToolButton->isChecked()) {
        if (ui->clefToolButton->isChecked()) {
            ui->clefToolButton->click();
        }
        if (ui->keySigToolButton->isChecked()) {
            ui->keySigToolButton->click();
        }
        if (ui->dynamicToolButton->isChecked()) {
            ui->dynamicToolButton->click();
        }
        if (ui->noteToolButton->isChecked()) {
            ui->noteToolButton->click();
        }
        if (ui->restToolButton->isChecked()) {
            ui->restToolButton->click();
        }
        if (ui->beamToolButton->isChecked()) {
            ui->beamToolButton->click();
        }
        _mode = Mode::Wedge;
    } else {
        _mode = Mode::None;
    }
    if (_shouldNotReleaseWedge) {
        ui->wedgeToolButton->setChecked(true);
        _shouldNotReleaseWedge = false;
    }
}

void ComposerWidget::accToolButtonClicked() {
    _accActive = ui->accToolButton->isChecked();
    if (_shouldNotReleaseAcc) {
        ui->accToolButton->setChecked(true);
        _shouldNotReleaseAcc = false;
    }
}

void ComposerWidget::noteToolButtonClicked() {
    if (ui->noteToolButton->isChecked()) {
        if (ui->clefToolButton->isChecked()) {
            ui->clefToolButton->click();
        }
        if (ui->keySigToolButton->isChecked()) {
            ui->keySigToolButton->click();
        }
        if (ui->dynamicToolButton->isChecked()) {
            ui->dynamicToolButton->click();
        }
        if (ui->wedgeToolButton->isChecked()) {
            ui->wedgeToolButton->click();
        }
        if (ui->restToolButton->isChecked()) {
            ui->restToolButton->click();
        }
        if (ui->beamToolButton->isChecked()) {
            ui->beamToolButton->click();
        }
        _mode = Mode::Notes;
    } else {
        _mode = Mode::None;
    }
    if (_shouldNotReleaseNote) {
        ui->noteToolButton->setChecked(true);
        _shouldNotReleaseNote = false;
    }
}

void ComposerWidget::beamToolButtonClicked() {
    if (ui->beamToolButton->isChecked()) {
        if (ui->clefToolButton->isChecked()) {
            ui->clefToolButton->click();
        }
        if (ui->keySigToolButton->isChecked()) {
            ui->keySigToolButton->click();
        }
        if (ui->dynamicToolButton->isChecked()) {
            ui->dynamicToolButton->click();
        }
        if (ui->wedgeToolButton->isChecked()) {
            ui->wedgeToolButton->click();
        }
        if (ui->noteToolButton->isChecked()) {
            ui->noteToolButton->click();
        }
        if (ui->restToolButton->isChecked()) {
            ui->restToolButton->click();
        }
        _mode = Mode::Beam;
    } else {
        _mode = Mode::None;
    }
    if (_shouldNotReleaseBeam) {
        ui->beamToolButton->setChecked(true);
        _shouldNotReleaseBeam = false;
    }
}

void ComposerWidget::restToolButtonClicked() {
    if (ui->restToolButton->isChecked()) {
        if (ui->clefToolButton->isChecked()) {
            ui->clefToolButton->click();
        }
        if (ui->keySigToolButton->isChecked()) {
            ui->keySigToolButton->click();
        }
        if (ui->dynamicToolButton->isChecked()) {
            ui->dynamicToolButton->click();
        }
        if (ui->wedgeToolButton->isChecked()) {
            ui->wedgeToolButton->click();
        }
        if (ui->noteToolButton->isChecked()) {
            ui->noteToolButton->click();
        }
        if (ui->beamToolButton->isChecked()) {
            ui->beamToolButton->click();
        }
        _mode = Mode::Rests;
    } else {
        _mode = Mode::None;
    }
    if (_shouldNotReleaseRest) {
        ui->restToolButton->setChecked(true);
        _shouldNotReleaseRest = false;
    }
}

void ComposerWidget::dotToolButtonClicked() {
    _dotActive = ui->dotToolButton->isChecked();
}

void ComposerWidget::songRendered(QPixmap render) {
    qDebug() << "songRendered";

    QPixmap pixmap(ui->composerDisplayWidget->size());
    QPainter painter(&pixmap);
    painter.fillRect(pixmap.rect(), Qt::white);
    painter.drawPixmap(0, 0, render);
    painter.end();

    _songPixmap = pixmap;

    ui->composerDisplayWidget->show(&_songPixmap);
}

