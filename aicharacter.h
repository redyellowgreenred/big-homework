// aicharacter.h
#ifndef AICHARACTER_H
#define AICHARACTER_H

#include "character.h"
#include <QTimer>

class AICharacter : public Character
{
    Q_OBJECT
public:
    explicit AICharacter(int mapRadius, QGraphicsItem *parent = nullptr);
    ~AICharacter();

    void startAI();
    void stopAI();
    void updateMovement();

protected:
    void timerEvent(QTimerEvent *event) override;

private slots:
    void makeDecision();
    void moveRandomly();
    // void chasePlayer();
    // void fleeFromPlayer();

private:
    float m_safetyMargin;
    bool isPositionValid(const QPointF& pos) const;
    QPointF m_currentTarget;
    int m_aiTimerId;

    //两个大脑
    QTimer m_decisionTimer;
    QTimer m_movementTimer;

    Character* m_targetPlayer;
    QPointF m_lastKnownPlayerPos;
    bool m_playerInSight;
    qreal m_detectionRadius;

};

#endif // AICHARACTER_H
