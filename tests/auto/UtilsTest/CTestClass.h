#ifndef CTESTCLASS_H
#define CTESTCLASS_H

#include <QObject>

class CTestClass : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("prorerty", "propertyvalue")
public:
    explicit CTestClass(QObject *parent = nullptr);
};

#endif // CTESTCLASS_H
