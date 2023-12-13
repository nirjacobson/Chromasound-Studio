#include "pcmusagedialog.h"
#include "ui_pcmusagedialog.h"

PCMUsageDialog::PCMUsageDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PCMUsageDialog),
    _app(nullptr),
    _color1(Qt::cyan),
    _color2(Qt::magenta),
    _color3(Qt::yellow),
    _color4(Qt::lightGray),
    _color5(Qt::red),
    _color6(Qt::green),
    _color7(Qt::blue),
    _color8(Qt::darkMagenta),
    _freeSpaceColor(Qt::white)
{
    ui->setupUi(this);

    _colors.append(&_color1);
    _colors.append(&_color2);
    _colors.append(&_color3);
    _colors.append(&_color4);
    _colors.append(&_color5);
    _colors.append(&_color6);
    _colors.append(&_color7);
    _colors.append(&_color8);

    ui->displayWidget->setItems(&_items);
    ui->legendWidget->setItems(&_items);}

PCMUsageDialog::~PCMUsageDialog()
{
    delete ui;
}

void PCMUsageDialog::setApplication(Application* app)
{
    _app = app;
}

const QColor& PCMUsageDialog::color1() const
{
    return _color1;
}

const QColor& PCMUsageDialog::color2() const
{
    return _color2;
}

const QColor& PCMUsageDialog::color3() const
{
    return _color3;
}

const QColor& PCMUsageDialog::color4() const
{
    return _color4;
}

const QColor& PCMUsageDialog::color5() const
{
    return _color5;
}

const QColor& PCMUsageDialog::color6() const
{
    return _color6;
}

const QColor& PCMUsageDialog::color7() const
{
    return _color7;
}

const QColor& PCMUsageDialog::color8() const
{
    return _color8;
}

const QColor& PCMUsageDialog::freeSpaceColor() const
{
    return _freeSpaceColor;
}

void PCMUsageDialog::setColor1(const QColor& color)
{
    _color1 = color;
}

void PCMUsageDialog::setColor2(const QColor& color)
{
    _color2 = color;
}

void PCMUsageDialog::setColor3(const QColor& color)
{
    _color3 = color;
}

void PCMUsageDialog::setColor4(const QColor& color)
{
    _color4 = color;
}

void PCMUsageDialog::setColor5(const QColor& color)
{
    _color5 = color;
}

void PCMUsageDialog::setColor6(const QColor& color)
{
    _color6 = color;
}

void PCMUsageDialog::setColor7(const QColor& color)
{
    _color7 = color;
}

void PCMUsageDialog::setColor8(const QColor& color)
{
    _color8 = color;
}

void PCMUsageDialog::setFreeSpaceColor(const QColor& color)
{
    _freeSpaceColor = color;
}

QString PCMUsageDialog::sizeString(quint32 bytes)
{
    if (bytes >= 1024) {
        return QString("%1 %2").arg(QString::number((float)bytes / 1024, 'f', 2)).arg("KB");
    } else {
        return QString("%1 %2").arg(bytes).arg("B");
    }
}

void PCMUsageDialog::paintEvent(QPaintEvent* event)
{
    _items.clear();

    QList<QString> paths;
    for (const Channel& channel : _app->project().channels()) {
        if (channel.type() == Channel::Type::PCM) {
            paths.append(dynamic_cast<const PCMChannelSettings&>(channel.settings()).path());
        }
    }

    quint32 totalSize = 0;
    for (int i = 0; i < paths.size(); i++) {
        QFile file(paths[i]);
        QFileInfo fileInfo(file);

        QString name = fileInfo.fileName();
        quint32 size = fileInfo.size();

        totalSize += size;

        _items.append(PCMUsageDisplayItem(name, size, *_colors[i]));
    }

    _items.append(PCMUsageDisplayItem("Free space", MAX_SIZE - totalSize, _freeSpaceColor));

    quint32 pcmSizeFrontPattern = VGMStream().encodeStandardPCM(_app->project(), _app->project().getFrontPattern()).size();
    quint32 pcmSizeSong = VGMStream().encodeStandardPCM(_app->project()).size();

    ui->frontPatternSizeLabel->setText(sizeString(pcmSizeFrontPattern));
    ui->songSizeLabel->setText(sizeString(pcmSizeSong));

    QWidget::paintEvent(event);
}
