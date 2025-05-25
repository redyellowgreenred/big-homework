#include "aicharacter.h"
#include <QRandomGenerator>
#include <QGraphicsScene>
#include <QDebug>
#include <cmath>
#include <QPainter>  // 用于调试绘制

AICharacter::AICharacter(int mapRadius, QGraphicsItem *parent)
    : Character(mapRadius, parent),
    m_aiTimerId(-1),
    m_targetPlayer(nullptr),
    m_playerInSight(false),
    m_detectionRadius(300.0),
    m_safetyMargin(30.0)  // 安全边距默认30像素
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
    updateKnivesPosition();
}

bool AICharacter::isPositionValid(const QPointF& pos) const
{
    if (!scene()) return false;

    QRectF sceneRect = scene()->sceneRect();
    QRectF charRect = boundingRect();

    // 考虑角色尺寸和安全边距
    qreal effectiveMargin = charRect.width()/2 + m_safetyMargin;
    return pos.x() >= sceneRect.left() + effectiveMargin &&
           pos.x() <= sceneRect.right() - effectiveMargin &&
           pos.y() >= sceneRect.top() + effectiveMargin &&
           pos.y() <= sceneRect.bottom() - effectiveMargin;
}

void AICharacter::makeDecision()
{
    if (!isAlive()) return;
    moveRandomly();
}

void AICharacter::moveRandomly()
{
    if (!isAlive() || !scene()) return;

    // 在当前位置附近生成有效目标
    int attempts = 0;
    const int maxAttempts = 5;
    QPointF center = scene()->sceneRect().center();

    do {
        qreal angle = QRandomGenerator::global()->bounded(360);
        qreal distance = QRandomGenerator::global()->bounded(150, 300);

        m_currentTarget = pos() + QPointF(
                              distance * std::cos(qDegreesToRadians(angle)),
                              distance * std::sin(qDegreesToRadians(angle))
                              );
        attempts++;
    } while (!isPositionValid(m_currentTarget) && attempts < maxAttempts);

    // Fallback: 如果多次尝试无效，则向场景中心移动
}

void AICharacter::updateMovement()
{
    if (!isAlive() || !scene()) return;

    // 边界检查：如果当前位置已经出界，立即纠正
    if (!isPositionValid(pos())) {
        QPointF correction = scene()->sceneRect().center() - pos();
        correction /= std::sqrt(QPointF::dotProduct(correction, correction));
        setPos(pos() + correction * 10);  // 快速回归场景
        moveRandomly();
        return;
    }

    QPointF direction = m_currentTarget - pos();
    qreal distance = std::sqrt(QPointF::dotProduct(direction, direction));

    // 如果接近目标或需要重新规划路径
    if (distance < 20) {
        moveRandomly();
        return;
    }

    // 标准化方向并移动
    direction /= distance;
    QPointF newPos = pos() + direction * p_moveSpeed * 0.05;

    // 确保新位置有效
    if (isPositionValid(newPos)) {
        setPos(newPos);
        setState(CharacterState::Moving);
    } else {
        // 如果方向上有障碍，稍微调整角度
        qreal adjustedAngle = QRandomGenerator::global()->bounded(-30, 30);
        qreal newAngle = qRadiansToDegrees(std::atan2(direction.y(), direction.x())) + adjustedAngle;
        direction.setX(std::cos(qDegreesToRadians(newAngle)));
        direction.setY(std::sin(qDegreesToRadians(newAngle)));
        newPos = pos() + direction * p_moveSpeed * 0.03;

        if (isPositionValid(newPos)) {
            setPos(newPos);
        } else {
            moveRandomly(); // 实在无法移动则重新规划
        }
    }
    updateFrame();
}

