#ifndef HEALTHBAR_H
#define HEALTHBAR_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QVector>
#include <QPixmap>

class HealthBar : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    bool isPicked() const {return true;}
    enum { Type = UserType + 1 };
    int type() const override { return Type; }

    explicit HealthBar(QObject* parent = nullptr);
    ~HealthBar() override;

    // 初始化血条图片
    bool loadHealthBarImages(const QString& pathPrefix, const QString& fileExtension, int count = 11);

    // 设置关联的角色
    void setTargetCharacter(class Character* character);


    // 设置血条在角色上方的偏移量
    void setOffset(const QPointF& offset);

public slots:
    void updateHealthBar();

private slots:
    void onCharacterHealthChanged(int health);

private:
    QVector<QPixmap> m_healthBarImages;  // 存储11张血条图片
    Character* m_targetCharacter;        // 关联的角色
    QPointF m_offset;                    // 血条相对于角色的偏移量
    int m_maxHealth;                     // 最大生命值
    int m_currentHealth;                 // 当前生命值
};

#endif // HEALTHBAR_H
