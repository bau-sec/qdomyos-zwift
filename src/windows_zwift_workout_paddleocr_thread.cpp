#include "windows_zwift_workout_paddleocr_thread.h"
#include "elliptical.h"
#include "treadmill.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QMetaEnum>
#include <QProcess>
#include <QSettings>
#include <QThread>
#include <chrono>
#include <math.h>

using namespace std::chrono_literals;

windows_zwift_workout_paddleocr_thread::windows_zwift_workout_paddleocr_thread(bluetoothdevice *device) {
    this->device = device;
    emit debug("windows_zwift_workout_paddleocr_thread created!");
}

void windows_zwift_workout_paddleocr_thread::run() {
    float lastInclination = -100;
    float lastSpeed = -100;
    while (1) {
        QString ret = runPython("zwift-workout.py");
        if (ret.length() > 0) {
            QStringList list = ret.split(";");
            if (list.length() >= 2) {
                emit debug("windows_zwift_workout_paddleocr_thread onInclination " + list.at(1) + " onSpeed " +
                           list.at(0).toFloat());
                if (!list.at(1).toUpper().contains("NONE")) {
                    float inc = list.at(1).toFloat();
                    if (inc != lastInclination)
                        emit onInclination(inc, inc);
                    lastInclination = inc;
                }
                if (!list.at(0).toUpper().contains("NONE")) {
                    float speed = list.at(0).toFloat();
                    if (speed != lastSpeed)
                        emit onSpeed(speed);
                    lastSpeed = speed;
                }
            }
        }
    }
}

QString windows_zwift_workout_paddleocr_thread::runPython(QString command) {
#ifdef Q_OS_WINDOWS
    QProcess process;
    qDebug() << "run >> " << command;
    process.start("python\\x64\\python.exe", QStringList(command.split(' ')));
    process.waitForFinished(-1); // will wait forever until finished

    QString out = process.readAllStandardOutput();
    QString err = process.readAllStandardError();

    emit debug("python << OUT " + out);
    emit debug("python << ERR " + err);
#else
    QString out;
#endif
    return out;
}
