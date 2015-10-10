#ifndef SWEEPERWIDGET_H
#define SWEEPERWIDGET_H

#include <QWidget>
#include "sweepermodel.h"

class SweeperWidget : public QWidget
{
    Q_OBJECT
public:
    SweeperWidget(const SweeperModel &sweeperModel, QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    SweeperModel const *sweeperModel;
};

#endif // SWEEPERGRID_H
