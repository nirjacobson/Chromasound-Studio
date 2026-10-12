#ifndef COMPOSERWIDGET_H
#define COMPOSERWIDGET_H

#include <QPointer>
#include <QMainWindow>
#include "application.h"
#include "songrenderer.h"
#include "trackrenderer.h"
#include "toplevel/composerwidget/composerinstrumentsdialog.h"
#include "toplevel/composerwidget/composerackdialog.h"
#include "toplevel/composerwidget/songrenderer.h"

namespace Ui {
class ComposerWidget;
}

class ComposerWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit ComposerWidget(QWidget *parent = nullptr, Application* app = nullptr);
    ~ComposerWidget();

private:
    Ui::ComposerWidget *ui;
    Application* _app;

    SongRenderer::SongRenderer* _songRenderer;
    QPixmap _songPixmap;

    enum Mode {
        None,
        Clef,
        KeySignature,
        Dynamic,
        Wedge,
        Notes,
        Beam,
        Rests
    };

    ComposerInstrumentsDialog* _instrumentsDialog;
    ComposerAckDialog* _ackDialog;

    QMenu* _clefMenu;
    QMenu* _keySigMenu;
    QMenu* _dynamicMenu;
    QMenu* _wedgeMenu;
    QMenu* _accMenu;
    QMenu* _noteMenu;
    QMenu* _restMenu;

    QIcon _crescendoIcon;
    QIcon _diminuendoIcon;

    Mode _mode;
    bool _accActive;
    bool _accIsSharp;

    bool _dotActive;

    bool _mezzoActive;

    bool _shouldNotReleaseClef;
    bool _shouldNotReleaseKeySig;
    bool _shouldNotReleaseDynamic;
    bool _shouldNotReleaseWedge;
    bool _shouldNotReleaseAcc;
    bool _shouldNotReleaseNote;
    bool _shouldNotReleaseBeam;
    bool _shouldNotReleaseRest;

    QIcon createTextIcon(const QString& text, int size=16);
    void createMenus();
protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void showInstruments();
    void showAcknowledgements();

    void clefToolButtonClicked();
    void keySigToolButtonClicked();
    void mezzoToolButtonClicked();
    void dynamicToolButtonClicked();
    void wedgeToolButtonClicked();
    void accToolButtonClicked();
    void noteToolButtonClicked();
    void beamToolButtonClicked();
    void restToolButtonClicked();
    void dotToolButtonClicked();

    void songRendered(QPixmap render);
};

#endif // COMPOSERWIDGET_H
