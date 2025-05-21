#include "selectionline.h"
#include "mygraphicsview.h"
#include "character.h"
#include "aicharacter.h" // 包含AI角色头文件
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QPropertyAnimation>
#include <QPixmap>

SelectionLine::SelectionLine(Character* me, Character* nearestAI, QGraphicsItem* parent)
    : QGraphicsLineItem(parent), me(me), nearestAI(nearestAI) {
    // 初始化线条样式
    if (me == player) {
        m_pen.setColor(QColor(255, 100, 100, 250)); // 半透明红色
    } else {
        m_pen.setColor(QColor(0, 0, 255, 20)); // 半透明蓝色
    }
    m_pen.setWidth(10);
    m_pen.setStyle(Qt::DashLine);
    setPen(m_pen);

    // 初始化动画
    m_animation = new QPropertyAnimation(this);
    m_animation->setDuration(1000);
    m_animation->setStartValue(0.3);
    m_animation->setEndValue(1.0);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);

    // 设置 AI 射击间隔
    shootTimer = new QTimer(this);
    shootTimer->setInterval(1000); // 1秒间隔
    connect(shootTimer, &QTimer::timeout, this, &SelectionLine::shoot);
}

SelectionLine::~SelectionLine() {
    stopAnimation();
}

void SelectionLine::setEndPoints(const QPointF& start, const QPointF& end) {
    setLine(QLineF(start, end));
}

void SelectionLine::animate() {
    if (!player || !me) return;

    if (me != player){
        qreal distance = QLineF(me->pos(), nearestAI->pos()).length();
        if (nearestAI && distance < 500) { // 500是有效范围阈值
            setEndPoints(me->pos(), nearestAI->pos());
            m_animation->stop();
            m_animation->setStartValue(0.0);
            m_animation->setEndValue(1.0);
            m_animation->start();
            setVisible(true);
            m_isAnimating = true;
            if (!shootTimer->isActive()) {
                shootTimer->start();
            }
        } else {
            // 超出范围时完全隐藏
            m_animation->stop();
            m_animation->setStartValue(opacity());
            m_animation->setEndValue(0.0);
            m_animation->start();
            setVisible(false);
            m_isAnimating = false;
            shootTimer->stop();
        }
    }
    else{
    // 获取所有AI角色
        MyGraphicsView* view = qobject_cast<MyGraphicsView*>(scene()->views().first());
        if (!view) return;

        const std::vector<std::unique_ptr<AICharacter>>& aiCharacters = view->getAICharacters(); // 需在MyGraphicsView中添加public接口
    // 查找最近的AI
        nearestAI = nullptr;
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

        if (nearestAI && minDistance < 500) { // 500是有效范围阈值
            setEndPoints(playerPos, nearestAI->pos());
            m_animation->stop();
            m_animation->setStartValue(0.0);
            m_animation->setEndValue(1.0);
            m_animation->start();
            setVisible(true);
            m_isAnimating = true;
        } else {
            // 超出范围时完全隐藏
            m_animation->stop();
            m_animation->setStartValue(opacity());
            m_animation->setEndValue(0.0);
            m_animation->start();
            setVisible(false);
            m_isAnimating = false;
            }
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
    if (me == player) {
        // 玩家的索敌线发光效果
        painter->setPen(QPen(QColor(255, 255, 0, 200), 15, Qt::DashLine));
        painter->drawLine(line());
    } else {
        // AI 的索敌线发光效果
        painter->setPen(QPen(QColor(0, 255, 0, 20), 15, Qt::DashLine));
        painter->drawLine(line());
    }
}

void SelectionLine::shoot() {
    if (!m_isAnimating || !nearestAI || !player || !me) return;
    if (!me->knifeCount()) return;

    // 获取玩家和AI位置
    QPointF startPos = me->pos();
    QPointF targetPos = nearestAI->pos();

    // 创建飞刀视觉效果（修改此处）
    QGraphicsPixmapItem* knife = new QGraphicsPixmapItem(nullptr); // 先不设置父对象
    QPixmap knifePixmap(":/props/knife.png");
    if (knifePixmap.isNull()) {
        qDebug() << "Failed to load knife image!";
        delete knife;
        return;
    }

    // 设置飞刀属性
    knife->setPixmap(knifePixmap);
    knife->setPos(startPos);
    knife->setZValue(10); // 确保显示在顶层
    knife->setScale(0.2);

    // 添加到场景（修改此处）
    if (scene()) {
        scene()->addItem(knife);
    } else {
        qDebug() << "Error: Scene is null!";
        delete knife;
        return;
    }

    // 计算飞刀朝向（保持不变）
    QPointF direction = targetPos - startPos;
    qreal angle = qRadiansToDegrees(qAtan2(direction.y(), direction.x())) - 90;
    knife->setRotation(angle);

    // 后续动画代码保持不变...
    // 计算动画参数、创建定时器等...
    // 添加到场景
    scene()->addItem(knife);

    // 计算动画参数
    qreal distance = QLineF(startPos, targetPos).length();
    int duration = qMax(static_cast<int>(distance / 6), 150); // 速度控制
    int steps = duration / 16; // 每16ms更新一次（约60FPS）
    int currentStep = 0;

    // 创建定时器控制动画
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=]() mutable {
        if (currentStep >= steps || !knife->scene()) {
            // 动画结束或场景已销毁
            timer->stop();
            scene()->removeItem(knife);
            delete knife;
            delete timer;
            return;
        }

        // 线性插值计算当前位置
        qreal t = static_cast<qreal>(currentStep) / steps;
        QPointF currentPos = startPos + t * (targetPos - startPos);
        knife->setPos(currentPos);
        currentStep++;
    });

    timer->start(16); // 启动定时器

    // 处理飞刀消耗和伤害逻辑
    me->deleteKnife();
    if (nearestAI->knifeCount()) {
        nearestAI->deleteKnife(); // 双方飞刀抵消
    } else {
        nearestAI->setHealth(nearestAI->health() - 10); // 伤害AI
    }
}
