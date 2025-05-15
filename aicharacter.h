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

protected:
    void timerEvent(QTimerEvent *event) override;

private slots:
    void makeDecision();
    void moveRandomly();
    // void chasePlayer();
    // void fleeFromPlayer();

private:
    int m_aiTimerId;
    QTimer m_decisionTimer;
    Character* m_targetPlayer;
    QPointF m_lastKnownPlayerPos;
    bool m_playerInSight;
    qreal m_detectionRadius;
};

#endif // AICHARACTER_H
