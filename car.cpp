﻿#include "car.h"
#include "logwindow.h"
#include <QTimer>


Car::Car(ParkingLotManager * manager, QGraphicsItem *parent, int dir, Car::Color color) :
    QObject(0),
    QGraphicsPixmapItem(parent),
    posAni(new QPropertyAnimation(this,"m_pos")),
    m_target(PathPoint(this->pos(),this->rotation())),
    m_current(PathPoint(this->pos(),this->rotation()))
{
    m_manager = manager;
    m_pos = this->pos();
    this->setRotation(dir);
    if(color==RANDOM)
        color = Car::Color(rand()%3);  //车身颜色一定是确定的
    m_color = color;
    switch (m_color) {
    case Pink:
        m_pic = QPixmap(":/cars/pink");
        break;
    case Red:
        m_pic = QPixmap(":/cars/red");
        break;
    case Yellow:
        m_pic = QPixmap(":/cars/yellow");
        break;
    default:
        m_pic = QPixmap(":/cars/red");
        break;
    }
    m_pic = m_pic.scaled(M_WID, M_LEN);
    this->setPixmap(m_pic);
    setTransformOriginPoint(0, M_LEN/2);  //设置旋转中心
    connect(posAni,QPropertyAnimation::finished,this,followPath);
    connect(this, &Car::queueHead, manager, &ParkingLotManager::requestIn);
    connect(this, &Car::exit, manager, &ParkingLotManager::leave);
    connect(this, &Car::stair, manager, &ParkingLotManager::requestStair);

    m_number = "1234";

    // 测试:创建10s后离开
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this, manager](){emit manager->requestOut(this);});
    timer->setSingleShot(true);
    timer->start(10000);
}

void Car::Forward(qreal vel)
{
    qreal ang = qDegreesToRadians(rotation());
    this->moveBy(+vel*qSin(ang), -vel*qCos(ang));
}

void Car::Backward(qreal vel)
{
    qreal ang = qDegreesToRadians(rotation());
    this->moveBy(-vel*qSin(ang), +vel*qCos(ang));
}

void Car::moveLeft(qreal vel)
{
    qreal ang = qDegreesToRadians(rotation());
    moveBy(-vel*qCos(ang), -vel*qSin(ang));
}

void Car::moveRight(qreal vel)
{
    qreal ang = qDegreesToRadians(rotation());
    moveBy(vel*qCos(ang), vel*qSin(ang));
}

//ang为旋转角度（弧度），正数为顺时针，负数逆时针
void Car::Rotate(qreal ang)
{
    setRotation(rotation() + ang);
}

//r为弯道半径，ang为转过的角度
void Car::turnLeft(int r, double ang)
{
    qreal rad = qDegreesToRadians(ang);
    Forward(r*qSin(rad));
    moveLeft(r*(1-qCos(rad)));
    Rotate(-ang);
}

//r为弯道半径，ang为转过的角度
void Car::turnRight(int r, double ang)
{
    qreal rad = qDegreesToRadians(ang);
    Forward(r*qSin(rad));
    moveRight(r*(1-qCos(rad)));
    Rotate(ang);
}

void Car::setPath(Path *path)
{
    m_path = path;
    m_target = path->getNext();
    m_current = m_target;
    m_target.action = Road::none;
    this->setPos(m_target.point);
    this->setRotation(m_target.dir+90);
}

int Car::getNum()
{
    return num;
}

int Car::getEntryNum()
{
    return entryNum;
}

void Car::moveTo(QPointF target)
{
    qreal dx = m_current.point.x()-m_target.point.x();
    qreal dy = m_current.point.y()-m_target.point.y();
    qreal dis = qSqrt(qPow(dx,2)+qPow(dy,2));
    qreal dur = dis/0.3;
    posAni->setDuration(dur);
    posAni->setEndValue(target);
    posAni->start();
}

void Car::followPath()
{
    setRotation(m_target.dir+90);
    switch (m_target.action)
    {
    case Road::entry :
        Log::i("发送entry信号");
        emit entry(this);
        break;
    case Road::stair:
        Log::i("发送楼梯信号");
        emit stair(this);
        break;
    case Road::exit:
        Log::i("发送离开信号");
        emit exit(this);
        break;
    case Road::queueHead:
        Log::i("发送请求车位信号");
        emit queueHead(this);
        break;
    case Road::none:
        break;
    }
    if(!m_path->isEmpty())
    {
        m_current = m_target;
        m_target = m_path->getNext();
        moveTo(m_target.point);
    }
}

QPointF Car::getmPos()
{
    return this->pos();
}

void Car::setmPos(QPointF pos)
{
    this->setPos(pos);
    this->m_pos = pos;
}

Car::Color Car::getColor()
{
    return this->m_color;
}

qreal Car::getDir()
{
    return this->rotation();
}

QString Car::getPosition() const
{
    if (m_status == moving) {
        return "正在移动";
    } else if (m_status == waiting) {
        return "正在等候";
    } else  {
        return QString("%1%2号").arg(m_manager->getParkingLotName(targetFloor)).arg(num);
    }
}

QString Car::getPlateNumber() const
{
    return m_number;
}

QString Car::getFee() const
{
    if (m_status == waiting)
        return "0";
    int elapsed = startTime.msecsTo(QTime::currentTime());
    return QString::number(elapsed / 1000);
}

Car::Status Car::getStatus() const
{
    return m_status;
}

void Car::setStatus(const Status &value)
{
    m_status = value;
}

QTime Car::getStartTime() const
{
    return startTime;
}

void Car::setStartTime(const QTime &value)
{
    startTime = value;
}

int Car::getTargetFloor() const
{
    return targetFloor;
}

void Car::setTargetFloor(int value)
{
    targetFloor = value;
}

int Car::getCurrentFloor() const
{
    return currentFloor;
}

void Car::setCurrentFloor(int value)
{
    currentFloor = value;
}

void Car::setEntryNum(int value)
{
    entryNum = value;
}

void Car::setNum(int value)
{
    num = value;
}
