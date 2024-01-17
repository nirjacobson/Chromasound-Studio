#include "melodypatchsetwidget.h"
#include "ui_melodypatchsetwidget.h"

MelodyPatchsetWidget::MelodyPatchsetWidget(QWidget *parent, Application* app) :
    QWidget(parent),
    ui(new Ui::MelodyPatchsetWidget),
    _app(app)
{
    ui->setupUi(this);
    ui->patchSetComboBox->setCurrentIndex(-1);
    ui->patchListView->setModel(&_listModel);
    ui->patchListView->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    connect(ui->patchSetComboBox, &QComboBox::currentIndexChanged, this, &MelodyPatchsetWidget::patchsetChanged);
}

MelodyPatchsetWidget::~MelodyPatchsetWidget()
{
    delete ui;
}

void MelodyPatchsetWidget::setApplication(Application* app)
{
    _app = app;

    blockSignals(true);
    ui->patchSetComboBox->setCurrentIndex(app->project().opllType());
    blockSignals(false);
}

OPLL::Type MelodyPatchsetWidget::selectedPatchset() const
{
    return static_cast<OPLL::Type>(ui->patchSetComboBox->currentIndex());
}

void MelodyPatchsetWidget::setPatchset(const OPLL::Type type)
{
    blockSignals(true);
    ui->patchSetComboBox->setCurrentIndex(type);
    blockSignals(false);
}

void MelodyPatchsetWidget::patchsetChanged(const int index)
{
    _listModel.setStringList(OPLL::patches(static_cast<OPLL::Type>(index)));

    emit changed();
}

