#include "Filehandler.h"
#include <QFile>
#include <QJsonDocument>
#include <QTextStream>
#include <QDebug>

bool Filehandler::saveData(const QString &filePath, const QJsonObject &rootObj) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for writing:" << filePath;
        return false;
    }

    QJsonDocument doc(rootObj);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool Filehandler::loadData(const QString &filePath, QJsonObject &rootObj) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for reading:" << filePath;
        return false;
    }

    QByteArray saveData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(saveData);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid JSON format in file:" << filePath;
        return false;
    }

    rootObj = doc.object();
    return true;
}