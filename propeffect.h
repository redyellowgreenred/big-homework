#ifndef PROPEFFECT_H
#define PROPEFFECT_H

#include <QObject>
#include <QTimer>
#include <memory>

#include "character.h"
class Prop;

class PropEffect : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(PropEffect)

public:
    explicit PropEffect(QObject* parent = nullptr) : QObject(parent) {}

    virtual void apply(Character* character, std::unique_ptr<Prop>) = 0;
    virtual void remove(Character* character) = 0;
    virtual ~PropEffect() override = default;
};

class KnifeEffect : public PropEffect {
    Q_OBJECT

public:
    explicit KnifeEffect(QObject* parent = nullptr);
    void apply(Character* character, std::unique_ptr<Prop>) override;
    void remove(Character* character) override;

private:
    std::unique_ptr<QTimer> updateTimer;
};

class ShoesEffect : public PropEffect {
    Q_OBJECT

public:

    explicit ShoesEffect(int duration, QObject* parent = nullptr, int speedBoost = 100);
    void apply(Character* character, std::unique_ptr<Prop>) override;
    void remove(Character* character) override;

private:
    bool isActive = false;
    int duration;
    int speedBoost;
    std::unique_ptr<QTimer> updateTimer;

    QGraphicsPixmapItem* m_icon = nullptr;
    QTimer m_positionTimer;               // 位置更新定时器
    void updateIconPosition(Character* character);
};

class TreeEffect : public PropEffect {
    Q_OBJECT

public:
    explicit TreeEffect(QObject* parent = nullptr);
    void apply(Character* character, std::unique_ptr<Prop>) override;
    void remove(Character* character) override;
};

#endif // PROPEFFECT_H


class HpEffect : public PropEffect {
    Q_OBJECT

public:
    explicit HpEffect(QObject* parent = nullptr);
    void apply(Character* character, std::unique_ptr<Prop>) override;
    void remove(Character* character) override;
};
