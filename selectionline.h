#ifndef SELECTIONLINE_H
#define SELECTIONLINE_H

#pragma once

#include <QGraphicsLineItem>
#include <QPropertyAnimation>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QEasingCurve>
class Character;

class SelectionLine : public QObject, public QGraphicsLineItem {

public:
    explicit SelectionLine(QGraphicsItem* parent = nullptr);
    ~SelectionLine();

    void setEndPoints(const QPointF& start, const QPointF& end);
    void setPlayer(Character* character){player = character;}
    void stopAnimation();
    void throwAKnife();
protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    QPropertyAnimation* m_animation;
    QPen m_pen;
    bool m_isAnimating = false;
    Character* player;
    Character* nearestAI;

public slots:
    void shoot();
    void animate();
};

#endif // SELECTIONLINE_H
