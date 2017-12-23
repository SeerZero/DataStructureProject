#include "monitor.h"
#include "ui_monitor.h"
#include <QObject>
#include <QDebug>
#include <mainwindow.h>
#include "parkingLotManager.h"
#include "carlist.h"
#include "logwindow.h"

Monitor::Monitor(MainWindow *parent) :
    QWidget(parent),
    ui(new Ui::Monitor)
{

    l->append(FakeCar());
//    l->append(FakeCar());
//    l->append(FakeCar());
//    l->append(FakeCar());
//    l->append(FakeCar());

    adapter = new Adapter(this, l);

    ui->setupUi(this);
    QGraphicsScene *scene = new QGraphicsScene(this);  // 创建场景
    ParkingLotManager *pk = new ParkingLotManager(this, scene);
    QObject::connect(pk, &ParkingLotManager::enableUpButton, ui->upStairButton, &QPushButton::setEnabled);
    QObject::connect(pk, &ParkingLotManager::enableDownButton, ui->downStairButton, &QPushButton::setEnabled);
    QObject::connect(pk, &ParkingLotManager::setCapacity, ui->capacity, &QLabel::setText);
    QObject::connect(pk, &ParkingLotManager::setLoad, ui->load, &QLabel::setText);
    QObject::connect(pk, &ParkingLotManager::setLayerName, ui->layerName, &QLabel::setText);
    QObject::connect(ui->downStairButton, &QPushButton::clicked, pk, &ParkingLotManager::showDownStairFloor);
    QObject::connect(ui->upStairButton, &QPushButton::clicked, pk, &ParkingLotManager::showUpStairFloor);
    QObject::connect(ui->showMargin, &QCheckBox::toggled, pk, &ParkingLotManager::showMargin);
    QObject::connect(ui->showGraph, &QCheckBox::toggled, pk, &ParkingLotManager::showGraph);
    QObject::connect(ui->showLog, &QCheckBox::toggled, parent, &MainWindow::showLogWindow);
    QObject::connect(ui->showList, &QCheckBox::toggled, parent, &MainWindow::showListWindow);
    QObject::connect(ui->drawPath, &QPushButton::clicked, [=](){
        pk->drawPath(ui->placeNum1->text().toInt(), ui->placeNum2->text().toInt());
    });

    QObject::connect(ui->random, &QPushButton::clicked, this, &Monitor::addCar);

    pk->showParkingLot();
    ui->view->setScene(scene);
    CarList::getInstance()->setAdapter(adapter);
}

Monitor::~Monitor()
{
    delete ui;
}

void Monitor::addCar()
{
//    QModelIndex topLeft = adapter->index(0, 0);
//    QModelIndex bottomRight = adapter->index(adapter->rowCount(), 6);
//    emit adapter->dataChanged(topLeft, bottomRight);
    l->append(FakeCar());
}
