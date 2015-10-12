#include <QtGui>
#include "../inc/sweeper_widget.h"

SweeperWidget::SweeperWidget(const SweeperModel &sweeperModel, QWidget *parent) : QWidget(parent)
{
    this->sweeperModel = &sweeperModel;
}

void SweeperWidget::mousePressEvent(QMouseEvent *event)
{

}

void SweeperWidget::mouseMoveEvent(QMouseEvent *event)
{

}

void SweeperWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawLine(0, 0, 10, 10);
    painter.drawLine(0, 10, 10, 0);
    QSize size = this->size();
    int height = size.height();
    int width = size.width();
    QRectF fullWidgetCanvasFloating = QRectF(0, 0, width, height);
    painter.fillRect(fullWidgetCanvasFloating, Qt::darkCyan);

    QRect fullWidgetCanvas = QRect(0, 0, width, height);
    QPixmap detonated = QPixmap(":/tiles/detonated.png");
    painter.drawPixmap(fullWidgetCanvas, detonated);
}
