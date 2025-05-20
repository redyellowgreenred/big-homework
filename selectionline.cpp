#include "selectionline.h"
#include "mygraphicsview.h"
#include "character.h"
#include <QPainter>

SelectionLine::SelectionLine(QGraphicsItem* parent)
    : QGraphicsLineItem(parent) {
    // 初始化线条样式
    m_pen.setColor(QColor(255, 100, 100, 200)); // 半透明红色
    m_pen.setWidth(15);
    m_pen.setStyle(Qt::DashLine);
    setPen(m_pen);

    // 初始化动画
    m_animation = new QPropertyAnimation(this);
    m_animation->setDuration(1000);
    m_animation->setStartValue(0.3);
    m_animation->setEndValue(1.0);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);
}

void SelectionLine::setEndPoints(const QPointF& start, const QPointF& end) {
    setLine(QLineF(start, end));
}

void SelectionLine::animate() {
    if (!player) return;

    // 获取所有AI角色
    MyGraphicsView* view = qobject_cast<MyGraphicsView*>(scene()->views().first());
    if (!view) return;

    const std::vector<std::unique_ptr<AICharacter>>& aiCharacters = view->getAICharacters(); // 需在MyGraphicsView中添加public接口

    // 查找最近的AI
    Character* nearestAI = nullptr;
    qreal minDistance = std::numeric_limits<qreal>::max();
    QPointF playerPos = player->pos();

    for (auto& ai : aiCharacters) {
        if (!ai || !ai->isAlive()) continue;
        qreal distance = QLineF(playerPos, ai->pos()).length();
        if (distance < minDistance) {
            minDistance = distance;
            nearestAI = ai.get();
        }
    }

    if (nearestAI) {
        setEndPoints(playerPos, nearestAI->pos());
        m_animation->stop();
        m_animation->setStartValue(opacity());
        m_animation->setEndValue(1.0); // 显示线段
        m_animation->start();
    } else {
        // 无AI时隐藏线段
        m_animation->stop();
        m_animation->setStartValue(opacity());
        m_animation->setEndValue(0.0); // 隐藏线段
        m_animation->start();
    }
}

void SelectionLine::stopAnimation() {
    if (m_isAnimating) {
        m_animation->stop();
        setOpacity(1.0);
        m_isAnimating = false;
    }
}

void SelectionLine::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    // 先调用基类绘制
    QGraphicsLineItem::paint(painter, option, widget);

    // 添加发光效果（可选）
    if (m_isAnimating) {
        painter->setPen(QPen(QColor(255, 150, 150, 50), 10));
        painter->drawLine(line());
    }
}
