#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pond = new PondView;
    animationAct = new QAction("start", this);
    connect(animationAct, SIGNAL(triggered()), pond, SLOT(startAnimation()));
    ui->mainToolBar->addAction(animationAct);
}

MainWindow::~MainWindow()
{
    delete ui;
}
