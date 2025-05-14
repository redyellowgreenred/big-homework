#ifndef CHARACTER_H
#define CHARACTER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPropertyAnimation>
#include <QTimer>
#include <QGraphicsScene>
#include <memory>
#include "prop.h"
#include "healthbar.h"

class PropEffect;

//角色状态枚举
enum class CharacterState {
    Idle,
    Moving,
    Attacking,
    Hurt,
    Dead
};

class Prop;

class Character : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

private:
    //飞刀存储系统
    std::vector<std::unique_ptr<Prop>> m_knives;
    const float m_knifeDistance = 60; // 增大旋转半径

    //血条
    std::unique_ptr<HealthBar> p_healthBar;

protected:  // 改为protected以便子类访问
    int p_hp;
    int p_maxhp;
    int p_originalSpeed;
    int p_moveSpeed;
    int mapRadius;
    CharacterState p_state;
    QPropertyAnimation* moveAnimation;
    QPointF targetPos;
    bool isMoving;

    explicit Character(int mapRadius, QGraphicsItem* parent = nullptr);

public:
    virtual ~Character();
    std::unordered_map<PropType, std::unique_ptr<PropEffect>> m_propEffects;

    // 通用方法
    int health() const;
    int maxHealth() const;
    int originalSpeed() const;
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

    //道具系统
    void addProp(std::unique_ptr<Prop> prop);

    //飞刀系统
    void addKnife(std::unique_ptr<Prop> prop){m_knives.push_back(std::move(prop));}
    int knifeCount() const { return m_knives.size(); }
    void updateKnivesPosition(); // 更新飞刀位置

    //血条系统
    void setMyHealthbar(std::unique_ptr<HealthBar> bar);
signals:
    void healthChanged(int newHealth);
    void positionChanged(QPointF newPos);
    void stateChanged(CharacterState newState);
    void died();

protected slots:
    virtual void updateFrame() {}  // 空实现
};

#endif // CHARACTER_H

