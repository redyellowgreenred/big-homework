#include "player.h"
#include <QDebug>

Player::Player(int mapRadius, QGraphicsItem* parent)
    : Character(mapRadius, parent),
    animationTimer(new QTimer(this)),
    movementTimer(new QTimer(this)),
    knifeTimer(new QTimer(this)),
    capooGifPath(":/figs/capoo.gif") {

    // 初始化动画系统
    loadAnimation(capooGifPath);

    // 设置定时器
    connect(movementTimer, &QTimer::timeout, this, &Player::onMovementTimeout);
    movementTimer->start(10);
    connect(knifeTimer, &QTimer::timeout, this, &Player::knifeTimeout);
    knifeTimer->start(2000);
}

void Player::addPressedKey(int key) {
    pressedKeys.insert(key);
    if (key == Qt::Key_R) setHealth(p_hp - 10);
    if (key == Qt::Key_J) emit attack();
}

void Player::removePressedKey(int key) {
    pressedKeys.remove(key);
}

void Player::setState(CharacterState state) {
    if (p_state == state) return;

    Character::setState(state);  // 调用父类实现

}

void Player::onMovementTimeout() {
    if (p_state == CharacterState::Dead) {
        return;
    }
    QVector2D direction(0, 0);
    const int speed = p_moveSpeed / 62.5;

    QPointF currentPos = pos();
    QPointF centerPoint = QPointF(mapRadius, mapRadius); // 假设圆心在 (mapRadius, mapRadius)
    QLineF lineToCenter(currentPos, centerPoint);
    qreal distanceToCenter = lineToCenter.length();

    bool canMoveUp = true;
    bool canMoveDown = true;
    bool canMoveLeft = true;
    bool canMoveRight = true;

    if (distanceToCenter + speed >= mapRadius - 50) {
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
    if (p_state != CharacterState::Dead){
        if (canMoveDown && pressedKeys.contains(Qt::Key_W)) direction.setY(-speed);
        if (canMoveUp && pressedKeys.contains(Qt::Key_S)) direction.setY(speed);
        if (canMoveRight && pressedKeys.contains(Qt::Key_A)) direction.setX(-speed);
        if (canMoveLeft && pressedKeys.contains(Qt::Key_D)) direction.setX(speed);
    }

    if (direction.x() != 0 || direction.y() != 0) {
        setState(CharacterState::Moving);
        if (pressedKeys.contains(Qt::Key_D)||pressedKeys.contains(Qt::Key_A)||pressedKeys.contains(Qt::Key_S)||pressedKeys.contains(Qt::Key_W)) updateFrame();
    } else {
        setState(CharacterState::Idle);
    }

    QPointF newPos = currentPos + QPointF(direction.x(), direction.y());
    setPos(newPos);
}

void Player::knifeTimeout() {
    if (knifeCount() < 4){
        auto knife = std::make_unique<Prop>(PropType::Knife);
        knife->setZValue(10);
        knife->pick();
        scene()->addItem(knife.get());
        addProp(std::move(knife));
    }
}

void Player::updateFrame() {
    if (q_movie && q_movie->state() == QMovie::Running) {
        if (!q_movie->currentPixmap().isNull()) {
            setPixmap(q_movie->currentPixmap());
            update();
        }
    }
}
