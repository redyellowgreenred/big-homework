#include "character.h"
#include "propeffect.h"
#include <QDebug>
#include <QPainter>

Character::Character(int mapRadius, QGraphicsItem* parent)
    : QObject(nullptr), QGraphicsPixmapItem(parent),
    p_hp(100), p_maxhp(100),p_originalSpeed(200), p_moveSpeed(200),mapRadius(mapRadius),
    p_state(CharacterState::Idle),
    isMoving(false),
    p_healthBar(new HealthBar(this)),
    q_movie(nullptr),
    isAnimationLoaded(false),m_isDead(false)
{

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    // moveAnimation = new QPropertyAnimation(this, "pos", this);

    // 构建映射
    m_propEffects[PropType::Knife] = std::make_unique<KnifeEffect>();
    m_propEffects[PropType::Shoes] = std::make_unique<ShoesEffect>(4000);
    m_propEffects[PropType::Hp] = std::make_unique<HpEffect>();

    loadDeadImage(":/figs/dead.png");

    QTimer::singleShot(0, this, [this]() {
        if (!scene()) {
            qDebug() << "[ERROR] Scene is null, cannot add knives!";
            return;
        }
        for (int i = 0; i < 4; ++i) {
            auto knife = std::make_unique<Prop>(PropType::Knife);
            knife->setZValue(10);
            knife->pick();
            scene()->addItem(knife.get());
            addProp(std::move(knife));
            qDebug() << "Knife" << i << "added to scene at pos:" << m_knives.back()->pos();
        }
        updateKnivesPosition(); // 立即更新位置
    });


}

Character::~Character() {
}


void Character::setHealth(int health)
{
    if (p_hp == health) return;

    p_hp = health;
    emit healthChanged(p_hp);

    if (p_hp <= 0) {
        p_hp = 0;
        die();
    }
}

void Character::setMaxHealth(int maxHealth)
{
    p_maxhp = maxHealth;
    if (p_hp > p_maxhp) {
        p_hp = p_maxhp;
        emit healthChanged(p_hp);
    }
}

void Character::setMoveSpeed(int speed)
{
    p_moveSpeed = speed;
}

void Character::setState(CharacterState state) {
    if (p_state == state) return;

    p_state = state;
    emit stateChanged(p_state);

}

void Character::moveTo(const QPointF& pos)
{
    if (!isAlive() || p_state == CharacterState::Dead) return;

    targetPos = pos;
    isMoving = true;

    // 根据距离和移动速度计算动画持续时间
    qreal distance = QLineF(this->pos(), pos).length();
    int duration = static_cast<int>(distance / p_moveSpeed * 1000); // 毫秒

    moveAnimation->stop();
    moveAnimation->setStartValue(this->pos());
    moveAnimation->setEndValue(pos);
    moveAnimation->setDuration(duration);
    moveAnimation->start();

    if (p_state != CharacterState::Attacking && p_state != CharacterState::Hurt) {
        setState(CharacterState::Moving);
    }

    updateKnivesPosition();
}

void Character::stopMoving()
{
    if (isMoving) {
        moveAnimation->stop();
        isMoving = false;
        if (p_state == CharacterState::Moving) {
            setState(CharacterState::Idle);
        }
    }
}

void Character::addProp(std::unique_ptr<Prop> prop){
    PropType proptype = prop->getType();
    auto it = m_propEffects.find(proptype);
    if (it != m_propEffects.end() && prop->getType() != PropType::Tree){
        it->second->apply(this, std::move(prop));
    }
}

void Character::deleteKnife(){
    if (m_knives.size()){
        m_knives.pop_back();
    }
}

void Character::updateKnivesPosition() {
    QPointF charPos = pos();
    int count = knifeCount();

    if (count == 0) return; // No knives to update

    // Increment the base angle for rotation
    m_baseAngle += 2.0;
    if (m_baseAngle >= 360.0) m_baseAngle -= 360.0;  // Keep baseAngle within 0-360 degrees

    double angleIncrement = 360.0 / count;  // Fixed angle increment for each knife

    // Pre-calculate the sine and cosine values for rotation
    for (int i = 0; i < count; ++i) {
        Prop* knife = m_knives[i].get();
        double angle = m_baseAngle + (i * angleIncrement);
        double radian = qDegreesToRadians(angle);

        // Calculate new position for the knife
        double x = charPos.x() + qCos(radian) * getDistance() + 30;
        double y = charPos.y() + qSin(radian) * getDistance() + 30;

        // Only update if position has changed (optional optimization)
        if (knife->pos() != QPointF(x, y)) {
            knife->setPos(x, y);
            knife->setRotation(angle + 180 + 90);  // Update rotation based on new position
        }
    }
}

void Character::loadAnimation(const QString& gifPath) {
    if (q_movie) {
        q_movie->stop();
        delete q_movie;
    }

    q_movie = new QMovie(gifPath, QByteArray(), this);
    if (q_movie->isValid()) {
        q_movie->setCacheMode(QMovie::CacheAll);
        q_movie->start();
        setPixmap(q_movie->currentPixmap());
        setOffset(-q_movie->currentPixmap().width()/4,
                  -q_movie->currentPixmap().height()/4);
        isAnimationLoaded = true;
    } else {
        qDebug() << "Failed to load animation:" << gifPath;
        QPixmap defaultPixmap(16, 16);
        QPainter painter(&defaultPixmap);
        painter.fillRect(defaultPixmap.rect(), Qt::red);
        setPixmap(defaultPixmap);
    }
}

void Character::loadDeadImage(const QString& imagePath) {
    m_deadPixmap = QPixmap(imagePath);
    if (m_deadPixmap.isNull()) {
        qDebug() << "Failed to load dead image:" << imagePath;
    } else {
        // 调整图片锚点（可选，根据图片尺寸设置）
        setOffset(-m_deadPixmap.width()/2, -m_deadPixmap.height()/2);
    }
}
void Character::die() {
    if (m_isDead) return; // 防止重复调用
    m_isDead = true;

    // 停止动画
    if (q_movie) {
        q_movie->stop();
        delete q_movie;
        q_movie = nullptr;
    }
    stopMoving();
    p_state = CharacterState::Dead;

    // 显示死亡图片
    if (!m_deadPixmap.isNull()) {
        setPixmap(m_deadPixmap);
    } else {
        //  fallback：显示红色方块（调试用）
        QPixmap defaultPixmap(32, 32);
        defaultPixmap.fill(Qt::red);
        setPixmap(defaultPixmap);
    }

    // 触发死亡信号
    emit died();

    setState(CharacterState::Dead);
}
