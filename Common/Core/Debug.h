#ifndef DEBUG_H
#define DEBUG_H

#undef __DEBUG
#ifdef QT_DEBUG
#include <QDebug>
#define __DEBUG(A) qDebug() << (A);
#else
#define __DEBUG(A)
#endif

#endif // DEBUG_H
