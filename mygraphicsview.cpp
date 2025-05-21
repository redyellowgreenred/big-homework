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
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGraphicsProxyWidget>


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
        m_line = std::make_unique<SelectionLine>(m_player.get(), nullptr, nullptr);
    m_line->setPlayer(m_player.get());
    m_line->setmPlayer(m_player.get());
    scene->addItem(m_line.get());
    // if (m_line) {
    //     connect(&m_lineUpdateTimer, &QTimer::timeout, this, [this]() {
    //         if (m_line && m_line->scene()) {
    //             m_line->animate();
    //         }
    //     });
    //     m_lineUpdateTimer.start(80);
    // } else {
    //     qDebug() << "MyGraphicsView: m_line is not initialized!";
    // }

    // connect(m_player.get(), &Player::attack, m_line.get(), &SelectionLine::shoot);
    lines.push_back(std::move(m_line));

    connect(m_player.get(), &Player::attack, lines[0].get(), &SelectionLine::shoot);

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
    connect(&m_healthbarTimer, &QTimer::timeout, this,
            &MyGraphicsView::checkCollision1);
    connect(&m_pathTimer2, &QTimer::timeout, this,
            &MyGraphicsView::checkCollision2);
    connect(&m_pathTimer3, &QTimer::timeout, this,
            [this](){
                MyGraphicsView::generateProps(5);
    });
    QMetaObject::Connection conn = connect(&m_healthbarTimer, &QTimer::timeout, m_healthBar.get(), &HealthBar::updateHealthBar);
    if (conn) {
        qDebug() << "Signal and slot connected successfully";
    } else {
        qDebug() << "Failed to connect signal and slot";
    }
    m_pathTimer.start(50);
    m_pathTimer2.start(400);
    m_pathTimer3.start(3000);
    m_healthbarTimer.start(100);

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
        if (!scene) {
            qDebug() << "[ERROR] Scene is null!";
            return;
        }
        scene->addItem(ai.get());
        //给ai血条
        std::unique_ptr m_healthBar = std::make_unique<HealthBar>();
        m_healthBar->loadHealthBarImages(":/props/healthbar/", "png", 11);
        m_healthBar->setTargetCharacter(ai.get());
        scene->addItem(m_healthBar.get());
        m_healthBar->setZValue(10);
        QMetaObject::Connection conn = connect(&m_healthbarTimer, &QTimer::timeout, m_healthBar.get(), &HealthBar::updateHealthBar);

        // 初始化 AI 的索敌线
        auto line = std::make_unique<SelectionLine>(ai.get(), m_player.get(), nullptr);
        line->setmPlayer(m_player.get());
        scene->addItem(line.get());
        lines.push_back(std::move(line));

        m_aiCharacters.push_back(std::move(ai));
        healthBars.push_back(std::move(m_healthBar));
    }
    connect(&m_lineUpdateTimer, &QTimer::timeout, this, [this]() {
        for (auto& lined d : lines) {
            if (line && line->scene()) {
                line->animate();
            }
        }
    });
    m_lineUpdateTimer.start(80);

    connect(this, &MyGraphicsView::victory, this, &MyGraphicsView::onVictory);
}

MyGraphicsView::~MyGraphicsView() {
    // 停止所有定时器
    m_pathTimer.stop();
    m_pathTimer2.stop();
    m_pathTimer3.stop();
    m_healthbarTimer.stop();
    m_lineUpdateTimer.stop();

    // 确保所有异步操作完成
    QCoreApplication::processEvents();

    // 清理AI角色
    for (auto& ai : m_aiCharacters) {
        if (ai && ai->scene()) {
            ai->scene()->removeItem(ai.get());
        }
    }
    m_aiCharacters.clear();

    // 清理血条
    for (auto& bar : healthBars) {
        if (bar && bar->scene()) {
            bar->scene()->removeItem(bar.get());
        }
    }
    healthBars.clear();
}

void MyGraphicsView::keyPressEvent(QKeyEvent *event) {
    // 添加输入法事件检查
    if (event->type() == QEvent::KeyPress && !event->text().isEmpty()) {
        if (event->modifiers() & Qt::ControlModifier ||
            event->modifiers() & Qt::AltModifier) {
            QGraphicsView::keyPressEvent(event); // 交给父类处理输入法组合键
            return;
        }
    }

    m_player->addPressedKey(event->key());
    QGraphicsView::keyPressEvent(event); // 确保父类也能处理
}

void MyGraphicsView::keyReleaseEvent(QKeyEvent *event) {
    m_player->removePressedKey(event->key());
    QGraphicsView::keyReleaseEvent(event);
}

//初始化道具生成
void MyGraphicsView::generateProps(int count) {
    if (!scene()) {
        qWarning() << "Scene is null in generateProps";
        return;
    }
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
    QMutexLocker locker(&m_collisionMutex);

    if (m_player && m_player->isAlive()) {
        checkPropCollision(m_player.get());
    }

    for (auto& ai : m_aiCharacters) {
        if (ai && ai->isAlive()) {
            checkPropCollision(ai.get());
        }
    }
}

void MyGraphicsView::checkCollision2() {
    QMutexLocker locker(&m_collisionMutex);

    if (m_player && m_player->isAlive()) {
        for (auto& ai : m_aiCharacters) {
            if (ai && ai->isAlive()) {
                checkCharacterCollision(m_player.get(), ai.get());
            }
        }
    }

    // AI之间的碰撞检测
    for (size_t i = 0; i < m_aiCharacters.size(); ++i) {
        if (!m_aiCharacters[i] || !m_aiCharacters[i]->isAlive()) continue;

        for (size_t j = i + 1; j < m_aiCharacters.size(); ++j) {
            if (m_aiCharacters[j] && m_aiCharacters[j]->isAlive()) {
                checkCharacterCollision(m_aiCharacters[i].get(), m_aiCharacters[j].get());
            }
        }
    }
    checkAllAIDead();
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
    if (!character || !character->isAlive() || !scene() || !m_background) {
        return;
    }
    const qreal pickupRadius = 50.0; // 拾取半径
    QPointF charPos = character->pos();

    if (qIsNaN(charPos.x()) || qIsNaN(charPos.y())) {
        qWarning() << "Invalid character position";
        return;
    }

    QRectF sceneRect = scene()->sceneRect();
    QRectF detectArea(
        qBound(sceneRect.left(), charPos.x() - pickupRadius, sceneRect.right()),
        qBound(sceneRect.top(), charPos.y() - pickupRadius, sceneRect.bottom()),
        pickupRadius * 2,
        pickupRadius * 2
        );


    QList<QGraphicsItem*> items;
    try {
        items = scene()->items(detectArea);
    } catch (...) {
        qCritical() << "Exception when getting scene items";
        return;
    }


    for (QGraphicsItem* item : items) {
        if (!item || item == character || item->type() == HealthBar::Type) {
            continue;
        }

        Prop* prop = qgraphicsitem_cast<Prop*>(item);
        if (prop && prop->scene()) {
            // 检查prop是否已被标记删除
            if (prop->isPicked() || !prop->scene()) {
                continue;
            }

            qreal distance = QLineF(charPos, prop->pos()).length();
            if (distance < pickupRadius) {
                try {
                    prop->interact(character);
                } catch (...) {
                    qCritical() << "Exception in prop interaction";
                }
            }
        }
    }

}

void MyGraphicsView::onPlayerDeath() {
    m_pathTimer.stop();
    m_player->setEnabled(false);
    m_pathTimer.stop();

    // 创建胜利图片
    QPixmap victoryPixmap(":/figs/lose.png");
    victoryPixmap = victoryPixmap.scaled(200, 200, Qt::KeepAspectRatio); // 调整图片大小
    QLabel* victoryLabel = new QLabel(this);
    victoryLabel->setPixmap(victoryPixmap);
    victoryLabel->setAlignment(Qt::AlignCenter);

    // 创建按钮布局
    QPushButton* restartBtn = new QPushButton("重新开始");
    QPushButton* exitBtn = new QPushButton("退出游戏");

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(restartBtn);
    buttonLayout->addWidget(exitBtn);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(victoryLabel);
    mainLayout->addLayout(buttonLayout);
    mainLayout->setSpacing(10); // 设置布局间距

    // 创建对话框
    QDialog* dialog = new QDialog(this);
    dialog->setLayout(mainLayout);
    dialog->setWindowTitle("游戏胜利");
    dialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    dialog->resize(300, 250); // 设置对话框大小

    int rank = 1;

    for (auto& ai : m_aiCharacters) {
        if (ai && ai->isAlive()) {
            rank++;
        }
    }

    setPlayerRank(rank);

    if (m_playerRank != -1) {
        QLabel* rankLabel = new QLabel(QString("你的排名: %1").arg(m_playerRank), this);
        rankLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(rankLabel);
    }

    // 连接按钮信号
    connect(restartBtn, &QPushButton::clicked, [this, dialog]() {
        dialog->close();
        emit gameOver(true);
    });

    connect(exitBtn, &QPushButton::clicked, [this, dialog]() {
        dialog->close();
        emit gameOver(false);
    });

    // 显示对话框
    dialog->exec();
}

void MyGraphicsView::onVictory() {
    m_pathTimer.stop();

    // 创建胜利图片
    QPixmap victoryPixmap(":/figs/victory.png");
    victoryPixmap = victoryPixmap.scaled(200, 200, Qt::KeepAspectRatio); // 调整图片大小
    QLabel* victoryLabel = new QLabel(this);
    victoryLabel->setPixmap(victoryPixmap);
    victoryLabel->setAlignment(Qt::AlignCenter);

    // 创建按钮布局
    QPushButton* restartBtn = new QPushButton("重新开始");
    QPushButton* exitBtn = new QPushButton("退出游戏");

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(restartBtn);
    buttonLayout->addWidget(exitBtn);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(victoryLabel);
    mainLayout->addLayout(buttonLayout);
    mainLayout->setSpacing(10); // 设置布局间距

    // 创建对话框
    QDialog* dialog = new QDialog(this);
    dialog->setLayout(mainLayout);
    dialog->setWindowTitle("游戏胜利");
    dialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    dialog->resize(300, 250); // 设置对话框大小

    setPlayerRank(1);
    if (m_playerRank != -1) {
        QLabel* rankLabel = new QLabel(QString("你的排名: %1").arg(m_playerRank), this);
        rankLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(rankLabel);
    }

    // 连接按钮信号
    connect(restartBtn, &QPushButton::clicked, [this, dialog]() {
        dialog->close();
        emit gameOver(true);
    });

    connect(exitBtn, &QPushButton::clicked, [this, dialog]() {
        dialog->close();
        emit gameOver(false);
    });

    // 显示对话框
    dialog->exec();
}

void MyGraphicsView::checkAllAIDead() {
    bool allDead = true;
    for (auto& ai : m_aiCharacters) {
        if (ai && ai->isAlive()) {
            allDead = false;
            break;
        }
    }

    if (allDead) {
        emit victory();  // 触发胜利信号
    }
}
