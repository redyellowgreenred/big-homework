#include "character.h"
#include "player.h"
#include <QPainter>

// 基础道具类实现
Prop::Prop(PropType type, QGraphicsItem* parent)
    : QObject(nullptr), QGraphicsPixmapItem(parent), m_type(type) {
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    setFlag(QGraphicsItem::ItemIgnoresTransformations);

    setZValue(10);
    loadIcon();
    if (type == PropType::Tree) setZValue(20);
}

void Prop::loadIcon() {
    // 根据类型设置不同图标
    switch(m_type) {
    case PropType::Knife:
        m_iconPath = ":/props/knife.png";
        break;
    case PropType::Hp:
        m_iconPath = ":/props/hp.png";

        break;
    case PropType::Shoes:
        m_iconPath = ":/props/shoes.png";

        break;
    case PropType::Tree:
        m_iconPath = ":/props/tree.png";
        break;
    default:
        break;
    }

    QPixmap pixmap(m_iconPath);
    if (!pixmap.isNull()) {
        if (m_type == PropType::Tree) {
            // 为树设置特殊大小（例如300x300像素）
            const QSize treeSize(400, 400);
            setPixmap(pixmap.scaled(treeSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            // 其他道具保持原大小（100x100像素）
            setPixmap(pixmap.scaled(100, 100));
        }
    } else {
        // 默认图标
        QPixmap defaultPixmap(30, 30);
        defaultPixmap.fill(Qt::green);
        setPixmap(defaultPixmap);
    }
}
