#include "character.h"
#include <QDebug>

Character::Character(int mapRadius, QGraphicsItem* parent)
    : QObject(nullptr), QGraphicsPixmapItem(parent),
    p_hp(100), p_maxhp(100), p_moveSpeed(200),mapRadius(mapRadius),
    p_state(CharacterState::Idle),
    moveAnimation(new QPropertyAnimation(this, "pos", this)),
    isMoving(false) {
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

Character::~Character() {
    delete moveAnimation;
}


int Character::health() const{
    return p_hp;
}

int Character::maxHealth() const
{
    return p_maxhp;
}

int Character::moveSpeed() const
{
    return p_moveSpeed;
}

CharacterState Character::state() const
{
    return p_state;
}

bool Character::isAlive() const
{
    return p_hp > 0;
}

void Character::setHealth(int health)
{
    if (p_hp == health) return;

    p_hp = health;
    emit healthChanged(p_hp);

    if (p_hp <= 0) {
        p_hp = 0;
        setState(CharacterState::Dead);
        emit died();
    }
}

void Character::setMaxHealth(int maxHealth)
{
    p_maxhp = maxHealth;
    if (p_hp > p_maxhp) {
        p_hp = p_maxhp;
        emit healthChanged(p_hp);
    }
}

void Character::setMoveSpeed(int speed)
{
    p_moveSpeed = speed;
}

void Character::setState(CharacterState state) {
    if (p_state == state) return;

    p_state = state;
    emit stateChanged(p_state);

}

void Character::setPos(const QPointF &pos)
{
    QGraphicsPixmapItem::setPos(pos);
    emit positionChanged(pos);
}


void Character::moveTo(const QPointF& pos)
{
    if (!isAlive() || p_state == CharacterState::Dead) return;

    targetPos = pos;
    isMoving = true;

    // 根据距离和移动速度计算动画持续时间
    qreal distance = QLineF(this->pos(), pos).length();
    int duration = static_cast<int>(distance / p_moveSpeed * 1000); // 毫秒

    moveAnimation->stop();
    moveAnimation->setStartValue(this->pos());
    moveAnimation->setEndValue(pos);
    moveAnimation->setDuration(duration);
    moveAnimation->start();

    if (p_state != CharacterState::Attacking && p_state != CharacterState::Hurt) {
        setState(CharacterState::Moving);
    }
}

void Character::stopMoving()
{
    if (isMoving) {
        moveAnimation->stop();
        isMoving = false;
        if (p_state == CharacterState::Moving) {
            setState(CharacterState::Idle);
        }
    }
}

void Character::addProp(PropType prop){
    switch(prop){
    case PropType::Knife:
        break;
    default:
        break;
    }
}
