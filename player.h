#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
class MyGraphicsView;
class Player : public Character {
    Q_OBJECT

private:
    QTimer* animationTimer;
    QTimer* movementTimer;
    QTimer* knifeTimer;
    QSet<int> pressedKeys;
    QString capooGifPath;

public:
    explicit Player(int mapRadius, QGraphicsItem* parent = nullptr);

    // 玩家特有方法
    void addPressedKey(int key);
    void removePressedKey(int key);

    // 重写虚函数
    void setState(CharacterState state) override;
    void handleMovement();
    void updateFrame() override;

    void setView(MyGraphicsView* view);

signals:
    void attack();

private slots:
    void onMovementTimeout();
    void knifeTimeout();
};

#endif // PLAYER_H
