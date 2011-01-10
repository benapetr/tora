
#include "tocache.h"
#include "toconfiguration.h"
#include "utils.h"
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QProgressDialog>
//#include <boost/preprocessor/iteration/detail/local.hpp>

toCache::toCache(QString description)
{
    ConnectionDescription = description;
    ReadingCache = false;
}

toCache::~toCache()
{
    if (ReadingCache)
    {
        ReadingValues.down();
        ReadingValues.down();
    }
}

QString toCache::cacheDir()
{
    QString home(QDir::homePath());
    QString dirname(toConfigurationSingle::Instance().cacheDir());

    if (dirname.isEmpty())
    {
#ifdef Q_OS_WIN32
        if (getenv("TEMP"))
            dirname = QString(getenv("TEMP"));
        else
#endif
            dirname = QString(home);
        dirname += "/.tora_cache";
    }
    return dirname;
} // cacheDir

QString toCache::cacheFile()
{
    return (cacheDir() + "/" + ConnectionDescription).trimmed();
} // cacheFile

bool toCache::loadDiskCache()
{
    if (!toConfigurationSingle::Instance().cacheDisk())
        return false;

    objectName *cur = 0;
    int objCounter = 0;
    int synCounter = 0;

    QString filename = cacheFile();

    QFile file(filename);

    if (!QFile::exists(filename))
        return false;

    QFileInfo fi(file);
    QDateTime today;
    if (fi.lastModified().addDays(toConfigurationSingle::Instance().cacheTimeout()) < today)
        return false;

    /** read in all data
     */

    if (!file.open(QIODevice::ReadOnly))
        return false;

    QString data = file.readAll();

    /** build cache lists
     */

    if(!data.isEmpty())
    {
        QStringList records = data.split("\x1D", QString::KeepEmptyParts);
        for (QStringList::Iterator i = records.begin(); i != records.end(); i++)
        {
            objCounter++;
            QStringList record = (*i).split("\x1E", QString::KeepEmptyParts);
            QStringList::Iterator rec = record.begin();
            cur = new objectName;
            (*cur).Owner = (*rec);
            rec++;
            (*cur).Name = (*rec);
            rec++;
            (*cur).Type = (*rec);
            rec++;
            (*cur).Comment = (*rec);
            rec++;
            QStringList slist = (*rec).split("\x1F", QString::SkipEmptyParts);
            for (QStringList::Iterator s = slist.begin(); s != slist.end(); s++)
            {
                SynonymMap[(*s)] = (*cur);
                (*cur).Synonyms.insert((*cur).Synonyms.end(), (*s));
                synCounter++;
            }
            /** TODO: This "if" condition is added here for bakwards compatibility as timestamp
             * parameter was not saved to disk before 2010-12-12. Therefore this condition can
             * be removed after 2011-12-12 (one year for everybody to update). Another way to
             * fix this is to delete the cache.
             */
            if (record.count() >= 6)
            {
                rec++;
                (*cur).Timestamp = QDate::fromString((*rec), "yyyy-MM-dd");
            }
            else
                (*cur).Timestamp = QDate::currentDate();
            ObjectNames.insert(ObjectNames.end(), (*cur));
            delete cur;
            cur = 0;
        }
    }

    ObjectNames.sort();

    return true;
}

void toCache::writeDiskCache()
{
    QString text;
    long objCounter = 0;
    long synCounter = 0;

    if (!toConfigurationSingle::Instance().cacheDisk())
        return ;

    QString filename(cacheFile());

    /** check pathnames and create
     */
    QString dirname(cacheDir());
    QDir dir;
    dir.setPath(dirname);

    if (!dir.exists(dirname))
        dir.mkdir(dirname);

    /** build record to write out
     */
    QStringList record; // information about one object
    QStringList records; // all objects
    QStringList recordSynonym; // all synonyms of one particular object
    for (std::list<objectName>::iterator i = ObjectNames.begin(); i != ObjectNames.end(); i++)
    {
        record.clear();
        record.append((*i).Owner);
        record.append((*i).Name);
        record.append((*i).Type);
        record.append((*i).Comment);
        for (std::list<QString>::iterator s = (*i).Synonyms.begin(); s != (*i).Synonyms.end(); s++)
        {
            recordSynonym.append((*s));
            synCounter++;
        }
        record.append(recordSynonym.join("\x1F"));
        record.append((*i).Timestamp.toString("yyyy-MM-dd"));
        recordSynonym.clear();
        objCounter++;
        records.append(record.join("\x1E"));
    }

    /** Write all records to a file
     */
    QFile file(filename);
    file.open(QIODevice::ReadWrite | QIODevice::Truncate);
    QTextStream t(&file);
    t << records.join("\x1D");
    file.flush();
    file.close();
}

bool toCache::cacheAvailable(bool synonyms, bool block, bool need, toTask * t)
{
    if (toConfigurationSingle::Instance().objectCache() == 3)
        return true;

    if (!ReadingCache)
    {
        if (!need)
            return true;
        if (toConfigurationSingle::Instance().objectCache() == 2 && !block)
            return true;
        if (t)
            readObjects(t);
        toMainWidget()->checkCaching();
    }
    if (ReadingValues.getValue() == 0 || (ReadingValues.getValue() == 1 && synonyms == true))
    {
        if (block)
        {
            toBusy busy;
            if (toThread::mainThread())
            {
                QProgressDialog waiting(qApp->translate("toConnection",
                                                        "Waiting for object caching to be completed.\n"
                                                        "Canceling this dialog will probably leave some list of\n"
                                                        "database objects empty."),
                                        qApp->translate("toConnection", "Cancel"),
                                        0,
                                        10,
                                        toMainWidget());
                waiting.setWindowTitle(qApp->translate("toConnection", "Waiting for object cache"));
                int num = 1;

                int waitVal = (synonyms ? 2 : 1);
                do
                {
                    qApp->processEvents();
                    toThread::msleep(100);
                    waiting.setValue((++num) % 10);
                    if (waiting.wasCanceled())
                        return false;
                }
                while (ReadingValues.getValue() < waitVal);
            }

            ReadingValues.down();
            if (synonyms)
            {
                ReadingValues.down();
                ReadingValues.up();
            }
            ReadingValues.up();
        }
        else
            return false;
    }
    return true;
}

bool toCache::addIfNotExists(objectName &obj)
{
    if (!cacheAvailable(true, false))
    {
        toStatusMessage(qApp->translate("toConnection", "Not done caching objects"), false, false);
        return false;
    }
    std::list<objectName>::iterator i = ObjectNames.begin();
    while (i != ObjectNames.end() && (*i) < obj)
        i++;
    if (i != ObjectNames.end() && *i == obj) // Already exists, don't add
        return false;
    ObjectNames.insert(i, obj);
    return true;
}

void toCache::readObjects(toTask * t)
{
    if (toConfigurationSingle::Instance().objectCache() == 3)
    {
        ReadingCache = false;
        return ;
    }

    if (!ReadingCache)
    {
        ReadingCache = true;
        try
        {
            (new toThread(t))->start();
            //(new toThread(new cacheObjects(this)))->start();
        }
        catch (...)
        {
            ReadingCache = false;
        }
    }
}

void toCache::rereadCache(toTask * t)
{

    if (toConfigurationSingle::Instance().objectCache() == 3)
    {
        ColumnCache.clear();
        return ;
    }

    if (ReadingValues.getValue() < 2 && ReadingCache)
    {
        toStatusMessage(qApp->translate("toConnection",
                                        "Not done caching objects, can not clear unread cache"));
        return ;
    }


    ReadingCache = false;
    while (ReadingValues.getValue() > 0)
        ReadingValues.down();

    ObjectNames.clear();
    ColumnCache.clear();
    SynonymMap.clear();

    /** delete cache file to force reload
     */

    QString filename(cacheFile());

    if (QFile::exists(filename))
        QFile::remove(filename);

    readObjects(t);
}

const toCache::objectName &toCache::realName(const QString &object,
        QString &synonym,
        bool block,
        QString user,
        QString database)
{
    if (!cacheAvailable(true, block))
        throw qApp->translate("toConnection", "Not done caching objects");

    QString name;
    QString owner;

    QChar q('"');
    QChar c('.');

    bool quote = false;
    for (int pos = 0; pos < object.length(); pos++)
    {
        if (object.at(pos) == q)
        {
            quote = !quote;
        }
        else
        {
            if (!quote && object.at(pos) == c)
            {
                owner = name;
                name = QString::null;
            }
            else
                name += object.at(pos);
        }
    }

    QString uo = owner.toUpper();
    QString un = name.toUpper();

    synonym = QString::null;
    for (std::list<objectName>::iterator i = ObjectNames.begin(); i != ObjectNames.end(); i++)
    {
        if (owner.isEmpty())
        {
            if (((*i).Name == un || (*i).Name == name) &&
                    ((*i).Owner == user.toUpper() || (*i).Owner == database))
                return *i;
        }
        else if (((*i).Name == un || (*i).Name == name) &&
                 ((*i).Owner == uo || (*i).Owner == owner))
            return *i;
    }
    if (owner.isEmpty())
    {
        std::map<QString, objectName>::iterator i = SynonymMap.find(name);
        if (i == SynonymMap.end() && un != name)
        {
            i = SynonymMap.find(un);
            synonym = un;
        }
        else
            synonym = name;
        if (i != SynonymMap.end())
        {
            return (*i).second;
        }
    }
    throw qApp->translate(
        "toConnection",
        "Object %1 not available for %2").arg(object).arg(user);
}

std::map<QString, toCache::objectName> &toCache::synonyms(bool block)
{
    if (!cacheAvailable(true, block))
    {
        toStatusMessage(qApp->translate("toConnection", "Not done caching objects"), false, false);
        static std::map<QString, objectName> ret;
        return ret;
    }

    return SynonymMap;
}

std::list<toCache::objectName> &toCache::objects(bool block)
{
    if (!cacheAvailable(false, block))
    {
        toStatusMessage(qApp->translate("toConnection", "Not done caching objects"), false, false);
        static std::list<objectName> ret;
        return ret;
    }

    return ObjectNames;
}

toQDescList &toCache::columns(const objectName &object)
{
    return ColumnCache[object];
}

void toCache::addColumns(objectName object, toQDescList list)
{
    ColumnCache[object] = list;
} // addColumns

std::list<toCache::objectName> toCache::tables(const objectName &object, bool nocache)
{
    std::list<objectName> ret;

    Q_FOREACH(objectName obj, ObjectNames)
    {
        if(obj.Owner == object.Name)
            ret.insert(ret.end(), obj);
    }

    return ret;
}

bool toCache::objectName::operator < (const objectName &nam) const
{
    if (Owner < nam.Owner || (Owner.isNull() && !nam.Owner.isNull()))
        return true;
    if (Owner > nam.Owner || (!Owner.isNull() && nam.Owner.isNull()))
        return false;
    if (Name < nam.Name || (Name.isNull() && !nam.Name.isNull()))
        return true;
    if (Name > nam.Name || (!Name.isNull() && nam.Name.isNull()))
        return false;
    if (Type < nam.Type)
        return true;
    return false;
}

bool toCache::objectName::operator == (const objectName &nam) const
{
    return Owner == nam.Owner && Name == nam.Name && Type == nam.Type;
}

void toCache::setObjectList(std::list<objectName> &list)
{
    ObjectNames = list;
    // Set the date when information about this object was red. This will later
    // be used to clean up an old information.
    for (std::list<objectName>::iterator i = ObjectNames.begin(); i != ObjectNames.end(); i++)
        (*i).Timestamp = QDate::currentDate();
    list.sort();
} // setObjectList

void toCache::setSynonymList(std::map<QString, objectName> &list)
{
    SynonymMap = list;
} // setSynonymList

bool toCache::objectExists(const QString &owner, const QString &type, const QString &name)
{
    // TODO: ObjectList is sorted therefore going through all of it is not necessary!
    for (std::list<objectName>::iterator i = ObjectNames.begin(); i != ObjectNames.end(); i++)
    {
        if (((*i).Owner == owner || owner == "%") &&
            (*i).Name == name &&
            (*i).Type == type)
            return true;
    }

    return false;
} // objectExists

toCache::RowList toCache::getObjects(const QString &owner, const QString &type)
{
    Row r;
    RowList rl;
    for (std::list<objectName>::iterator i = ObjectNames.begin(); i != ObjectNames.end(); i++)
    {
        if (((*i).Owner == owner || owner == "%") &&
            (*i).Type == type)
        {
            r.append((*i).Name);
            rl.append(r);
            r.clear();
        }
    }

    return rl;
} // getObjects

void toCache::updateObjects(const QString &owner, const QString &type, const QList<objectName> rows)
{
    bool OwnerExists = false;
    QList<objectName>::const_iterator newObjects = rows.begin();
    std::list<objectName>::iterator currentObjects = ObjectNames.begin();

    if (ObjectNames.size() > 0)
    {
        // Find first object belonging to required owner/schema
        while (currentObjects != ObjectNames.end() &&
               (*currentObjects).Owner < owner)
            currentObjects++;
        if (currentObjects != ObjectNames.end() &&
            (*currentObjects).Owner == owner)
            OwnerExists = true;
    }

    while (newObjects != rows.end())
    {
        if (OwnerExists)
        {
            while (currentObjects != ObjectNames.end() &&
                   (*currentObjects).Type != type)
                currentObjects++; // skip cached objects of other types

            if (currentObjects != ObjectNames.end() &&
                (*currentObjects).Name == (*newObjects).Name)
            {
                //qDebug() << "Object is already in cache" << (*newObjects).Name;
                currentObjects++;
                newObjects++;
            }
            else if (currentObjects != ObjectNames.end() &&
                     (*currentObjects).Name < (*newObjects).Name)
            {
                //qDebug() << "DELETE:" << (*currentObjects).Name;
                currentObjects = ObjectNames.erase(currentObjects);
            }
            else
            {
                //qDebug() << "NEW:" << (*newObjects).Name;
                ObjectNames.insert(currentObjects, (*newObjects));
                newObjects++;
            }
        }
        else
        {
            //qDebug() << "NEW2:" << (*newObjects).Name;
            ObjectNames.insert(currentObjects, (*newObjects));
            newObjects++;
        }
    }
    // Delete any remaining objects
    while (OwnerExists &&
           currentObjects != ObjectNames.end() &&
           (*currentObjects).Owner == owner)
    {
        //qDebug() << "iterating2 through" << (*currentObjects).Name;
        if ((*currentObjects).Type == type)
        {
            //qDebug() << "DELETE2:" << (*currentObjects).Name;
            currentObjects = ObjectNames.erase(currentObjects);
        }
        else
            currentObjects++;
    }
} // updateObjects
