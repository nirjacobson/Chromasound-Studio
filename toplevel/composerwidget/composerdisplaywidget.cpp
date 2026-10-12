#include "composerdisplaywidget.h"

ComposerDisplayWidget::ComposerDisplayWidget(QWidget* parent)
	: QWidget(parent)
	, _pixmap(nullptr)
{

}

ComposerDisplayWidget::~ComposerDisplayWidget()
{

}

void ComposerDisplayWidget::show(QPixmap* pixmap) {
	_pixmap = pixmap;
	update();
}

void ComposerDisplayWidget::paintEvent(QPaintEvent *event) {
	QWidget::paintEvent(event);

	QPainter painter(this);
	painter.fillRect(rect(), Qt::white);

	if (_pixmap != nullptr) {
		painter.drawPixmap(rect(), *_pixmap);
	}
}


