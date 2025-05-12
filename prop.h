#define PROP_H

#include "character.h"
#include <QGraphicsPixmapItem>
#include <QObject>

class Prop : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Prop(PropType type, QGraphicsItem* parent = nullptr);

    // 获取道具类型
    PropType getType() const { return m_type; }


    // 道具效果持续时间（毫秒），0表示立即效果
    virtual int duration() const { return 0; }

    virtual void interact(Character* character);

protected:
    PropType m_type;
    QString m_iconPath; // 道具图标路径

    // 加载道具图标
    void loadIcon();

signals:
    void removeRequested(Prop* prop);
};
