#include "prop.h"
#include <QPainter>

// 基础道具类实现
Prop::Prop(PropType type, QGraphicsItem* parent)
    : QObject(nullptr), QGraphicsPixmapItem(parent), m_type(type) {
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    setFlag(QGraphicsItem::ItemIgnoresTransformations);

    loadIcon();
}

void Prop::loadIcon() {
    // 根据类型设置不同图标
    switch(m_type) {
    case PropType::Knife:
        m_iconPath = ":/props/knife.png";

        break;
    default:
        break;
    }

    QPixmap pixmap(m_iconPath);
    if(!pixmap.isNull()) {
        setPixmap(pixmap.scaled(100, 100));
    } else {
        // 默认图标
        QPixmap defaultPixmap(30, 30);
        defaultPixmap.fill(Qt::green);
        setPixmap(defaultPixmap);
    }
}

void Prop::interact(Character* character) {
    if (!character || !character->isAlive()) return;
    // 基础交互逻辑
    switch(m_type){
    case PropType::Knife:
        emit removeRequested(this);
        break;
    default:
        break;
    }

}
