#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QKeyEvent>
#include <QMovie>
#include <QTimer>
#include "player.h"
#include "aicharacter.h"
#include "selectionline.h"
#include <QMutex>

class MyGraphicsView : public QGraphicsView {
    Q_OBJECT

private:
    void resetGame();
    std::unique_ptr<Player> m_player;
    std::unique_ptr<SelectionLine> m_line;
    QGraphicsPixmapItem *m_background;      // 背景图像项
    QGraphicsEllipseItem *m_moving_circle;  // 运动圆形

    QTimer m_pathTimer;
    QTimer m_pathTimer2;
    QTimer m_pathTimer3;
    QTimer m_healthbarTimer;
    QTimer m_lineUpdateTimer;
    const int SQUARE_SIZE = 500;  // 运动轨迹正方形边长
    int currentEdge = 0;          // 当前所在轨迹边（0-3）
    QPointF initPoint;
    std::vector<std::unique_ptr<HealthBar>> healthBars;
    std::vector<std::unique_ptr<AICharacter>> m_aiCharacters;
    std::vector<std::unique_ptr<SelectionLine>> lines;

    void checkAllAIDead();
    QMutex m_collisionMutex;
    int m_playerRank = -1;

signals:
    void gameOver(bool restart); // 游戏结束信号
    void allAIDead();  // 新增信号
    void victory();     // 新增胜利信号

public slots:
    // 处理信号, 展示游戏窗口
    void handleEvokeGameSignal();

    void onPlayerDeath();

    void onVictory();

public:
    explicit MyGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);
    ~MyGraphicsView();
    void generateProps(int count);
    void checkCollision1();
    void checkCollision2();
    void checkPropCollision(Character* character);
    void checkCharacterCollision(Character* character1, Character* character2);
    const std::vector<std::unique_ptr<AICharacter>>& getAICharacters() const {
        return m_aiCharacters;
    }
    void setPlayerRank(int rank) {
        m_playerRank = rank;
    }
    void stopAllTimers();

    void showEndGameDialog(const QPixmap& pixmap, const QString& title, int rank);

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif
