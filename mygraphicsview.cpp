#include "mygraphicsview.h"
#include "propfactory.h"
#include "player.h"
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QMovie>
#include <QPainter>
#include <QTimer>
#include <QMessageBox>

const int bgSize = 3600;
// 槽函数: 处理信号, 展示游戏窗口
void MyGraphicsView::handleEvokeGameSignal() {
    this->show();
}

// 槽函数: 更新运动圆形的路径（这里未涉及角色移动，保持不变）
void MyGraphicsView::updateCirclePath() {
    // 沿正方形轨迹移动
    // 这里用的是什么坐标系?
    const int speed = 3;
    QPointF localPos = m_moving_circle->pos() - initPoint;

    switch (currentEdge) {
    case 0:  // 上边向右移动
        localPos.rx() += speed;
        if (localPos.x() > SQUARE_SIZE / 2) {
            localPos.rx() = SQUARE_SIZE / 2;
            currentEdge = 1;
        }
        break;
    case 1:  // 右边向下移动
        localPos.ry() += speed;
        if (localPos.y() > SQUARE_SIZE / 2) {
            localPos.ry() = SQUARE_SIZE / 2;
            currentEdge = 2;
        }
        break;
    case 2:  // 下边向左移动
        localPos.rx() -= speed;
        if (localPos.x() < -SQUARE_SIZE / 2) {
            localPos.rx() = -SQUARE_SIZE / 2;
            currentEdge = 3;
        }
        break;
    case 3:  // 左边向上移动
        localPos.ry() -= speed;
        if (localPos.y() < -SQUARE_SIZE / 2) {
            localPos.ry() = -SQUARE_SIZE / 2;
            currentEdge = 0;
        }
        break;
    }
    m_moving_circle->setPos(localPos + initPoint);
}

MyGraphicsView::MyGraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent) {
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle("Moving demo");

    // 加载并处理圆形背景
    // ":/" 表示从 qrc 加载资源
    QPixmap backgroundPixmap(":/figs/background3.png");
    if (backgroundPixmap.isNull()) {
        qDebug() << "Failed to load background image!";
    } else {
        qDebug() << "Background image loaded successfully!";
    }

    const int bgSize = 3600;  // 圆形背景的直径
    QPixmap scaledBg =
        backgroundPixmap.scaled(bgSize, bgSize, Qt::KeepAspectRatioByExpanding,
                                Qt::SmoothTransformation);

    QPixmap circularBg(bgSize, bgSize);
    circularBg.fill(Qt::transparent);
    QPainter painter(&circularBg);
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::SmoothPixmapTransform);
    painter.setClipRegion(QRegion(0, 0, bgSize, bgSize, QRegion::Ellipse));
    painter.drawPixmap((bgSize - scaledBg.width()) / 2,
                       (bgSize - scaledBg.height()) / 2, scaledBg);

    m_background = scene->addPixmap(circularBg);
    m_background->setPos(0, 0);
    scene->setSceneRect(0, 0, bgSize, bgSize);

    // 创建运动圆形
    m_moving_circle = new QGraphicsEllipseItem(-15, -15, 30, 30, m_background);
    m_moving_circle->setBrush(Qt::red);
    // 将圆形设置在背景的中心
    QPointF bgCenter = m_background->boundingRect().center();
    m_moving_circle->setPos(bgCenter);  // 初始位置
    initPoint = bgCenter;

    // 加载人物 GIF
    m_player = std::make_unique<Player>(bgSize / 2, m_background);
    QPointF playerPos = bgCenter;
    m_player->setPos(playerPos);
    m_player->setMaxHealth(100);
    m_player->setHealth(100);
    m_player->loadAnimation(":/figs/capoo.gif");

    qreal scaleFactor = 0.3;
    m_player->setScale(scaleFactor);

    connect(m_player.get(), &Player::positionChanged, this, [this]() {
        centerOn(m_player.get()); // 跟随角色移动
    });

    HealthBar* m_healthBar = new HealthBar();
    m_healthBar->loadHealthBarImages(":/props/healthbar/", "png", 11);
    m_healthBar->setTargetCharacter(m_player.get());
    scene->addItem(m_healthBar);  // 必须添加到场景！

    //角色死亡事件绑定
    connect(m_player.get(), &Player::died, this, &MyGraphicsView::onPlayerDeath);
    // 路径运动定时器（每 30ms 更新位置）
    connect(&m_pathTimer, &QTimer::timeout, this,
            &MyGraphicsView::updateCirclePath);
    connect(&m_pathTimer, &QTimer::timeout, this,
            &MyGraphicsView::checkCollision);
    QMetaObject::Connection conn = connect(&m_pathTimer, &QTimer::timeout, m_healthBar, &HealthBar::updateHealthBar);
    if (conn) {
        qDebug() << "Signal and slot connected successfully";
    } else {
        qDebug() << "Failed to connect signal and slot";
    }
    m_pathTimer.start(20);

    // 居中显示人物
    centerOn(m_player.get());

    //初始化生成道具
    generateProps(100);
    //生成ai
}

void MyGraphicsView::keyPressEvent(QKeyEvent *event) {
    m_player->addPressedKey(event->key());
    QGraphicsView::keyPressEvent(event);
}

void MyGraphicsView::keyReleaseEvent(QKeyEvent *event) {
    m_player->removePressedKey(event->key());
    QGraphicsView::keyReleaseEvent(event);
}

//初始化道具生成
void MyGraphicsView::generateProps(int count) {
    QPointF center = m_background->boundingRect().center();
    PropFactory::setMapRadius(bgSize / 2);

    for (int i = 0; i < count; ++i) {
        std::unique_ptr<Prop> prop = PropFactory::createRandomProp(center);
        prop->setZValue(10);
        scene()->addItem(prop.release());
    }
}

template void Prop::interact<Player>(Player*);
//碰撞检测函数
void MyGraphicsView::checkCollision() {
    if (!m_player) return;

    const qreal pickupRadius = 2.0;
    QPointF playerPos = m_player->pos();

    // 先获取大致矩形区域内的项
    QRectF detectArea(
        playerPos.x() - pickupRadius + 1,
        playerPos.y() - pickupRadius + 1,
        pickupRadius * 2,
        pickupRadius * 2
        );

    QList<QGraphicsItem*> items = scene()->items(detectArea);

    for (QGraphicsItem* item : items) {
        Prop* prop = dynamic_cast<Prop*>(item);
        if (prop) {
            // 计算距离
            qreal distance = QLineF(m_player->pos(), prop->pos()).length();
            if (distance < 50) {  // 拾取半径
                prop->interact(m_player.get());
            }
        }
    }
}

void MyGraphicsView::onPlayerDeath() {
    m_pathTimer.stop();
    m_player->setEnabled(false);

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "游戏结束", "你已死亡！是否重新开始？",
        QMessageBox::Yes | QMessageBox::No
        );

    emit gameOver(reply == QMessageBox::Yes); // 发出游戏结束信号
}
