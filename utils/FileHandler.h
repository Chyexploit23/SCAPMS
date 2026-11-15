#pragma once

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include "../model/ClinicDatabase.h"

class FileHandler {
public:
    // Save data to a specified file path
    static bool saveData(const QString &filePath, const QJsonObject &rootObj);

    // Load data from a specified file path into a JSON object
    static bool loadData(const QString &filePath, QJsonObject &rootObj);
};