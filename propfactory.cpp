#include "propfactory.h"
#include <QtMath>  // 用于三角函数（qCos, qSin）
#include <QRandomGenerator>
int PropFactory::mapRadius = 0;

Prop* PropFactory::createRandomProp(const QPointF& centerPos){
    PropType type = static_cast<PropType>(
        QRandomGenerator::global()->bounded(static_cast<int>(PropType::COUNT))
        );
    //随机生成道具类型
    QPointF pos = randomPositionNearCenter(centerPos);
    //随机生成位置
    Prop* prop = nullptr;
    //初始化道具

    switch(type){
    case PropType::Knife:
        prop = new InteractiveProp(PropType::Knife);
        break;
    default:
        break;
    }
    //创建具体道具

    prop->setPos(pos);
    return prop;
}


QPointF PropFactory::randomPositionNearCenter(const QPointF& center) {
    // 生成距离中心500到mapRadius-500范围内的随机位置
    double distance = 500 + QRandomGenerator::global()->bounded(mapRadius - 1000);
    double angle = QRandomGenerator::global()->bounded(360);

    return QPointF(
        center.x() + distance * qCos(qDegreesToRadians(angle)),
        center.y() + distance * qSin(qDegreesToRadians(angle))
        );
}
