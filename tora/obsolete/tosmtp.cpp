
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

#include "tosmtp.h"
#include "toconf.h"
#include "toconfiguration.h"

#include <qapplication.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <QtGui/QTextStream>
#include <qtimer.h>

#include <QtGui/QTcpSocket>


toSMTP::toSMTP(const QString &from,
               const QString &to,
               const QString &subject,
               const QString &body)
{
    From      = from;
    Recipient = to;
    State     = Init;
    Stream    = 0;

    Socket = new QTcpSocket(this);
    connect(Socket, SIGNAL(readyRead()),
            this, SLOT(readyRead()));
    connect(Socket, SIGNAL(connected()),
            this, SLOT(connected()));

    Message = QString::fromLatin1("From: ") + from +
              QString::fromLatin1("\nTo: ") + to +
              QString::fromLatin1("\nSubject: ") + subject +
              QString::fromLatin1("\n\n") + body + "\n";
    Message.replace(QRegExp(QString::fromLatin1("\n")),
                    QString::fromLatin1("\r\n"));
    Message.replace(QRegExp(QString::fromLatin1("\r\n.\r\n")),
                    QString::fromLatin1("\r\n..\r\n"));

    QString server(toConfigurationSingle::Instance().smtp());
    int port = toConfigurationSingle::Instance().smtpPort();

    if (server.isNull() || server.isEmpty())
        toStatusMessage("No SMTP Server configured. Please check the preferences dialog.");
    else
    {
        Socket->connectToHost(server, port);
        Stream = new QTextStream(Socket);
    }
}

toSMTP::~toSMTP()
{
    delete Stream;
    delete Socket;
}

void toSMTP::connected()
{
    toStatusMessage(tr("Connected to %1").arg(Socket->peerName()), false, false);
}

void toSMTP::readyRead()
{
    // SMTP is line-oriented
    if (!Socket->canReadLine())
        return ;

    QString responseLine;
    do
    {
        responseLine = Socket->readLine();
        Response += responseLine;
    }
    while (Socket->canReadLine() && responseLine[3] != ' ');
    responseLine.truncate(3);

    if (State == Init && responseLine[0] == '2')
    {
        // banner was okay, let's go on
        *Stream << "HELO there\r\n";
        State = Mail;
    }
    else if (State == Mail && responseLine[0] == '2')
    {
        // HELO response was okay (well, it has to be)
        *Stream << "MAIL FROM: <" << From << ">\r\n";
        State = Rcpt;
    }
    else if (State == Rcpt && responseLine[0] == '2')
    {
        *Stream << "RCPT TO: <" << Recipient << ">\r\n";
        State = Data;
    }
    else if (State == Data && responseLine[0] == '2')
    {
        *Stream << "DATA\r\n";
        State = Body;
    }
    else if (State == Body && responseLine[0] == '3')
    {
        *Stream << Message << ".\r\n";
        State = Quit;
    }
    else if (State == Quit && responseLine[0] == '2')
    {
        *Stream << "QUIT\r\n";
        // here, we just close.
        State = Close;
        toStatusMessage(tr("Message sent"), false, false);
    }
    else if (State == Close)
    {
        delete this;
    }
    else
    {
        toStatusMessage(tr("Unexpected reply from SMTP server:\n\n") +
                        Response);
        State = Close;
    }

    Response = "";
}
