#include "mygraphicsview.h"
#include "propfactory.h"
#include "player.h"
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QMovie>
#include <QPainter>
#include <QTimer>
#include <QMessageBox>

const int bgSize = 3600;
// 槽函数: 处理信号, 展示游戏窗口
void MyGraphicsView::handleEvokeGameSignal() {
    this->show();
}

// 槽函数: 更新运动圆形的路径（这里未涉及角色移动，保持不变）
void MyGraphicsView::updateCirclePath() {
    // 沿正方形轨迹移动
    // 这里用的是什么坐标系?
    const int speed = 3;
    QPointF localPos = m_moving_circle->pos() - initPoint;

    switch (currentEdge) {
    case 0:  // 上边向右移动
        localPos.rx() += speed;
        if (localPos.x() > SQUARE_SIZE / 2) {
            localPos.rx() = SQUARE_SIZE / 2;
            currentEdge = 1;
        }
        break;
    case 1:  // 右边向下移动
        localPos.ry() += speed;
        if (localPos.y() > SQUARE_SIZE / 2) {
            localPos.ry() = SQUARE_SIZE / 2;
            currentEdge = 2;
        }
        break;
    case 2:  // 下边向左移动
        localPos.rx() -= speed;
        if (localPos.x() < -SQUARE_SIZE / 2) {
            localPos.rx() = -SQUARE_SIZE / 2;
            currentEdge = 3;
        }
        break;
    case 3:  // 左边向上移动
        localPos.ry() -= speed;
        if (localPos.y() < -SQUARE_SIZE / 2) {
            localPos.ry() = -SQUARE_SIZE / 2;
            currentEdge = 0;
        }
        break;
    }
    m_moving_circle->setPos(localPos + initPoint);
}

MyGraphicsView::MyGraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent) {
    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle("Moving demo");

    // 加载并处理圆形背景
    // ":/" 表示从 qrc 加载资源
    QPixmap backgroundPixmap(":/figs/background3.png");
    if (backgroundPixmap.isNull()) {
        qDebug() << "Failed to load background image!";
    } else {
        qDebug() << "Background image loaded successfully!";
    }

    const int bgSize = 3600;  // 圆形背景的直径
    QPixmap scaledBg =
        backgroundPixmap.scaled(bgSize, bgSize, Qt::KeepAspectRatioByExpanding,
                                Qt::SmoothTransformation);

    QPixmap circularBg(bgSize, bgSize);
    circularBg.fill(Qt::transparent);
    QPainter painter(&circularBg);
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::SmoothPixmapTransform);
    painter.setClipRegion(QRegion(0, 0, bgSize, bgSize, QRegion::Ellipse));
    painter.drawPixmap((bgSize - scaledBg.width()) / 2,
                       (bgSize - scaledBg.height()) / 2, scaledBg);

    m_background = scene->addPixmap(circularBg);
    m_background->setPos(0, 0);
    scene->setSceneRect(0, 0, bgSize, bgSize);

    // 创建运动圆形
    m_moving_circle = new QGraphicsEllipseItem(-15, -15, 30, 30, m_background);
    m_moving_circle->setBrush(Qt::red);
    // 将圆形设置在背景的中心
    QPointF bgCenter = m_background->boundingRect().center();
    m_moving_circle->setPos(bgCenter);  // 初始位置
    initPoint = bgCenter;

    // 加载人物 GIF
    m_player = std::make_unique<Player>(bgSize / 2, m_background);
    QPointF playerPos = bgCenter;
    m_player->setPos(playerPos);
    m_player->setMaxHealth(100);
    m_player->setHealth(100);
    m_player->loadAnimation(":/figs/capoo.gif");

    //加载索敌线
    m_line = std::make_unique<SelectionLine>();
    m_line->setPlayer(m_player.get());
    scene->addItem(m_line.get());
    connect(&m_lineUpdateTimer, &QTimer::timeout, m_line.get(), &SelectionLine::animate);
    m_lineUpdateTimer.start(10);

    qreal scaleFactor = 0.3;
    m_player->setScale(scaleFactor);

    connect(m_player.get(), &Player::positionChanged, this, [this]() {
        centerOn(m_player.get()); // 跟随角色移动
    });

    std::unique_ptr m_healthBar = std::make_unique<HealthBar>();
    m_healthBar->loadHealthBarImages(":/props/healthbar/", "png", 11);
    m_healthBar->setTargetCharacter(m_player.get());

    //角色死亡事件绑定
    connect(m_player.get(), &Player::died, this, &MyGraphicsView::onPlayerDeath);
    // 路径运动定时器（每 30ms 更新位置）
    connect(&m_pathTimer, &QTimer::timeout, this,
            &MyGraphicsView::updateCirclePath);
    connect(&m_pathTimer, &QTimer::timeout, this,
            &MyGraphicsView::checkCollision1);
    connect(&m_pathTimer2, &QTimer::timeout, this,
            &MyGraphicsView::checkCollision2);
    QMetaObject::Connection conn = connect(&m_healthbarTimer, &QTimer::timeout, m_healthBar.get(), &HealthBar::updateHealthBar);
    if (conn) {
        qDebug() << "Signal and slot connected successfully";
    } else {
        qDebug() << "Failed to connect signal and slot";
    }
    m_pathTimer.start(40);
    m_pathTimer2.start(400);
    m_healthbarTimer.start(60);

    scene->addItem(m_healthBar.get());  // 必须添加到场景！
    healthBars.push_back(std::move(m_healthBar));

    // 居中显示人物
    centerOn(m_player.get());

    //初始化生成道具
    generateProps(100);

    //生成ai
    const int aiCount = 5;
    for (int i = 0; i < aiCount; i++){
        auto ai = std::make_unique<AICharacter>(bgSize / 2, m_background);
        ai->setScale(0.3);
        ai->setPos(QPointF(
            QRandomGenerator::global()->bounded(bgSize/4, bgSize*3/4),
            QRandomGenerator::global()->bounded(bgSize/4, bgSize*3/4)
            ));
        ai->startAI(); // 启动AI行为
        scene->addItem(ai.get());
        //给ai血条
        std::unique_ptr m_healthBar = std::make_unique<HealthBar>();
        m_healthBar->loadHealthBarImages(":/props/healthbar/", "png", 11);
        m_healthBar->setTargetCharacter(ai.get());
        scene->addItem(m_healthBar.get());
        m_healthBar->setZValue(10);
        QMetaObject::Connection conn = connect(&m_healthbarTimer, &QTimer::timeout, m_healthBar.get(), &HealthBar::updateHealthBar);
        m_aiCharacters.push_back(std::move(ai));
        healthBars.push_back(std::move(m_healthBar));
    }
}

void MyGraphicsView::keyPressEvent(QKeyEvent *event) {
    m_player->addPressedKey(event->key());
    QGraphicsView::keyPressEvent(event);
}

void MyGraphicsView::keyReleaseEvent(QKeyEvent *event) {
    m_player->removePressedKey(event->key());
    QGraphicsView::keyReleaseEvent(event);
}

//初始化道具生成
void MyGraphicsView::generateProps(int count) {
    QPointF center = m_background->boundingRect().center();
    PropFactory::setMapRadius(bgSize / 2);

    for (int i = 0; i < count; ++i) {
        std::unique_ptr<Prop> prop = PropFactory::createRandomProp(center);
        prop->setZValue(10);
        scene()->addItem(prop.release());
    }
}

template void Prop::interact<Character>(Character*);
//碰撞检测函数
void MyGraphicsView::checkCollision1() {
    // 1. 检测玩家与道具的碰撞
    checkPropCollision(m_player.get());

    // 2. 检测每个AI与道具的碰撞
    for (auto& ai : m_aiCharacters) {
        if (ai && ai->isAlive()) {
            checkPropCollision(ai.get());
        }
    }
}

void MyGraphicsView::checkCollision2() {
    for (size_t i = 0; i < m_aiCharacters.size(); ++i){
        checkCharacterCollision(m_player.get(), m_aiCharacters[i].get());
    }

    // 检测 AI 和 AI 之间的碰撞
    for (size_t i = 0; i < m_aiCharacters.size(); ++i) {
        for (size_t j = i + 1; j < m_aiCharacters.size(); ++j) {
            checkCharacterCollision(m_aiCharacters[i].get(), m_aiCharacters[j].get());
        }
    }
}

void MyGraphicsView::checkCharacterCollision(Character* character1, Character* character2){
    if (!character1 || !character1->isAlive() || !character2 || !character2->isAlive()) return;

    QPointF pos1 = character1->pos();
    QPointF pos2 = character2->pos();

    qreal distance = QLineF(pos1, pos2).length();
    const qreal collisionRadius = (character1->getDistance() + character2->getDistance()) * 3 + 100; // 碰撞半径

    if (distance < collisionRadius) {
        // 处理碰撞逻辑
        if (character1->knifeCount() && character2->knifeCount()){
            character1->deleteKnife();
            character2->deleteKnife();
            qDebug() << "Collision detected between" << static_cast<QObject*>(character1) << "and" << static_cast<QObject*>(character2);
        }
        else if (character1->knifeCount() && !character2->knifeCount()){
            character2->setHealth(character2->health()-10);
        }
        else if (!character1->knifeCount() && character2->knifeCount()){
            character1->setHealth(character1->health()-10);
        }
    }
}

void MyGraphicsView::checkPropCollision(Character* character) {
    if (!character || !character->isAlive()) return;

    const qreal pickupRadius = 50.0; // 拾取半径
    QPointF charPos = character->pos();

    // 检测区域
    QRectF detectArea(
        charPos.x() - pickupRadius,
        charPos.y() - pickupRadius,
        pickupRadius * 2,
        pickupRadius * 2
        );

    QList<QGraphicsItem*> items = scene()->items(detectArea);

    for (QGraphicsItem* item : items) {
        // 排除玩家角色本身
        if (item == character || item->type() == HealthBar::Type) {
            continue;
        }

        // 检查是否为道具
        Prop* prop = qgraphicsitem_cast<Prop*>(item);
        if (prop && !prop->isPicked()) {
            qreal distance = QLineF(charPos, prop->pos()).length();
            if (distance < pickupRadius) {
                prop->interact(character);
            }
        }
    }
}

void MyGraphicsView::onPlayerDeath() {
    m_pathTimer.stop();
    m_player->setEnabled(false);

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "游戏结束", "你已死亡！是否重新开始？",
        QMessageBox::Yes | QMessageBox::No
        );

    emit gameOver(reply == QMessageBox::Yes); // 发出游戏结束信号
}
