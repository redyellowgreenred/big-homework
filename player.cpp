#include "player.h"
#include <QDebug>
#include <QPainter>
#include <QVector2D>

Player::Player(int mapRadius, QGraphicsItem* parent)
    : Character(mapRadius, parent),
    q_movie(nullptr),
    animationTimer(new QTimer(this)),
    movementTimer(new QTimer(this)),
    capooGifPath(":/figs/capoo.gif"),
    isAnimationLoaded(false) {

    // 初始化动画系统
    loadAnimation(capooGifPath);

    // 设置定时器
    connect(movementTimer, &QTimer::timeout, this, &Player::onMovementTimeout);
    movementTimer->start(16);
}

Player::~Player() {
    if (q_movie) {
        q_movie->stop();
        delete q_movie;
    }
}

void Player::addPressedKey(int key) {
    pressedKeys.insert(key);
}

void Player::removePressedKey(int key) {
    pressedKeys.remove(key);
}

void Player::loadAnimation(const QString& gifPath) {
    if (q_movie) {
        q_movie->stop();
        delete q_movie;
    }

    q_movie = new QMovie(gifPath, QByteArray(), this);
    if (q_movie->isValid()) {
        q_movie->setCacheMode(QMovie::CacheAll);
        q_movie->start();
        setPixmap(q_movie->currentPixmap());
        setOffset(-q_movie->currentPixmap().width()/4,
                  -q_movie->currentPixmap().height()/4);
        isAnimationLoaded = true;
    } else {
        qDebug() << "Failed to load animation:" << gifPath;
        QPixmap defaultPixmap(16, 16);
        QPainter painter(&defaultPixmap);
        painter.fillRect(defaultPixmap.rect(), Qt::red);
        setPixmap(defaultPixmap);
    }
}

void Player::setState(CharacterState state) {
    if (p_state == state) return;

    Character::setState(state);  // 调用父类实现

}

void Player::onMovementTimeout() {
    QVector2D direction(0, 0);
    const int speed = p_moveSpeed / 62.5;

    QPointF currentPos = pos();
    QPointF centerPoint = QPointF(mapRadius, mapRadius-100); // 假设圆心在 (mapRadius, mapRadius)
    QLineF lineToCenter(currentPos, centerPoint);
    qreal distanceToCenter = lineToCenter.length();

    bool canMoveUp = true;
    bool canMoveDown = true;
    bool canMoveLeft = true;
    bool canMoveRight = true;

    if (distanceToCenter + speed >= mapRadius) {
        if (currentPos.y() < centerPoint.y()) {
            canMoveDown = false;
        }if (currentPos.y() > centerPoint.y()){
            canMoveUp = false;
        }
        if (currentPos.x() < centerPoint.x()) {
            canMoveRight = false;
        }if (currentPos.x() > centerPoint.x()) {
            canMoveLeft = false;
        }
    }

    if (canMoveDown && pressedKeys.contains(Qt::Key_W)) direction.setY(-speed);
    if (canMoveUp && pressedKeys.contains(Qt::Key_S)) direction.setY(speed);
    if (canMoveRight && pressedKeys.contains(Qt::Key_A)) direction.setX(-speed);
    if (canMoveLeft && pressedKeys.contains(Qt::Key_D)) direction.setX(speed);

    if (direction.x() != 0 || direction.y() != 0) {
        setState(CharacterState::Moving);
        if (pressedKeys.contains(Qt::Key_D)||pressedKeys.contains(Qt::Key_A)||pressedKeys.contains(Qt::Key_S)||pressedKeys.contains(Qt::Key_W)) updateFrame();
    } else {
        setState(CharacterState::Idle);
    }

    QPointF newPos = currentPos + QPointF(direction.x(), direction.y());
    setPos(newPos);
}

void Player::updateFrame() {
    if (q_movie && q_movie->state() == QMovie::Running) {
        if (!q_movie->currentPixmap().isNull()) {
            setPixmap(q_movie->currentPixmap());
            update();
        }
    }
}
