/*****
 *
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2003-2005 Quest Software, Inc
 * Portions Copyright (C) 2005 Other Contributors
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *      As a special exception, you have permission to link this program
 *      with the Oracle Client libraries and distribute executables, as long
 *      as you follow the requirements of the GNU GPL in regard to all of the
 *      software in the executable aside from Oracle client libraries.
 *
 *      Specifically you are not permitted to link this program with the
 *      Qt/UNIX, Qt/Windows or Qt Non Commercial products of TrollTech.
 *      And you are not permitted to distribute binaries compiled against
 *      these libraries without written consent from Quest Software, Inc.
 *      Observe that this does not disallow linking to the Qt Free Edition.
 *
 *      You may link this product with any GPL'd Qt library such as Qt/Free
 *
 * All trademarks belong to their respective owners.
 *
 *****/

#ifndef TOCONNECTIONPOOL_H
#define TOCONNECTIONPOOL_H

#include "toconnection.h"

#include <QMutex>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QWaitCondition>
#include <QThread>
#include <QEvent>
#include <QTimer>

class toConnectionPool;

class ExecFinished : public QEvent {
    QPointer<QThread> Thread;

public:
    ExecFinished(QThread *t) : QEvent(QEvent::User) {
        Thread = t;
    }


    QThread* thread() {
        return Thread;
    }
};


/**
 * Tests connections in pool
 *
 */
class toConnectionPoolTest : public QThread {
    Q_OBJECT;

    QPointer<toConnectionPool> Pool;
    QTimer *timer;

public:
    toConnectionPoolTest(toConnectionPool *pool);

    /**
     * Overrides QThread::run. Call start() to execute thread.
     *
     */
    virtual void run(void);

private slots:
    // execute tests
    void test(void);
};


/**
 * Executes sql on pool connections
 *
 */
class toConnectionPoolExec : public QThread {
    Q_OBJECT;

public:
    enum Action {
        Commit,
        Rollback,
        Cancel,
        Execute
    };

private:
    QPointer<toConnectionPool> Pool;
    Action action;

    // for Execute
    toQList Params;
    QString Sql;


public:
    toConnectionPoolExec(toConnectionPool *pool, Action act);

    toConnectionPoolExec(toConnectionPool *pool,
                         Action act,
                         const QString &sql,
                         toQList &params);


    /**
     * Overrides QThread::run. Call start() to execute thread.
     *
     */
    virtual void run(void);

protected:
    virtual void customEvent(QEvent *event);
};


class toConnectionPool : public QObject {
    Q_OBJECT;

    friend class toConnectionPoolTest;
    friend class toConnectionPoolExec;

    toConnectionPoolTest *TestThread;

    // for future configuration
    static const int PreferredSize = 3;

    enum PooledState {
        Busy,
        Free,
        Broken
    };

    class PooledSub {
    public:
        toConnectionSub *Sub;
        PooledState State;

        // need this for template container
        PooledSub() {
            Sub = 0;
            State = Broken;
        }

        PooledSub(toConnectionSub *sub, PooledState state = Free) {
            Sub = sub;
            State = state;
        }
    };


    // the pool
    QList<PooledSub *> Pool;

    // lock for the pool.
    QMutex PoolLock;

    // toConnection instance this class is a member of.  will be used
    // to create new connections when needed.
    QPointer<toConnection> Connection;

    // this is used internally to test
    PooledState test(PooledSub *sub);
    PooledState test(int member);

    // get a specific connection.
    toConnectionSub* steal(int member);

    // create new connection at position
    void fix(int member);

public:
    toConnectionPool(toConnection *conn);
    ~toConnectionPool();


    /**
     * Get a connection from pool.
     *
     * This method is thread safe
     */
    toConnectionSub* borrow(void);



    /**
     * Release connection back to pool
     *
     * This method is thread safe
     */
    void release(toConnectionSub *sub);


    /**
     * Commit connections that are not busy
     *
     */
    void commit(void);


    /**
     * Rollback connections that are not busy
     *
     */
    void rollback(void);


    /**
     * Cancels all running queries
     *
     */
    void cancelAll(bool wait = false);


    /**
     * Execute sql on all connections
     *
     */
    void executeAll(const QString &sql, toQList &params);
};


/**
 * Encapsulates returning toConnectionSub to pool for easier exception
 * proof code.
 *
 */
class PoolPtr {
    QPointer<toConnectionPool> Pool;
    toConnectionSub *Sub;

public:
    PoolPtr(toConnectionPool *pool) {
        Pool = pool;
        Sub = Pool->borrow();
    }


    toConnectionSub* operator*() {
        return Sub;
    }


    ~PoolPtr() {
        if(Pool)
            Pool->release(Sub);
    }
};

#endif
