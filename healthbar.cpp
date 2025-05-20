#include "healthbar.h"
#include "character.h"
#include <QDebug>

HealthBar::HealthBar(QObject* parent)
    : QObject(parent)
    , QGraphicsPixmapItem(nullptr)
    , m_targetCharacter(nullptr)
    , m_offset(-25, -90)  // 默认在角色上方50像素
    , m_maxHealth(100)
    , m_currentHealth(100)
{
    // 设置初始图片为全满状态
    setVisible(true);
    setZValue(10);
    setPixmap(QPixmap());
    // 设置合适的大小
    setTransformOriginPoint(boundingRect().center());
    setScale(0.2);
    // 设置透明度为不透明
    setOpacity(1);
    qDebug() << "Set size and opacity for HealthBar";
    //禁用碰撞
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setFlag(QGraphicsItem::ItemIsFocusable, false);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
}

HealthBar::~HealthBar()
{
    qDebug() << "HealthBar destructor called"; // 添加调试语句
}

bool HealthBar::loadHealthBarImages(const QString& pathPrefix, const QString& fileExtension, int count)
{
    m_healthBarImages.clear();

    for (int i = 0; i < count; ++i) {
        QString filePath = QString("%1%2.%3").arg(pathPrefix).arg(i-1).arg(fileExtension);
        QPixmap pixmap(filePath);

        m_healthBarImages.append(pixmap);
        qDebug() << "Loaded health bar image:" << filePath; // 添加调试语句
    }

    // 加载成功后设置初始图片
    if (!m_healthBarImages.isEmpty()) {
        setPixmap(m_healthBarImages.last());
        qDebug() << "Set initial pixmap for HealthBar"; // 添加调试语句
    }

    return true;
}

void HealthBar::setTargetCharacter(Character* character)
{
    if (m_targetCharacter) {
        disconnect(m_targetCharacter, &Character::healthChanged, this, &HealthBar::onCharacterHealthChanged);
    }

    m_targetCharacter = character;
    if (m_targetCharacter) {
        m_maxHealth = m_targetCharacter->maxHealth();
        m_currentHealth = m_targetCharacter->health();

        connect(m_targetCharacter, &Character::healthChanged, this, &HealthBar::onCharacterHealthChanged);
        // 初始更新
        qDebug() << "Connected character's healthChanged signal and called updateHealthBar"; // 添加调试语句
    }
}

void HealthBar::updateHealthBar()
{
    if (!m_targetCharacter || m_healthBarImages.isEmpty()) {
        qDebug() << "Either target character is null or health bar images are empty, skipping update"; // 添加调试语句
        return;
    }

    // 计算生命值百分比
    float healthPercent = static_cast<float>(m_currentHealth) / m_maxHealth;
    healthPercent = qBound(0.0f, healthPercent, 1.0f);  // 确保在0-1范围内

    // 计算对应的图片索引 (0-9)
    int imageIndex = qMin(static_cast<int>(healthPercent * m_healthBarImages.size()),
                          m_healthBarImages.size() - 1);

    // 更新图片
    if (imageIndex >= 0 && imageIndex < m_healthBarImages.size()) {
        setPixmap(m_healthBarImages[imageIndex]);
    }

    // 更新位置，跟随角色
    if (m_targetCharacter) {
        setPos(m_targetCharacter->pos() + m_offset);
    }
}

void HealthBar::setOffset(const QPointF& offset)
{
    m_offset = offset;
    updateHealthBar();
    qDebug() << "Set offset and called updateHealthBar"; // 添加调试语句
}


void HealthBar::onCharacterHealthChanged(int health)
{
    m_currentHealth = health;
    updateHealthBar();
    qDebug() << "Character's health changed, called updateHealthBar"; // 添加调试语句
}
