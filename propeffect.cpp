#include "propeffect.h"
#include <QGraphicsPixmapItem>
#include <QtCore/qmath.h> // 确保包含必要的头文件

KnifeEffect::KnifeEffect(QObject* parent) : PropEffect(parent) {
    updateTimer = std::make_unique<QTimer>(this);
}

void KnifeEffect::apply(Character *character, std::unique_ptr<Prop> prop){
    character->addKnife(std::move(prop));
    // 断开之前的连接，避免重复连接
    disconnect(updateTimer.get(), &QTimer::timeout, nullptr, nullptr);

    connect(updateTimer.get(), &QTimer::timeout, [character, this]() {
        character->updateKnivesPosition();
    });

    updateTimer->start(10); // 启动定时器
}

void KnifeEffect::remove(Character* character) {
    if (updateTimer) {
        updateTimer->stop();
    }
}

ShoesEffect::ShoesEffect(int duration,QObject* parent, int speedBoost)
    : PropEffect(parent), speedBoost(speedBoost), duration(duration) {
    updateTimer = std::make_unique<QTimer>(this);
    m_positionTimer.setInterval(30);
}

void ShoesEffect::apply(Character* character, std::unique_ptr<Prop> prop) {
    if (isActive) {
        // 如果效果已激活，先移除旧图标
        if (m_icon) {
            if (m_icon->scene()) {
                m_icon->scene()->removeItem(m_icon);
            }
            delete m_icon;
            m_icon = nullptr;
        }
    }
    // 提升移动速度
    character->setMoveSpeed(character->originalSpeed() + speedBoost);

    // 断开之前的连接
    disconnect(updateTimer.get(), &QTimer::timeout, nullptr, nullptr);

    // 设置定时器，超时后移除效果（只保留一种方式）
    updateTimer->singleShot(duration, [this, character]() {
        remove(character);
    });

    // 创建图标并设置为角色的子项
    m_icon = new QGraphicsPixmapItem(QPixmap(":/props/shoes.png"), character);
    m_icon->setScale(0.1);
    m_icon->setZValue(15);
    m_icon->setFlag(QGraphicsItem::ItemIgnoresTransformations);

    // 初始位置调整
    updateIconPosition(character);

    // 连接位置更新定时器
    connect(&m_positionTimer, &QTimer::timeout,
            [this, character]() { updateIconPosition(character); });
    m_positionTimer.start();

    isActive = true;
}

void ShoesEffect::updateIconPosition(Character* character) {
    if (!m_icon || !character) return;

    // 计算相对位置
    QRectF charRect = character->boundingRect();
    qreal x = charRect.width()/2 - m_icon->boundingRect().width()/2 * m_icon->scale()-200;
    qreal y = charRect.height()-150;

    m_icon->setPos(x, y);
}

void ShoesEffect::remove(Character* character) {
    if (updateTimer && updateTimer->isActive()) {
        updateTimer->stop();
    }
    if (m_positionTimer.isActive()) {
        m_positionTimer.stop();
    }

    // 恢复原始速度
    character->setMoveSpeed(character->originalSpeed());

    // 图标由Qt的父对象机制自动删除，只需置空指针
    if (m_icon && m_icon->scene()) {
        m_icon->scene()->removeItem(m_icon);
    }
    m_icon = nullptr;
    isActive = false;
}

TreeEffect::TreeEffect(QObject* parent)
    : PropEffect(parent) {
}

void TreeEffect::apply(Character* character, std::unique_ptr<Prop> prop) {
    // 如果道具需要被使用，应该在这里处理
    // 例如：character->addTree(std::move(prop));
}

void TreeEffect::remove(Character* character) {
    // 如果有效果需要移除，在这里实现
}

HpEffect::HpEffect(QObject* parent) : PropEffect(parent) {}

void HpEffect::apply(Character* character, std::unique_ptr<Prop> prop) {
    // 直接增加生命值
    int currentHp = character->health();
    int newHp = qMin(currentHp + 10, character->maxHealth());
    character->setHealth(newHp);
}

void HpEffect::remove(Character* character) {
    // 生命值加成无需移除
}




















