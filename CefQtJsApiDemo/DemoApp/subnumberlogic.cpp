#include "subnumberlogic.h"
#include <QtCore>

SubNumberLogic::SubNumberLogic(QObject*parent)
    : QObject(parent),m_counter(0){
    if (parent) {

        parent->setProperty("SubNumberLogic", QVariant::fromValue((QObject*)this));
    }
    setObjectName("SubNumberLogic");
}
qint32 SubNumberLogic::counter() {
    return m_counter;
}
void SubNumberLogic::setCounter(qint32 counter) {
    this->m_counter = counter;
}
int SubNumberLogic::addCount(qint32 i) {
    m_counter += i;
    return m_counter;
}
void SubNumberLogic::testVoidAdd(qint32 a, qint32 b) {
    m_counter += a;
    m_counter += b;
}
int SubNumberLogic::test3ArgAdd(qint32 a, qint32 b, QString c) {
    m_counter += a;
    m_counter += b;
    m_counter += c.toInt();
    return m_counter;
}
int SubNumberLogic::test4ArgAdd(qint32 a, qint32 b, QString c, QString d) {
    m_counter += a;
    m_counter += b;
    m_counter += c.toInt();
    m_counter += d.toInt();
    return m_counter;
}
void SubNumberLogic::nornmalAddCount(qint32 i) {
    m_counter += i;
}

void SubNumberLogic::onIncreace() {
    m_counter++;
    emit counterChanged(m_counter);
    emit counterChangedText(QString::number(m_counter));
}