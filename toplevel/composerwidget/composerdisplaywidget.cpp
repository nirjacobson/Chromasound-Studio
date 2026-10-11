#include "composerdisplaywidget.h"

ComposerDisplayWidget::ComposerDisplayWidget(QWidget* parent)
	: QWidget(parent)
{

}

ComposerDisplayWidget::~ComposerDisplayWidget()
{

}

void ComposerDisplayWidget::paintEvent(QPaintEvent *event) {
	QWidget::paintEvent(event);

	QPainter painter(this);

	painter.fillRect(rect(), QColor::fromRgb(255, 255, 255));
}

