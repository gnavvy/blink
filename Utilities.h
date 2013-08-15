#ifndef UTILITIES_H
#define UTILITIES_H

#include <QFile>
#include <QThread>
#include <QString>
#include <QDateTime>
#include <QTextStream>

class Sleeper : public QThread {
public:
    static void usleep(unsigned long usecs) { QThread::usleep(usecs); }
    static void msleep(unsigned long msecs) { QThread::msleep(msecs); }
    static void sleep(unsigned long secs)   { QThread::sleep(secs); }
};

#endif // UTILITIES_H
