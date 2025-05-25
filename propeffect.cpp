#include "propeffect.h"
#include "character.h"
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
    : PropEffect(parent), speedBoost(speedBoost), duration(duration),isActive(false) {
    updateTimer = std::make_unique<QTimer>(this);  // 提前创建定时器
    m_positionTimer.setParent(this);
}

void ShoesEffect::apply(Character* character, std::unique_ptr<Prop> prop) {
    // 1. 先清理旧状态
    if (isActive) {
        remove(character);  // 强制移除旧效果
    }

    // 2. 提升速度
    character->setMoveSpeed(character->originalSpeed() + speedBoost);

    // 3. 设置持续时间（单次触发）
    updateTimer->singleShot(duration, [this, character]() {
        remove(character);
    });

    // 4. 创建新图标
    m_icon = new QGraphicsPixmapItem(QPixmap(":/props/shoes.png"), character);
    m_icon->setScale(0.1);
    m_icon->setZValue(15);
    m_icon->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    updateIconPosition(character);  // 初始位置

    // 5. 启动位置更新定时器
    m_positionTimer.start(30);  // 30ms更新一次位置

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
    if (!isActive) return;

    // 1. 停止所有定时器
    updateTimer->stop();
    m_positionTimer.stop();

    // 2. 恢复速度
    character->setMoveSpeed(character->originalSpeed());

    // 3. 移除图标
    if (m_icon) {
        if (m_icon->scene()) {
            m_icon->scene()->removeItem(m_icon);
        }
        delete m_icon;  // 显式删除
        m_icon = nullptr;
    }

    isActive = false;
    character->deleteShoes();
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




















