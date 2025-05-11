#ifndef CHARACTER_H
#define CHARACTER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPropertyAnimation>

enum class CharacterState {
    Idle,
    Moving,
    Attacking,
    Hurt,
    Dead
};

class Character : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

protected:  // 改为protected以便子类访问
    int p_hp;
    int p_maxhp;
    int p_moveSpeed;
    int mapRadius;
    CharacterState p_state;
    QPropertyAnimation* moveAnimation;
    QPointF targetPos;
    bool isMoving;

    explicit Character(int mapRadius, QGraphicsItem* parent = nullptr);

public:
    virtual ~Character();

    // 通用方法
    int health() const;
    int maxHealth() const;
    int moveSpeed() const;
    CharacterState state() const;
    bool isAlive() const;

    virtual void setHealth(int health);
    virtual void setMaxHealth(int maxHealth);
    virtual void setMoveSpeed(int speed);
    virtual void setState(CharacterState state);
    void setPos(const QPointF& pos);
    virtual void moveTo(const QPointF& pos);
    virtual void stopMoving();

signals:
    void healthChanged(int newHealth);
    void positionChanged(QPointF newPos);
    void stateChanged(CharacterState newState);
    void died();

protected slots:
    virtual void updateFrame() {}  // 空实现
};

#endif // CHARACTER_H
