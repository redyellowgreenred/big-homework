#ifndef CHARACTER_H
#define CHARACTER_H

#include <QMovie>
#include <QSet>
#include <QPainter>
#include <QVector2D>
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

    //血条（改为普通指针）
    HealthBar* p_healthBar = nullptr;  // 普通指针

    QPixmap m_deadPixmap;       // 死亡静态图片
    bool m_isDead = false;

protected:  // 改为protected以便子类访问
    double m_baseAngle = 0;
    QMovie* q_movie;
    bool isAnimationLoaded;
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
    bool isPicked() const {return true;}

    // 通用方法（内联实现）
    int health() const { return p_hp; }
    int maxHealth() const { return p_maxhp; }
    int originalSpeed() const { return p_originalSpeed; }
    int moveSpeed() const { return p_moveSpeed; }
    CharacterState state() const { return p_state; }
    bool isAlive() const { return p_hp > 0; }

    virtual void setHealth(int health);
    virtual void setMaxHealth(int maxHealth);
    virtual void setMoveSpeed(int speed);
    virtual void setState(CharacterState state);
    void setPos(const QPointF& pos) {
        QGraphicsPixmapItem::setPos(pos);
        emit positionChanged(pos);
    }
    float getDistance(){return m_knifeDistance *(0.1 + knifeCount() * 0.05);}
    virtual void moveTo(const QPointF& pos);
    virtual void stopMoving();
    void loadAnimation(const QString& gifPath);

    HealthBar* getHealthBar() { return p_healthBar; }

    void die();
    void loadDeadImage(const QString& imagePath); // 加载死亡图片
    void setDeadState(bool isDead) { m_isDead = isDead; } // 设置死亡状态

    //道具系统
    void addProp(std::unique_ptr<Prop> prop);

    //飞刀系统
    void addKnife(std::unique_ptr<Prop> prop) { m_knives.push_back(std::move(prop)); }
    void deleteKnife();
    int knifeCount() const { return m_knives.size(); }
    void updateKnivesPosition(); // 更新飞刀位置

    //血条系统
    void setMyHealthbar(HealthBar* bar) {
        p_healthBar = bar;
        if (p_healthBar) {
            p_healthBar->setTargetCharacter(this);
        }
    }

signals:
    void healthChanged(int newHealth);
    void positionChanged(QPointF newPos);
    void stateChanged(CharacterState newState);
    void died();

protected slots:
    virtual void updateFrame() {}  // 空实现
};

#endif // CHARACTER_H
