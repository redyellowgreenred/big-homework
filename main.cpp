#include <QApplication>
#include "mainwindow.h"
#include "mygraphicsview.h"
#include <QEventLoop>

int main(int argc, char *argv[]) {
    qSetMessagePattern("%{file}(%{line}): %{message}");
    QApplication app(argc, argv);
    bool restartGame = true;

    while (restartGame) {
        // 创建新场景和视图
        QGraphicsScene* scene = new QGraphicsScene();
        MyGraphicsView* view = new MyGraphicsView(scene);
        view->resize(1080, 675);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        // 创建开始界面
        MainWindow* mainWindow = new MainWindow();

        // 连接开始游戏信号
        QObject::connect(mainWindow, &MainWindow::evokeGameSignal, view,
                         &MyGraphicsView::handleEvokeGameSignal);

        // 连接游戏结束信号（关键修改：使用局部事件循环）
        QEventLoop loop; // 创建局部事件循环
        QObject::connect(view, &MyGraphicsView::gameOver, [&](bool restart) {
            restartGame = restart;
            loop.quit(); // 退出局部事件循环
        });
        QObject::connect(mainWindow, &MainWindow::close, [&]() {
            loop.quit(); // 主窗口关闭时也退出循环
        });

        // 显示开始界面并启动局部事件循环
        mainWindow->show();
        loop.exec(); // 阻塞在此，直到 gameOver 或窗口关闭

        // 清理资源（确保在事件循环退出后执行）
        delete view;
        delete scene;
        delete mainWindow;

        qDebug() << "游戏重启中...";
    }

    return 0;
}
