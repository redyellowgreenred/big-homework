#include "propfactory.h"
#include <QtMath>  // 用于三角函数（qCos, qSin）
#include <QRandomGenerator>
#include <vector>
#include <random>
#include <numeric>
int PropFactory::mapRadius = 0;

PropType PropFactory::selectByWeight() {
    // 定义道具权重表
    static const std::vector<std::pair<PropType, int>> propWeights = {
        {PropType::Knife, 80},
        {PropType::Hp, 5},
        {PropType::Shoes, 10},
        {PropType::Tree, 5}
    };

    // 计算总权重
    int totalWeight = 0;
    for (const auto& [type, weight] : propWeights){
        totalWeight += weight;
    }

    // 生成随机数
    int random = QRandomGenerator::global()->bounded(totalWeight);

    // 轮盘赌选择
    int cumulative = 0;
    for (const auto& [type, weight] : propWeights) {
        cumulative += weight;
        if (random < cumulative) {
            return type;
        }
    }

    return PropType::Knife; // 默认返回
}

std::unique_ptr<Prop> PropFactory::createRandomProp(const QPointF& centerPos){
    PropType type = selectByWeight();
    //随机生成道具类型
    QPointF pos = randomPositionNearCenter(centerPos);
    //随机生成位置
    std::unique_ptr<Prop> prop = nullptr;
    //初始化道具
    prop = std::make_unique<Prop>(type);
    // switch(type){
    // case PropType::Knife:
    //     prop = std::make_unique<Prop>(type);
    //     break;
    // case PropType::Hp:
    //     prop = std::make_unique<Prop>(type);
    //     break;
    // case PropType::Shoes:
    //     prop = std::make_unique<Prop>(type);
    //     break;
    // default:
    //     break;
    // }
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
