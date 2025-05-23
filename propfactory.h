#ifndef PROPFACTORY_H
#define PROPFACTORY_H

#include "character.h"
#include "mygraphicsview.h"
#include <QPainter>
#include <QRandomGenerator>
#include <vector>

class PropFactory{
private:
    static int mapRadius;
    static QPointF randomPositionNearCenter(const QPointF& center);
    static PropType selectByWeight();

public:
    static std::unique_ptr<Prop> createRandomProp(const QPointF& centerPos);
    static void setMapRadius(int radius) { mapRadius = radius; }
};

#endif // PROPFACTORY_H
