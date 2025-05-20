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

    void setEndPoints(const QPointF& start, const QPointF& end);
    void setPlayer(Character* character){player = character;}
    void animate();
    void stopAnimation();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    QPropertyAnimation* m_animation;
    QPen m_pen;
    bool m_isAnimating = false;
    Character* player;
};
#endif // SELECTIONLINE_H
