#ifndef GUICONFIGPROFILE_H
#define GUICONFIGPROFILE_H

#include "stbprofile.h"

#include <QObject>

namespace yasem
{

class GuiConfigProfile : public QObject, public Profile
{
    Q_OBJECT
public:
    explicit GuiConfigProfile(StbPlugin* profilePlugin, const QString &id  = "");

signals:

public slots:


    // Profile interface
public:
    void start();
    void stop();
    void initDefaults();
    void configureKeyMap();

    // Profile interface
public:
    QString portal();
};

}

#endif // GUICONFIGPROFILE_H
