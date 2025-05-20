// aicharacter.cpp
#include "aicharacter.h"
#include <QRandomGenerator>
#include <QGraphicsScene>
#include <QDebug>
#include <cmath>

AICharacter::AICharacter(int mapRadius, QGraphicsItem *parent)
    : Character(mapRadius, parent),
    m_aiTimerId(-1),
    m_targetPlayer(nullptr),
    m_playerInSight(false),
    m_detectionRadius(300.0)
{
    int interval = QRandomGenerator::global()->bounded(1000, 3000);
    m_decisionTimer.setInterval(interval);
    connect(&m_decisionTimer, &QTimer::timeout, this, &AICharacter::makeDecision);


    connect(&m_movementTimer, &QTimer::timeout, this, &AICharacter::updateMovement);
    m_movementTimer.start(40);

    loadAnimation(":/figs/capoo.gif");
    setScale(0.3);
}

AICharacter::~AICharacter()
{
    stopAI();
}

void AICharacter::startAI()
{
    if (m_aiTimerId == -1) {
        m_aiTimerId = startTimer(100); // 每100ms更新一次
        m_decisionTimer.start();
    }
}

void AICharacter::stopAI()
{
    if (m_aiTimerId != -1) {
        killTimer(m_aiTimerId);
        m_aiTimerId = -1;
        m_decisionTimer.stop();
    }
}

void AICharacter::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    // 更新刀子位置
    updateKnivesPosition();

    // 检测玩家
    // detectPlayer();
}

void AICharacter::makeDecision()
{
    if (!isAlive()) return;

    //瞎鸡儿走
    moveRandomly();
    // if (m_playerInSight) {
    //     // 根据AI状态决定行为
    //     int decision = QRandomGenerator::global()->bounded(100);

    //     if (health() < maxHealth() * 0.3) {
    //         // 血量低时更可能逃跑
    //         if (decision < 70) {
    //             fleeFromPlayer();
    //         } else {
    //             chasePlayer();
    //         }
    //     } else {
    //         // 血量高时更可能追击
    //         if (decision < 70) {
    //             chasePlayer();
    //         } else {
    //             fleeFromPlayer();
    //         }
    //     }
    // } else {
    //     // 玩家不在视野内，随机移动
    //     if (QRandomGenerator::global()->bounded(100) < 80) {
    //         moveRandomly();
    //     }
    // }
}

void AICharacter::moveRandomly()
{
    if (!isAlive()) return;

    // 随机角度和距离
    qreal angle = QRandomGenerator::global()->bounded(360);
    qreal distance = QRandomGenerator::global()->bounded(200, 400);

    m_currentTarget = pos() + QPointF(
                          distance * std::cos(qDegreesToRadians(angle)),
                          distance * std::sin(qDegreesToRadians(angle))
                          );

    // 确保目标在场景内
    QRectF sceneRect = scene()->sceneRect();
    m_currentTarget.setX(qBound(sceneRect.left() + 60, m_currentTarget.x(), sceneRect.right() - 50));
    m_currentTarget.setY(qBound(sceneRect.top() + 60, m_currentTarget.y(), sceneRect.bottom() - 50));
}

void AICharacter::updateMovement()
{
    if (!isAlive()) return;

    QPointF direction = m_currentTarget - pos();
    qreal distance = std::sqrt(QPointF::dotProduct(direction, direction));

    // 如果接近目标或超出边界，选择新目标
    if (distance < 20 || !scene()->sceneRect().contains(pos())) {
        moveRandomly();
        return;
    }

    // 标准化方向并移动
    direction /= distance;
    QPointF newPos = pos() + direction * p_moveSpeed * 0.05;
    setPos(newPos);

    // 更新动画
    setState(CharacterState::Moving);
    updateFrame();
}
// void AICharacter::chasePlayer()
// {
//     if (!m_playerInSight || !isAlive()) return;

//     // 计算朝向玩家的方向
//     QPointF direction = m_lastKnownPlayerPos - pos();
//     qreal distance = std::sqrt(QPointF::dotProduct(direction, direction));

//     // 如果距离足够近，停止移动
//     if (distance < 100) {
//         stopMoving();
//         return;
//     }

//     // 移动到玩家位置
//     moveTo(m_lastKnownPlayerPos);
// }

// void AICharacter::fleeFromPlayer()
// {
//     if (!m_playerInSight || !isAlive()) return;

//     // 计算远离玩家的方向
//     QPointF direction = pos() - m_lastKnownPlayerPos;
//     qreal distance = std::sqrt(QPointF::dotProduct(direction, direction));
//     direction /= distance; // 归一化

//     // 逃跑距离
//     qreal fleeDistance = QRandomGenerator::global()->bounded(200, 400);
//     QPointF newPos = pos() + direction * fleeDistance;

//     // 确保不超出地图边界
//     QRectF sceneRect = scene()->sceneRect();
//     newPos.setX(qBound(sceneRect.left() + 50, newPos.x(), sceneRect.right() - 50));
//     newPos.setY(qBound(sceneRect.top() + 50, newPos.y(), sceneRect.bottom() - 50));

//     moveTo(newPos);
// }

// void AICharacter::detectPlayer()
// {
//     if (!m_targetPlayer || !scene()) {
//         m_playerInSight = false;
//         return;
//     }

//     // 计算与玩家的距离
//     QPointF playerPos = m_targetPlayer->pos();
//     qreal distance = QLineF(pos(), playerPos).length();

//     if (distance <= m_detectionRadius) {
//         m_playerInSight = true;
//         m_lastKnownPlayerPos = playerPos;
//     } else {
//         m_playerInSight = false;
//     }
// }
