#include "guistbobject.h"
#include "profilemanager.h"
#include "stbprofile.h"
#include "profileconfig.h"


#include <QJsonDocument>
#include <QRegularExpression>

using namespace yasem;

GuiStbObject::GuiStbObject(QObject *parent) :
    QObject(parent)
{
    datasourcePlugin = dynamic_cast<DatasourcePlugin*>(PluginManager::instance()->getByRole("datasource"));
}

QString GuiStbObject::getProfilesInfoJson(bool includeNewProfileItem = false)
{
    QList<Profile*> profiles = ProfileManager::instance()->getProfiles().toList();

    QJsonArray result;

    qSort(profiles.begin(), profiles.end(), [](Profile *first, Profile *second) { return first->getName() < second->getName(); });

    foreach(Profile* profile, profiles)
    {
        if(profile->hasFlag(Profile::HIDDEN)) continue;
        QJsonObject obj;

        obj.insert("image", profile->getImage());
        obj.insert("name", profile->getName());
        obj.insert("id", profile->getId());
        obj.insert("url", profile->datasource()->get("profile", "portal", "-----"));
        obj.insert("classId", profile->getProfilePlugin()->getProfileClassId());

        result.append(obj);
    }

    if(includeNewProfileItem)
    {
        QJsonObject obj;
        obj.insert("image", QString(""));
        obj.insert("name", tr("New profile"));
        obj.insert("id", -1);
        obj.insert("url", QString());
        obj.insert("classid", QString());

        result.append(obj);
    }

    return QString(QJsonDocument(result).toJson(QJsonDocument::Indented));
}

QString GuiStbObject::getStbTypes()
{
    QMap<QString, StbProfilePlugin*> classes = ProfileManager::instance()->getRegisteredClasses();

    QJsonArray result;

    for(auto iter = classes.begin(); iter != classes.end(); iter++)
    {
        foreach(Plugin::PluginRole role, iter.value()->roles)
        {
            if(role.hasFlag(Plugin::HIDDEN) || role.name != "stbapi") break;

            qDebug() << "GuiStbObject::getStbTypes" << iter.value()->className << role.name << role.flags;

            //TODO: Should use role name, not Plugin id
            QJsonObject obj;
            obj.insert("image", iter.value()->getImage());
            obj.insert("classId", iter.key());
            obj.insert("name", iter.key().toCaseFolded());
            result.append(obj);
        }
    }

    return QString(QJsonDocument(result).toJson(QJsonDocument::Indented));
}

QString GuiStbObject::getProfileConfigOptions(const QString &profileId)
{
   DEBUG() << profileId;
   QString result = "";
   Profile* profile = ProfileManager::instance()->findById(profileId);

   if(!profile)
   {
       WARN() << "Profile not found!";
   }
   else
   {
       QJsonArray arr;

       auto options = profile->getProfileConfig().options();
       for(ProfileConfig::Option option: options)
       {
            QJsonObject obj;

            obj.insert("tag", option.tag);
            obj.insert("name", option.name);
            obj.insert("title", option.title);
            obj.insert("comment", option.comment);



            switch(option.type)
            {
                case ProfileConfig::STRING:
                    obj.insert("value", profile->datasource()->get(option.tag, option.name, option.strValue));
                    break;
                case ProfileConfig::INT:
                    obj.insert("value", profile->datasource()->get(option.tag, option.name, option.intValue));
                    break;
                case ProfileConfig::BOOL:
                    obj.insert("value", profile->datasource()->get(option.tag, option.name, QVariant(option.boolValue).toString()));
                    break;
                default:
                    obj.insert("value", option.varValue.toString());
                    break;
            }

            obj.insert("type", option.type);

            arr.append(obj);
       }

       result = QString(QJsonDocument(arr).toJson(QJsonDocument::Indented));
       qDebug() << result;
   }
   return result;

}

QString GuiStbObject::createProfile(const QString &classId, const QString &data = "")
{
    STUB();
    qDebug() << classId << "data" << data;

    Profile* profile = ProfileManager::instance()->createProfile(classId);
    Q_ASSERT(profile);
    ProfileManager::instance()->addProfile(profile);
    qDebug() << profile;
    return profile->getId();
}

QString GuiStbObject::getTranslations()
{
    QJsonArray result;

    result.append(tr("Create new profile"));
    result.append(tr("Select profile"));
    result.append(tr("Choose base profile"));
    result.append(tr("Configure profile"));

    return QString(QJsonDocument(result).toJson(QJsonDocument::Indented));
}

void GuiStbObject::loadProfile(QString id)
{
    Profile* profile = ProfileManager::instance()->findById(id);
    Q_ASSERT(profile);
    ProfileManager::instance()->setActiveProfile(profile);
}

bool GuiStbObject::saveProfile(const QString &id, const QString& jsonData)
{
    STUB();
    qDebug() << id << jsonData;
    Profile* profile = ProfileManager::instance()->findById(id);
    Q_ASSERT(profile);
    return profile->saveJsonConfig(jsonData);
}
