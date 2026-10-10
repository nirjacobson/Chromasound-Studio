#ifndef COMPOSERCHANNELSELECTOR_H
#define COMPOSERCHANNELSELECTOR_H

#include <QWidget>

namespace Ui {
class ComposerChannelSelector;
}

class ComposerChannelSelector : public QWidget
{
    Q_OBJECT

public:
    explicit ComposerChannelSelector(QWidget *parent = nullptr);
    ~ComposerChannelSelector();

private:
    Ui::ComposerChannelSelector *ui;
};

#endif // COMPOSERCHANNELSELECTOR_H
