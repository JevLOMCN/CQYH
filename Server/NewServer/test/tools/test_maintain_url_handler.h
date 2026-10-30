#pragma once
#include "stdafx.h"
#include <QObject>
#include <QUrl>
#include <vector>

class MaintainUrlHandler:public QObject
{
    Q_OBJECT
public:
    std::vector<QUrl> urls;
public slots:
    void receive(QUrl url) { urls.push_back(std::move(url)); }
};
