#include "propeffect.h"

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

    updateTimer->start(10); // 启动定时器（移到 lambda 外部）
}


void KnifeEffect::remove(Character* character) {
    if (updateTimer) {
        updateTimer->stop();
        // 不需要手动 delete，Qt 会自动管理子对象
    }
}

ShoesEffect::ShoesEffect(int duration,QObject* parent, int speedBoost)
    : PropEffect(parent), speedBoost(speedBoost), duration(duration) {
    updateTimer = std::make_unique<QTimer>(this);
}


void ShoesEffect::apply(Character* character, std::unique_ptr<Prop> prop) {

    // 提升移动速度
    character->setMoveSpeed(character->originalSpeed() + speedBoost);

    // 断开之前的连接
    disconnect(updateTimer.get(), &QTimer::timeout, nullptr, nullptr);

    // 设置定时器，超时后移除效果
    connect(updateTimer.get(), &QTimer::timeout, [character, this]() {
        remove(character);
    });

    updateTimer->start(duration);
}

void ShoesEffect::remove(Character* character) {
    if (updateTimer) {
        updateTimer->stop();
    }

    // 恢复原始速度
    character->setMoveSpeed(character->originalSpeed());
}

TreeEffect::TreeEffect(QObject* parent)
    : PropEffect(parent) {
}

void TreeEffect::apply(Character* character, std::unique_ptr<Prop> /*prop*/) {

}

void TreeEffect::remove(Character* character) {
}

