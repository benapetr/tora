//***************************************************************************
/* $Id$
**
** Copyright (C) 2000-2001 GlobeCom AB.  All rights reserved.
**
** This file is part of the Toolkit for Oracle.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.globecom.net/tora/ for more information.
**
** Contact tora@globecom.se if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef __TOOUTPUT_H
#define __TOOUTPUT_H

#include "totool.h"

class toConnection;
class toMarkedText;
class QToolButton;

class toOutput : public toToolWidget {
  Q_OBJECT

  toMarkedText *Output;
  QToolButton *DisableButton;
  void poll(void);
public:
  toOutput(QWidget *parent,toConnection &connection,bool enabled=true);
  virtual ~toOutput();
  void insertLine(const QString &str)
  { Output->insertLine(str); }
  bool enabled(void);
public slots:
  void clear(void);
  virtual void refresh(void);
  virtual void disable(bool);
  void changeRefresh(const QString &str);
};

#endif
