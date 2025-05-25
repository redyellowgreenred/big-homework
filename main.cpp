#include <QApplication>
#include "mainwindow.h"
#include "mygraphicsview.h"
#include <QEventLoop>

int main(int argc, char *argv[]) {
    qSetMessagePattern("%{file}(%{line}): %{message}");
    QApplication app(argc, argv);
    bool restartGame = true;

    while (restartGame) {
        QGraphicsScene* scene = new QGraphicsScene();
        MyGraphicsView* view = new MyGraphicsView(scene);
        view->resize(1080, 675);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        MainWindow* mainWindow = new MainWindow();

        QObject::connect(mainWindow, &MainWindow::evokeGameSignal, view, &MyGraphicsView::handleEvokeGameSignal);

        QEventLoop loop;
        QObject::connect(view, &MyGraphicsView::gameOver, [&](bool restart) {
            restartGame = restart;
            loop.quit();
        });
        QObject::connect(mainWindow, &MainWindow::close, [&]() {
            loop.quit();
        });

        mainWindow->show();
        loop.exec();

        delete view;
        delete scene;
        delete mainWindow;
    }

    qDebug() << "游戏退出中...";
    return 0;
}
