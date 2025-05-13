#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
#include <QMovie>
#include <QSet>
class MyGraphicsView;
class Player : public Character {
    Q_OBJECT

private:
    QMovie* q_movie;
    QTimer* animationTimer;
    QTimer* movementTimer;
    QSet<int> pressedKeys;
    QString capooGifPath;
    bool isAnimationLoaded;

public:
    explicit Player(int mapRadius, QGraphicsItem* parent = nullptr);

    // 玩家特有方法
    void addPressedKey(int key);
    void removePressedKey(int key);
    void loadAnimation(const QString& gifPath);

    // 重写虚函数
    void setState(CharacterState state) override;
    void handleMovement();
    void updateFrame() override;

    void setView(MyGraphicsView* view);

private slots:
    void onMovementTimeout();
};

#endif // PLAYER_H
