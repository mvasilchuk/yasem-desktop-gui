#ifndef GUISTBOBJECT_H
#define GUISTBOBJECT_H

#include <QJsonObject>
#include <QObject>

namespace yasem
{
class DatasourcePlugin;

class GuiStbObject : public QObject
{
    Q_OBJECT
public:
    explicit GuiStbObject(QObject *parent = 0);



signals:

public slots:
    QString makeJsonMenu();

    QString createProfile(const QString &classId, const QString &submodel, const QString &data);
    QString getTranslations();
    QString getProfileConfigOptions(const QString &profileId);
    QJsonObject getProfilesMenuJson();
    QJsonObject getNewProfileMenuJson();

    void loadProfile(const QString &id);

    bool saveProfile(const QString &id, const QString &jsonData);
    QString getAppInfo();
    bool removeProfile(const QString &id);
protected:
    DatasourcePlugin* datasourcePlugin;

};

}

#endif // GUISTBOBJECT_H
