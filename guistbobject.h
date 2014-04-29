#ifndef GUISTBOBJECT_H
#define GUISTBOBJECT_H

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
    QString getProfilesInfoJson(bool includeNewProfileItem);
    QString getStbTypes();
    QString createProfile(const QString &classId, const QString &data);
    QString getTranslations();
    QString getProfileConfigOptions(const QString &profileId);

    void loadProfile(QString id);

    bool saveProfile(const QString &id, const QString &jsonData);
protected:
    DatasourcePlugin* datasourcePlugin;
};

}

#endif // GUISTBOBJECT_H
