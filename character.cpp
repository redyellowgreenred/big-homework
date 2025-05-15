#include "character.h"
#include "propeffect.h"
#include <QDebug>

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

    moveAnimation = new QPropertyAnimation(this, "pos", this);

    // 构建映射
    m_propEffects[PropType::Knife] = std::make_unique<KnifeEffect>();
    m_propEffects[PropType::Shoes] = std::make_unique<ShoesEffect>(4400);
    m_propEffects[PropType::Hp] = std::make_unique<HpEffect>();

    loadDeadImage(":/figs/dead.png");
}

Character::~Character() {
}


int Character::health() const{
    return p_hp;
}

int Character::maxHealth() const
{
    return p_maxhp;
}

int Character::originalSpeed() const
{
    return p_originalSpeed;
}

int Character::moveSpeed() const
{
    return p_moveSpeed;
}

CharacterState Character::state() const
{
    return p_state;
}

bool Character::isAlive() const
{
    return p_hp > 0;
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

void Character::setPos(const QPointF &pos)
{
    QGraphicsPixmapItem::setPos(pos);
    emit positionChanged(pos);
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

void Character::updateKnivesPosition() {
    QPointF charPos = pos();
    int count = m_knives.size();

    if (count == 0) return; // No knives to update

    // Increment the base angle for rotation
    static double baseAngle = 0;
    baseAngle += 2.0;
    if (baseAngle >= 360.0) baseAngle -= 360.0;  // Keep baseAngle within 0-360 degrees

    double angleIncrement = 360.0 / count;  // Fixed angle increment for each knife

    // Pre-calculate the sine and cosine values for rotation
    for (int i = 0; i < count; ++i) {
        Prop* knife = m_knives[i].get();
        double angle = baseAngle + (i * angleIncrement);
        double radian = qDegreesToRadians(angle);

        // Calculate new position for the knife
        double x = charPos.x() + m_knifeDistance * qCos(radian) + 30.0;
        double y = charPos.y() + m_knifeDistance * qSin(radian) + 30.0;

        // Only update if position has changed (optional optimization)
        if (knife->pos() != QPointF(x, y)) {
            knife->setPos(x, y);
            knife->setRotation(angle + 180 + 90);  // Update rotation based on new position
        }
    }
}

void Character::setMyHealthbar(HealthBar* bar)
{
    p_healthBar = bar;
    if (p_healthBar) {
        p_healthBar->setTargetCharacter(this);
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
