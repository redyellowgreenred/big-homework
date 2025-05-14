#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QMovie>
#include <QTimer>
#include "player.h"

class MyGraphicsView : public QGraphicsView {
private:
    std::unique_ptr<Player> m_player;
    QGraphicsPixmapItem *m_background;      // 背景图像项
    QGraphicsEllipseItem *m_moving_circle;  // 运动圆形

    QTimer m_pathTimer;
    const int SQUARE_SIZE = 500;  // 运动轨迹正方形边长
    int currentEdge = 0;          // 当前所在轨迹边（0-3）
    QPointF initPoint;
    std::unique_ptr<HealthBar> m_healthBar;

public slots:
    // 处理信号, 展示游戏窗口
    void handleEvokeGameSignal();

    // 更新运动圆形的路径
    void updateCirclePath();

public:
    explicit MyGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);
    void generateProps(int count);
    void checkCollision();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif
