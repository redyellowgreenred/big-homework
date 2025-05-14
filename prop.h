#define PROP_H
#include <QGraphicsPixmapItem>
#include <QObject>
class Character;
class Player;

// 道具类型枚举
enum class PropType {
    Knife,
    Hp,
    Shoes,
    COUNT
};

class Prop : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Prop(PropType type, QGraphicsItem* parent = nullptr);

    // 获取道具类型
    PropType getType() const { return m_type; }


    // 道具效果持续时间（毫秒），0表示立即效果
    virtual int duration() const { return 0; }

    template <typename CharacterType>
    void interact(CharacterType* character) {
        if (!character || !character->isAlive()) return;
        // 基础交互逻辑
        switch(m_type){
        case PropType::Knife:
        case PropType::Shoes:
        case PropType::Hp:
            character->addProp(std::unique_ptr<Prop>(this));
            break;
        default:
            break;
        }

    }

protected:
    PropType m_type;
    QString m_iconPath; // 道具图标路径

    // 加载道具图标
    void loadIcon();

};
