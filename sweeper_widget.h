#ifndef SWEEPER_WIDGET_H
#define SWEEPER_WIDGET_H

#include <QWidget>
#include "sweeper_model.h"

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

#endif // SWEEPER_WIDGET_H
