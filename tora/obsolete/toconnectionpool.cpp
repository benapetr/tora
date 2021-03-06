
/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 *
 * Shared/mixed copyright is held throughout files in this product
 *
 * Portions Copyright (C) 2000-2001 Underscore AB
 * Portions Copyright (C) 2003-2005 Quest Software, Inc.
 * Portions Copyright (C) 2004-2013 Numerous Other Contributors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program as the file COPYING.txt; if not, please see
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
 *
 *      As a special exception, you have permission to link this program
 *      with the Oracle Client libraries and distribute executables, as long
 *      as you follow the requirements of the GNU GPL in regard to all of the
 *      software in the executable aside from Oracle client libraries.
 *
 * All trademarks belong to their respective owners.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "utils.h"
#include "tologger.h"

#include "toconnectionpool.h"
#include "toconfiguration.h"
#include "tosql.h"
#include "toquery.h"

#include <QtGui/QTimer>
#include <QtGui/QDateTime>
#include <QtGui/QCoreApplication>
#include <QtGui/QApplication>

toConnectionPoolTest::toConnectionPoolTest(toConnectionPool *pool)
    : QThread(0), timer(0)
{
    setObjectName("toConnectionPoolTest");
    Pool = pool;
}

void toConnectionPoolTest::run()
{
    //toThread::Lock->lock (); not initialized yet
    toThread::toThreadInfoStorage.setLocalData(new toThreadInfo(++toThread::lastThreadNumber));
    //toThread::Lock->unlock();

    timer = new QTimer();
    timer->setInterval(toConfigurationSingle::Instance().connTestInterval() * 1000); // Test interval in miliseconds and connTestInterval in seconds
    connect(timer, SIGNAL(timeout()), this, SLOT(test()), Qt::DirectConnection);
    timer->start();

    exec();

    delete timer;
}

void toConnectionPoolTest::test()
{
    for(int i = 0; Pool && i < Pool->size(); i++)
    {
        TLOG(0, toDecorator, __HERE__) << "Pool member id:"  << i << std::endl;
        toConnectionPool::PooledState state = Pool->test(i);
        if(state == toConnectionPool::Busy)
        {
            TLOG(0, toDecorator, __HERE__) << "Pool member id:"  << i << " Busy" << std::endl;
            continue;
        }

        if(state == toConnectionPool::Broken)
        {
            TLOG(0, toDecorator, __HERE__) << "Pool member id:"  << i << " Broken" << std::endl;
            Pool->fix(i);
        }
    }
}

toConnectionPoolExec::toConnectionPoolExec(toConnectionPool *pool, Action act)
{
    action = act;
    Pool   = pool;
    setObjectName("toConnectionPoolExec");
}


toConnectionPoolExec::toConnectionPoolExec(toConnectionPool *pool,
        Action act,
        const QString &sql,
        toQList &params)
{
    action = act;
    Pool   = pool;
    Sql    = sql;
    Params = params;
}


void toConnectionPoolExec::run()
{
    // toThread::Lock->lock ();
    // toThread::toThreadInfoStorage.setLocalData(new toThreadInfo(++toThread::lastThreadNumber));
    // toThread::Lock->unlock();

    for(int mem = 0; Pool && mem < Pool->size(); mem++)
    {
        toConnectionSub *sub = Pool->steal(mem);
        if(!sub)
            continue;

        try
        {
            switch(action)
            {
            case Commit:
                Pool->Connection->commit(sub);
                break;

            case Rollback:
                Pool->Connection->rollback(sub);
                break;

            case Cancel:
                sub->cancel();
                // send it back to the pool

                // edit: not a good idea, can cause dead locks in sub
                // if you're extremely unlucky
                // Pool->release(sub);
                break;

            case Execute:
            {
                toQuery q(*(Pool->Connection), sub, Sql, Params);
//                 Pool->Connection->commit(sub);
                break;
            }

            } // switch
        }
        TOCATCH; // show errors to user
    }
}


toConnectionPool::toConnectionPool(toConnection *conn) : QObject(NULL)
{
    Connection = conn;
    LockingPtr<SubList> ptr(Pool, PoolLock);

    for(int i = 0; i < PreferredSize; i++)
    {
        PooledSub *psub = new PooledSub;
        psub->Sub = Connection->addConnection();
        psub->State = Free;

        ptr->append(psub);
    }

    if(toConfigurationSingle::Instance().firewallMode())
    {
        TestThread = new toConnectionPoolTest(this);
        TestThread->start();
        TestThreadRunning = true;
    }
    else
        TestThreadRunning = false;
}


toConnectionPool::~toConnectionPool()
{
    if (TestThreadRunning)
    {
        QMetaObject::invokeMethod(TestThread, "quit", Qt::QueuedConnection);
#ifdef Q_WS_MAC
        // This is propably mandatory on Mac. If it's missing, the TestThread->wait()
        // will block entire GUI for ever.
        TestThread->quit();
#endif
        // must call this or the queued call never executes while we wait
        // on the thread. Awesome.
        qApp->processEvents();
        TestThread->wait();
        delete TestThread;
    }

    toConnection *conn = Connection;
    Connection = 0;

    LockingPtr<SubList> ptr(Pool, PoolLock);

    for(int mem = 0; mem < ptr->size(); mem++)
    {
        PooledSub *psub = (*ptr)[mem];
        try
        {
            if(psub->Sub)
                psub->Sub->cancel();
        }
        catch(...)
        {
            TLOG(1, toDecorator, __HERE__) << " Ignored exception." << std::endl;
        }

        if(psub->Sub)
            conn->closeConnection(psub->Sub);
    }

    while(!ptr->isEmpty())
        delete ptr->takeFirst();
}


void toConnectionPool::fix(int member)
{
    if(!Connection)
        return;

    LockingPtr<SubList> ptr(Pool, PoolLock);
    PooledSub *psub = (*ptr)[member];
    psub->State = Broken;
    ptr.unlock();

    Connection->closeConnection(psub->Sub);
    PooledState state = Broken;
    toConnectionSub *sub = 0;
    try
    {
        sub = Connection->addConnection();
        state = Free;
    }
    catch(...)
    {
        TLOG(1, toDecorator, __HERE__) << " Ignored exception." << std::endl;
        state = Broken;
    }

    ptr.lock();
    psub->State = state;
    psub->Sub = sub;
}


toConnectionPool::PooledState toConnectionPool::test(int member)
{
    if(!Connection)
        return Broken;

    LockingPtr<SubList> ptr(Pool, PoolLock);
    PooledSub *psub = (*ptr)[member];
    if(psub->State != Free)
    {
        TLOG(0, toDecorator, __HERE__) << "Pool member:" << member << " is NOT free" << std::endl;
        return psub->State;
    }
    TLOG(0, toDecorator, __HERE__) << "Pool member" << member << " is free" << std::endl;

    psub->State = Busy;
    ptr.unlock();

    PooledState state = test(psub);

    ptr.lock();
    psub->State = state;
    return psub->State;
}

toConnectionPool::PooledState toConnectionPool::test(PooledSub *sub)
{
    PooledState state = Free;
    try
    {
        QString sql = toSQL::string("Global:Now", *Connection);
        //TLOG(0,toDecorator,__HERE__) << sql.toUtf8().constData() << std::endl;
        if(sql.isEmpty())
            return state;

        toQList params;
        toQuery q(*Connection,
                  sub->Sub,
                  sql,
                  params);
    }
    catch(...)
    {
        TLOG(1, toDecorator, __HERE__) << " Ignored exception." << std::endl;
        state = Broken;
    }

    return state;
}


int toConnectionPool::size()
{
    LockingPtr<SubList> ptr(Pool, PoolLock);
    return ptr->size();
}


toConnectionSub* toConnectionPool::steal(int member)
{
    LockingPtr<SubList> ptr(Pool, PoolLock);
    return (*ptr)[member]->Sub;
}


/* Checks which connection is available in a pool and then returns it.
*/
toConnectionSub* toConnectionPool::borrow()
{
    {
        // keep lock here so adding connection below can be sure
        // there's no current lock in case of exception
        LockingPtr<SubList> ptr(Pool, PoolLock);
        int connectionTestTimeout = toConfigurationSingle::Instance().connectionTestTimeout();

        for(int mem = 0; mem < ptr->size(); mem++)
        {
            PooledSub *psub = (*ptr)[mem];

            if(psub->State == Free)
            {
                psub->State = Busy;
                ptr.unlock();

                // Check how many seconds have passed since something has been executed
                // on this particular database connection.
                int diff = psub->Sub->lastUsed().secsTo(QDateTime::currentDateTime());
                PooledState state;
                // If more than configured amount has passed - perform a connection
                // test - run some simple operation to check if connection is still alive.
                if (diff > connectionTestTimeout)
                    state = test(psub);
                else
                    state = Free;
                if (state == Free)
                    return psub->Sub;
                else
                {
                    ptr.lock();
                    psub->State = state;
                }

                // be careful adding code after this, as the pool size
                // might have changed since lock was last acquired.
            }
        }
    }

    toConnectionSub *sub = Connection->addConnection();
    PooledSub *psub = new PooledSub(sub, Busy);

    LockingPtr<SubList> ptr(Pool, PoolLock);
    ptr->append(psub);
    return psub->Sub;
}


void toConnectionPool::release(toConnectionSub *sub)
{
    LockingPtr<SubList> ptr(Pool, PoolLock);

    for(int mem = 0; mem < ptr->size(); mem++)
    {
        PooledSub *psub = (*ptr)[mem];
        if(psub->Sub == sub)
        {

            // if needscommit is false, we can eliminate extra
            // connections here.
            if(ptr->size() > PreferredSize &&
                    Connection &&
                    !Connection->needCommit())
            {

                PooledSub *psub = ptr->takeAt(mem);
                Connection->closeConnection(psub->Sub);
                delete psub;
            }
            else
                psub->State = Free;

            return;
        }
    }

    if(Connection)
        Connection->closeConnection(sub);
}


void toConnectionPool::commit(bool wait)
{
    toConnectionPoolExec *ex = new toConnectionPoolExec(
        this,
        toConnectionPoolExec::Commit);
    if(!wait)
        (new toRunnableThread(ex))->start();
    else
        ex->run();
}


void toConnectionPool::rollback(bool wait)
{
    toConnectionPoolExec *ex = new toConnectionPoolExec(
        this,
        toConnectionPoolExec::Rollback);
    if(!wait)
        (new toRunnableThread(ex))->start();
    else
        ex->run();
}


void toConnectionPool::cancelAll(bool wait)
{
    toConnectionPoolExec *ex = new toConnectionPoolExec(
        this,
        toConnectionPoolExec::Cancel);
    if(!wait)
        (new toRunnableThread(ex))->start();
    else
        ex->run();
}


void toConnectionPool::executeAll(const QString &sql, toQList &params)
{
    (new toRunnableThread(new toConnectionPoolExec(
                              this,
                              toConnectionPoolExec::Execute,
                              sql,
                              params)))->start();
}
