//***************************************************************************
/*
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2000-2001,2001 Underscore AB
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
 *      these libraries without written consent from Underscore AB. Observe
 *      that this does not disallow linking to the Qt Free Edition.
 *
 * All trademarks belong to their respective owners.
 *
 ****************************************************************************/

#include "utils.h"

#include "tohighlightedtext.h"
#include "tosqlparse.h"

#ifdef TOPARSE_DEBUG

#include <stdio.h>

bool toMonolithic(void)
{
  return false;
}

void printStatement(toSQLParse::statement &stat,int level)
{
  for (int i=0;i<level;i++)
    printf(" ");

  switch(stat.Type) {
  case toSQLParse::statement::Block:
    printf("Block:");
    break;
  case toSQLParse::statement::Statement:
    printf("Statement:");
    break;
  case toSQLParse::statement::List:
    printf("List:");
    break;
  case toSQLParse::statement::Keyword:
    printf("Keyword:");
    break;
  case toSQLParse::statement::Token:
    printf("Token:");
    break;
  case toSQLParse::statement::Raw:
    printf("Raw:");
    break;
  }
  printf("%s\n",(const char *)stat.String);
  if (!stat.Comment.isNull()) {
    for (int i=0;i<level;i++)
      printf(" ");
    printf("Comment:%s\n",(const char *)stat.Comment);
  }
  for(std::list<toSQLParse::statement>::iterator i=stat.subTokens().begin();
      i!=stat.subTokens().end();
      i++)
    printStatement(*i,level+1);
}

int main(int argc,char **argv) {
  QString res="
CREATE or REPLACE Procedure TEST_SPR
(
    IN_TICKET_NUM   IN  VARCHAR2
)
IS

BEGIN

BEGIN

for cur_rec in (select emp_id from employees) loop

	update employees set emp_id = emp_id + 1
	where emp_id = cur_rec.emp_id;
	commit;
end loop;
	
END;
END TEST_SPR;


SELECT owner,
       OBJECT,
       TYPE FROM v$access
 WHERE sid=:f1<char[101]>
 ORDER BY owner,
	  OBJECT,
	  TYPE;
DECLARE
grade CHAR(1);
appraisal VARCHAR2(20);
BEGIN
appraisal := 
CASE grade
WHEN 'A' THEN 'Excellent'
WHEN 'B' THEN 'Very Good'
WHEN 'C' THEN 'Good'
WHEN 'D' THEN 'Fair'
WHEN 'F' THEN 'Poor'
ELSE 'No such grade'
END;
END;

CREATE TABLE ess.EssCalLog (
        CalID		CHAR(5) NOT NULL,		-- Calender type
	SeqID		NUMBER(8) NOT NULL,
	ActStt		CHAR(1) NOT NULL
		CONSTRAINT EssCalLog_CK_ActStt CHECK (ActStt IN ('A','D') ),
	LogRun		CHAR(1) NOT NULL		-- Should runs of this type be logged
		CONSTRAINT EssCalLog_CK_LogRun CHECK (LogRun IN ('Y','N') ),
	PrcID		NUMBER(8) NOT NULL
		CONSTRAINT EssCalDay_FK_PrcID REFERENCES ess.EssPrc(PrcID),
	Dsc		VARCHAR2(4000) NOT NULL,	-- Description of this type
	CONSTRAINT EssCal_PK PRIMARY KEY (CalID,SeqID)
		USING INDEX TABLESPACE Index02 -- A Comment
);
-- Another comment

CREATE OR REPLACE procedure spTuxGetAccData (oRet                        OUT  NUMBER,
					     oNumSwt                     OUT  NUMBER)
IS
  vYear  CHAR(4);
BEGIN
    <<label>>
    DECLARE
      oTrdStt NUMBER;
    BEGIN
      oTrdStt := 0;
    END;

    EXCEPTION
        WHEN VALUE_ERROR THEN
	    oRet := 3;
	WHEN NO_DATA_FOUND THEN
	    oRet := 2;
	WHEN OTHERS THEN
	    oRet := 1;
END;
CREATE OR REPLACE procedure spTuxGetAccData as
  vYear  CHAR(4);
begin
  null;
end;
-------------------------------------------------------------------
--    EssCal, Current calendar view

CREATE VIEW ess.EssCal AS
        SELECT CalID,
	       LogRun,
	       PrcID,
	       Dsc
	  FROM ess.EssCalLog a
	 WHERE SeqID = (SELECT MAX(aa.SeqID) FROM EssCalLog aa WHERE aa.CalID = a.CalID)
	   AND ActStt = 'A';

    /* A little comment
     */
    SELECT /*+
FULL(a)
*/ a.TskCod TskCod -- Test comment
      ,a.CreEdt CreEdt,
       a.TspActOprID /* One comment OprID */ , -- Another comment
       COUNT(1) Tot,
       COUNT(a.TspActOprID) Lft,
       b.TraCod TraCod,
       SUM(b.FinAmt) FinAmt,
       TraCod
  FROM EssTsk a,EssTra b
 WHERE ((a.TspActOprID = 'Test') OR a.TspActOprID IS NULL)
   AND DECODE(a.TspActOprID,NULL,NULL,a.TskID) = b.TskID(+)
 GROUP BY a.TskCod,a.CreEdt,a.TspActOprID,b.TraCod
HAVING COUNT(a.TspActOprID) > 0;
SELECT a.Sid \"-Id\",
       a.Serial# \"-Serial#\",
       a.SchemaName \"Schema\",
       a.Status \"Status\",
       a.Server \"Server\",
       a.OsUser \"Osuser\",
       a.Machine \"Machine\",
       a.Program \"Program\",
       a.Type \"Type\",
       a.Module \"Module\",
       a.Action \"Action\",
       a.Client_Info \"Client Info\",
       b.Block_Gets \"-Block Gets\",
       b.Consistent_Gets \"-Consistent Gets\",
       b.Physical_Reads \"-Physical Reads\",
       b.Block_Changes \"-Block Changes\",
       b.Consistent_Changes \"-Consistent Changes\",
       c.Value*10 \"-CPU (ms)\",
       a.Process \"-Process\",
       a.SQL_Address||':'||SQL_Hash_Value \" SQL Address\",
       a.Prev_SQL_Addr||':'||Prev_Hash_Value \" Prev SQl Address\"
  FROM v$session a,
       v$sess_io b,
       v$sesstat c
 WHERE a.sid = b.sid(+)
   AND a.sid = c.sid(+) AND (c.statistic# = 12 OR c.statistic# IS NULL)
 ORDER BY a.Sid;
select a.TskCod TskCod,
       count(1) Tot
  from (select * from EssTsk where PrsID >= '1940') ,EssTra b
 where decode(a.TspActOprID,NULL,NULL,a.PrsID)+5 = b.PrsID(+)
 group by a.TskCod,a.CreEdt,a.TspActOprID,b.TraCod
having count(a.TspActOprID) > 0;

CREATE OR REPLACE procedure spTuxGetAccData (oRet OUT  NUMBER)
AS
  vYear  CHAR(4);
BEGIN
    DECLARE
      oTrdStt NUMBER;
    BEGIN
      oTrdStt := 0;
    END;
    EXCEPTION
        WHEN VALUE_ERROR THEN
	    oRet := 3;
END;";

#if 0

  QApplication test(argc,argv);
  toMarkedText text(NULL);
  text.setText(res);

  int pos;
  int line;

  printf("\nForward string without comments\n");
  pos=0;
  for (QString ret=toSQLParse::getToken(res,pos,true,false);
       !ret.isNull();
       ret=toSQLParse::getToken(res,pos,true,false)) {
    printf("Token:%s\n",(const char *)ret);
  }

  printf("\nForward editor without comments\n");
  line=0;
  pos=0;
  for (QString ret=toSQLParse::getToken(&text,line,pos,true,false);
       !ret.isNull();
       ret=toSQLParse::getToken(&text,line,pos,true,false)) {
    printf("Token:%s\n",(const char *)ret);
  }

  printf("\nForward string with comments\n");
  pos=0;
  for (QString ret=toSQLParse::getToken(res,pos,true,true);
       !ret.isNull();
       ret=toSQLParse::getToken(res,pos,true,true)) {
    printf("Token:%s\n",(const char *)ret);
  }

  printf("\nForward editor with comments\n");
  line=0;
  pos=0;
  for (QString ret=toSQLParse::getToken(&text,line,pos,true,true);
       !ret.isNull();
       ret=toSQLParse::getToken(&text,line,pos,true,true)) {
    printf("Token:%s\n",(const char *)ret);
  }

  printf("\nBackward string without comments (Shouldn't work with --)\n");
  pos=res.length();
  for (QString ret=toSQLParse::getToken(res,pos,false,false);
       !ret.isNull();
       ret=toSQLParse::getToken(res,pos,false,false)) {
    printf("Token:%s\n",(const char *)ret);
  }

  printf("\nBackward editor without comments\n");
  line=text.numLines()-1;
  pos=text.textLine(line).length();
  for (QString ret=toSQLParse::getToken(&text,line,pos,false,false);
       !ret.isNull();
       ret=toSQLParse::getToken(&text,line,pos,false,false)) {
    printf("Token:%s\n",(const char *)ret);
  }

  printf("\nBackward string with comments (Shouldn't work with --)\n");
  pos=res.length();
  for (QString ret=toSQLParse::getToken(res,pos,false,true);
       !ret.isNull();
       ret=toSQLParse::getToken(res,pos,false,true)) {
    printf("Token:%s\n",(const char *)ret);
  }

  printf("\nBackward editor with comments\n");
  line=text.numLines()-1;
  pos=text.textLine(line).length();
  for (QString ret=toSQLParse::getToken(&text,line,pos,false,true);
       !ret.isNull();
       ret=toSQLParse::getToken(&text,line,pos,false,true)) {
    printf("Token:%s\n",(const char *)ret);
  }

#endif

  std::list<toSQLParse::statement> stat=toSQLParse::parse(res);

  for(std::list<toSQLParse::statement>::iterator i=stat.begin();i!=stat.end();i++) {
    printStatement(*i,1);
  }

  printf("%s\n",(const char *)toSQLParse::indent(res));

  return 0;
}

#endif

toSQLParse::statement::statement(type ntype,const QString &token,int cline)
  : Type(ntype),String(token),Line(cline)
{
  SubTokens=NULL;
}

std::list<toSQLParse::statement> &toSQLParse::statement::subTokens(void)
{
  if (!SubTokens)
    SubTokens=new std::list<statement>;
  return *SubTokens;
}

toSQLParse::statement::~statement()
{
  delete SubTokens;
}

toSQLParse::statement::statement(const statement &stat)
{
  Type=stat.Type;
  String=stat.String;
  Comment=stat.Comment;
  if (stat.SubTokens) {
    SubTokens=new std::list<statement>;
    (*SubTokens)=(*stat.SubTokens);
  } else
    SubTokens=NULL;
}

const toSQLParse::statement &toSQLParse::statement::operator = (const statement &stat)
{
  Type=stat.Type;
  String=stat.String;
  Comment=stat.Comment;
  delete SubTokens;
  if (stat.SubTokens) {
    SubTokens=new std::list<statement>;
    (*SubTokens)=(*stat.SubTokens);
  } else
    SubTokens=NULL;
  return *this;
}

static char *Operators[]={":=",
			  "=>",
			  "||",
			  "**",
			  "<<",
			  ">>",
			  "..",
			  "<>",
			  "!=",
			  "~=",
			  "^=",
			  "<=",
			  ">=",
			  NULL};

QString toSQLParse::getToken(const QString &line,int &cline,int &pos,bool forward,bool comments)
{
  QChar c;
  QChar nc;
  QChar endString;

  enum {
    space,
    any,
    identifier,
    string,
    comment,
    label,
    bindOpen,
    bindClose
  } state=space;

  QString token;

  int inc=forward?1:-1;

  while((forward&&pos<int(line.length()))||(!forward&&pos>=1)) {
    if (!forward)
      pos--;
    c=line[pos];
    if (c=='\n')
      cline++;
    if ((forward&&pos<int(line.length()-1))||(!forward&&pos>0))
      nc=line[pos+inc];
    else
      nc='\n';
    if (state==space) {
      if(forward&&c=='-'&&nc=='-') {
	int spos=pos;
	if (forward)
	  for (pos++;pos<int(line.length())&&line[pos]!='\n';pos++)
	    ;
	if (comments)
	  return line.mid(spos,pos-spos);
	continue;
      }
      if(c=='/'&&nc=='*')
	state=comment;
      else if((forward&&c=='<'&&nc=='<')||
	      (!forward&&c=='>'&&nc=='>'))
	state=label;
      else if(!c.isSpace())
	state=any;
    }
    
    if (forward)
      pos++;
    
    if (state!=space) {
      if(forward)
	token+=c;
      else
	token.prepend(c);
      switch(state) {
      case comment:
	if(c=='*'&&nc=='/') {
	  if(forward)
	    token+=nc;
	  else
	    token.prepend(nc);
	  pos+=inc;
	  if (comments)
	    return token;
	  else {
	    state=space;
	    token=QString::null;
	  }
	}
	break;
      case label:
	if((forward&&c=='>'&&nc=='>')||
	   (!forward&&c=='<'&&nc=='<')) {
	  if(forward)
	    token+=nc;
	  else
	    token.prepend(nc);
	  pos+=inc;
	  return token;
	}
	break;
      case space:
	break;
      case bindOpen:
	if(!toIsIdent(nc)) {
	  if (nc=='<')
	    state=bindClose;
	  else
	    return token;
	}
	break;
      case bindClose:
	if (c=='>')
	  return token;
	break;
      case any:
	if (c==':'&&toIsIdent(nc)) {
	  state=bindOpen;
	} else if (toIsIdent(c)) {
	  if (!toIsIdent(nc))
	    return token;
	  state=identifier;
	} else if (c=='\''||c=='\"') {
	  endString=c;
	  state=string;
	} else {
	  for (int i=0;Operators[i];i++) {
	    if ((forward&&c==Operators[i][0]&&nc==Operators[i][1])||
		(!forward&&nc==Operators[i][0]&&c==Operators[i][1])) {
	      if(forward)
		token+=nc;
	      else
		token.prepend(nc);
	      pos+=inc;
	      break;
	    }
	  }
	  return token;
	}
	break;
      case identifier:
	if (!toIsIdent(nc))
	  return token;
	break;
      case string:
	if (c==endString)
	  return token;
	break;
      }
    }
  }
  return token;
}

QString toSQLParse::getToken(toMarkedText *text,int &curLine,int &pos,bool forward,
			     bool comments)
{
  bool first=true;
  while(curLine<int(text->numLines())&&curLine>=0) {
    QString line=text->textLine(curLine);
    if (!first) {
      if (forward)
	pos=0;
      else
	pos=line.length();
    }
    QString ret=getToken(line,pos,forward,true);
    if (!ret.isNull()) {
      if (forward) {
	QString end;
	if (ret.startsWith("/*")&&
	    (ret.at(ret.length()-2)!='*'||
	     ret.at(ret.length()-1)!='/')) {
	  end="*/";
	} else if (ret.startsWith("'")&&(ret.length()<2||ret.at(ret.length()-1)!='\'')) {
	  end="'";
	} else if (ret.startsWith("\"")&&(ret.length()<2||ret.at(ret.length()-1)!='\"')) {
	  end="\"";
	}
	if (!end.isNull()) {
	  for (curLine++;
	       curLine<int(text->numLines())&&(pos=text->textLine(curLine).find(end))<0;
	       curLine++)
	    ret+="\n"+text->textLine(curLine);
	  if (curLine<int(text->numLines())) {
	    ret+="\n"+text->textLine(curLine).mid(0,pos+2);
	    pos+=2;
	  }
	}
      } else {
	QString end;
	if (ret.length()>=2&&
	    ret.at(ret.length()-2)=='*'&&
	    ret.at(ret.length()-1)=='/'&&
	    !ret.startsWith("/*")) {
	  end="/*";
	} else if ((ret.length()>=1&&ret.at(ret.length()-1)=='\'')&&
		   (ret.length()<2||ret[0]!='\'')) {
	  end="\'";
	} else if ((ret.length()>=1&&ret.at(ret.length()-1)=='\"')&&
		   (ret.length()<2||ret.at(0)!='\"')) {
	  end="\"";
	}
	if (!end.isNull()) {
	  for (curLine--;
	       curLine>=0&&(pos=text->textLine(curLine).findRev(end))<0;
	       curLine--)
	    ret.prepend(text->textLine(curLine)+"\n");
	  if (curLine>=0) {
	    QString str=text->textLine(curLine);
	    ret.prepend(str.mid(pos,str.length()-pos)+"\n");
	  }
	}
      }
      if (comments||(!ret.startsWith("/*")&&!ret.startsWith("--")))
	return ret;
      else {
	first=true;
	continue;
      }
    }
    curLine+=(forward?1:-1);
    first=false;
  }
  return QString::null;
}

toSQLParse::statement toSQLParse::parseStatement(const QString &str,int &cline,
						 int &pos,bool declare)
{
  statement ret(statement::Statement);

  toSyntaxAnalyzer &syntax=toSyntaxAnalyzer::defaultAnalyzer();

  QString first;
  bool nokey=false;
  bool block=false;
  for (QString token=getToken(str,cline,pos,true,true);
       !token.isNull();
       token=getToken(str,cline,pos,true,true)) {
    QString upp=token.upper();
#ifdef TOPARSE_DEBUG
    printf("%s\n",(const char*)token);
#endif
    if (first.isNull()&&!token.startsWith("/*")&&!token.startsWith("--"))
      first=upp;

    if (upp=="PROCEDURE"||upp=="FUNCTION"||upp=="PACKAGE")
      block=true;

    if ((first=="IF"&&upp=="THEN")||
	upp=="LOOP"||
	upp=="DECLARE"||
	(block&&upp=="AS")||
	(block&&upp=="IS")||
	(!declare&&upp=="BEGIN")) {
      statement blk(statement::Block);
      ret.subTokens().insert(ret.subTokens().end(),statement(statement::Keyword,token,cline));
      blk.subTokens().insert(blk.subTokens().end(),ret);
      statement cur(statement::Statement);
      bool dcl=(upp=="DECLARE"||upp=="IS"||upp=="AS");
      do {
	cur=parseStatement(str,cline,pos,dcl);
	if (cur.Type==statement::List)
	  toStatusMessage("Unbalanced parenthesis (Too many ')')");
	blk.subTokens().insert(blk.subTokens().end(),cur);
      } while(cur.subTokens().begin()!=cur.subTokens().end()&&
	      (*cur.subTokens().begin()).String.upper()!="END");
      return blk;
    } else if (upp=="THEN"||upp=="BEGIN"||upp=="EXCEPTION") {
      ret.subTokens().insert(ret.subTokens().end(),statement(statement::Keyword,token,cline));
      return ret;
    } else if (upp==","||(syntax.reservedWord(upp)&&upp!="NOT"&&upp!="IS"&&upp!="LIKE"&&upp!="IN"&&upp!="BETWEEN"&&upp!="ASC"&&upp!="DESC"&&upp!="NULL")&&!nokey) {
      ret.subTokens().insert(ret.subTokens().end(),statement(statement::Keyword,token,cline));
      nokey=false;
    } else if (upp=="(") {
      ret.subTokens().insert(ret.subTokens().end(),statement(statement::Token,token,cline));
      statement lst=parseStatement(str,cline,pos,false);
      statement t=toPop(lst.subTokens());
      if (lst.Type!=statement::List)
	toStatusMessage("Unbalanced parenthesis (Too many '(')");
      nokey=false;
      if (first=="CREATE"&&!block) {
	statement end=parseStatement(str,cline,pos,false);
	statement blk(statement::Block);
	blk.subTokens().insert(blk.subTokens().end(),ret);
	blk.subTokens().insert(blk.subTokens().end(),lst);
	end.subTokens().insert(end.subTokens().begin(),t);
	blk.subTokens().insert(blk.subTokens().end(),end);
	return blk;
      } else {
	ret.subTokens().insert(ret.subTokens().end(),lst);
	ret.subTokens().insert(ret.subTokens().end(),t);
      }
    } else if (upp==")") {
      ret.Type=statement::List;
      ret.subTokens().insert(ret.subTokens().end(),statement(statement::Token,token,cline));
      return ret;
    } else if (upp==";") {
      ret.subTokens().insert(ret.subTokens().end(),statement(statement::Token,token,cline));
      return ret;
    } else if (upp.startsWith("/*+")||upp.startsWith("--+")) {
      QString com=token;
      if (com.startsWith("--+"))
	com="/*+ "+com.mid(3)+" */";
      ret.subTokens().insert(ret.subTokens().end(),statement(statement::Token,
							   com.simplifyWhiteSpace(),cline));
    } else if (upp.startsWith("/*")||upp.startsWith("--")) {
      if (ret.subTokens().begin()==ret.subTokens().end()) {
	if (ret.Comment.isNull())
	  ret.Comment=token;
	else
	  ret.Comment+="\n"+token;
      } else {
	QString &com=(*ret.subTokens().rbegin()).Comment;
	if (com.isEmpty())
	  com=token;
	else
	  com+="\n"+token;
      }
    } else {
      ret.subTokens().insert(ret.subTokens().end(),statement(statement::Token,token,cline));
      nokey=(token==".");
    }
    if (upp=="AS"||upp=="IS")
      first="INVALID";
  }
  return ret;
}

std::list<toSQLParse::statement> toSQLParse::parse(const QString &str)
{
  int pos=0;
  std::list<toSQLParse::statement> ret;
  statement cur(statement::Statement);
  int cline=0;
  for(cur=parseStatement(str,cline,pos,false);
      cur.subTokens().begin()!=cur.subTokens().end();
      cur=parseStatement(str,cline,pos,false)) {
    if (cur.Type==statement::List)
      toStatusMessage("Unbalanced parenthesis (Too many ')')");
    ret.insert(ret.end(),cur);
  }
  if (pos<int(str.length()))
    ret.insert(ret.end(),statement(statement::Raw,
				   str.mid(pos,str.length()-pos),cline));
  return ret;
}

#define TABSTOP 8

int toSQLParse::countIndent(const QString &txt,int &chars)
{
  int level=0;
  while(txt[chars].isSpace()&&chars<int(txt.length())) {
    char c=txt[chars];
    if (c=='\n')
      level=0;
    else if (c==' ')
      level++;
    else if (c=='\t')
      level=(level/TABSTOP+1)*TABSTOP;
    chars++;
  }
  return level;
}

toSQLParse::settings toSQLParse::Settings={true,
					   false,
					   false,
					   false,
					   true,
					   true,
					   true,
					   4,
					   60};

QString toSQLParse::indentString(int level)
{
  QString ret;
  if (Settings.ExpandSpaces) {
    for(int i=0;i<level/8;i++)
      ret+="\t";
    for(int j=0;j<level%8;j++)
      ret+=" ";
  } else
    for(int j=0;j<level;j++)
      ret+=" ";
  return ret;
}

static int CurrentColumn(const QString &txt)
{
  int pos=txt.findRev("\n");
  if (pos<0)
    pos=0;
  else
    pos++;

  int level=0;
  while(pos<int(txt.length())) {
    char c=txt[pos];
    if (c=='\n')
      level=0;
    else if (c=='\t')
      level=(level/TABSTOP+1)*TABSTOP;
    else
      level++;
    pos++;
  }
  return level;

}

static QString IndentComment(int level,int current,const QString &comment,bool endNl)
{
  bool nl=true;
  QString ret;
  if (comment.length()) {
    if (level<=current&&(level||current)) {
      ret+="\n";
      current=0;
    }
    for (unsigned int i=0;i<comment.length();i++) {
      if (!nl||!comment.at(i).isSpace()) {
	if (nl) {
	  if (current==0)
	    ret+=toSQLParse::indentString(level);
	  else {
	    while(current<level) {
	      ret+=" ";
	      current++;
	    }
	  }
	  if (comment.at(i)=="*") {
	    ret+=" ";
	    current++;
	  }
	  nl=false;
	}
	ret+=comment.at(i);
	if (comment.at(i)=='\n') {
	  current=0;
	  nl=true;
	} else
	  nl=false;
      }
    }
    if (!nl)
      ret+="\n";
  } else if (endNl) {
    ret="\n";
  }
  return ret;
}

static QString AddComment(const QString &old,const QString &comment)
{
  QString ret=old;
  if (!ret.isEmpty()&&!comment.isEmpty())
    ret+="\n";
  ret+=comment;
  return ret;
}

QString toSQLParse::indentStatement(statement &stat,int level)
{
  QString ret;
  toSyntaxAnalyzer &syntax=toSyntaxAnalyzer::defaultAnalyzer();

  switch(stat.Type) {
  default:
    throw QString("Internal error in toSQLParse, should never get here");
    break;
  case statement::Block:
    {
      ret=IndentComment(level,0,stat.Comment,false);
      int exc=0;
      for(std::list<toSQLParse::statement>::iterator i=stat.subTokens().begin();
	  i!=stat.subTokens().end();
	  i++) {
	int add=0;
	std::list<toSQLParse::statement>::iterator j=i;
	j++;
	if (i!=stat.subTokens().begin()&&
	    j!=stat.subTokens().end())
	  add=Settings.IndentLevel;
	else
	  exc=0;

	QString t;
	if ((*i).subTokens().begin()!=(*i).subTokens().
	  end())
	  t=(*(*i).subTokens().begin()).String.upper();
	if (t=="BEGIN"||t=="WHEN")
	  add=0;
	if ((*i).Type==statement::List)
	  ret+=indentString(level+add+exc);
	ret+=indentStatement(*i,level+add+exc);
	if ((*i).Type==statement::List) {
	  int i;
	  for (i=ret.length()-1;i>=0&&ret[i].isSpace();i--)
	    ;
	  ret=ret.mid(0,max(i+1,0));
	  ret+="\n";
	  ret+=indentString(level+exc);
	}
	if (t=="EXCEPTION")
	  exc=Settings.IndentLevel*2;
      }
      if (Settings.EndBlockNewline&&level!=0)
	ret+="\n";
    }
    break;
  case statement::List:
  case statement::Statement:
    int maxlev=0;
    int maxlevorig=0;
    bool useMaxLev=false;
    bool any=true;
    int current;
    bool first;
    bool noKeyBreak=false;
    bool lineList=false;
    QString comment;
    if (stat.Type==statement::Statement) {
      ret=IndentComment(level,0,stat.Comment,false);
      useMaxLev=true;
      first=true;
      current=0;
    } else {
      for(std::list<toSQLParse::statement>::iterator i=stat.subTokens().begin();
	  i!=stat.subTokens().end();) {
	if ((*i).Type!=statement::Keyword)
	  noKeyBreak=true;
	else
	  useMaxLev=true;
	break;
      }
      current=level;
      first=true;
    }
    if (useMaxLev) {
      int count=0;
      for(std::list<toSQLParse::statement>::iterator i=stat.subTokens().begin();
	  i!=stat.subTokens().end();
	  i++) {
	if (any) {
	  if ((*i).Type==statement::Keyword) {
	    if (int((*i).String.length())+1>maxlev)
	      maxlev=(*i).String.length()+1;
	    count++;
	    any=false;
	  } else if (i==stat.subTokens().begin()) {
	    noKeyBreak=true;
	    break;
	  }
	} else if ((*i).Type==statement::Token)
	  any=true;
	if ((*i).Type==statement::List)
	  count++;
      }
      if (count<=1&&maxlev>0)
	maxlev--;
      maxlevorig=maxlev;
      any=true;
    }

    for(std::list<toSQLParse::statement>::iterator i=stat.subTokens().begin();
	i!=stat.subTokens().end();
	i++) {
      comment=AddComment(comment,(*i).Comment);
      QString upp=(*i).String.upper();
#ifdef TOPARSE_DEBUG
      printf("%s\n",(const char*)(*i).String);
#endif
      if ((*i).Type==statement::List) {
	if (Settings.OperatorSpace) {
	  ret+=" ";
	  current++;
	}
	QString t=indentStatement(*i,current);
	if (t.find("\n")>=0)
	  current=CurrentColumn(t);
	else
	  current+=CurrentColumn(t);
	ret+=t;
	any=true;
      } else if ((*i).String==",") {
	if (Settings.CommaBefore) {
	  ret+=IndentComment(Settings.CommentColumn,current,comment,true);
	  comment=QString::null;
	  ret+=indentString(level+maxlev-(Settings.OperatorSpace?2:1));
	  ret+=",";
	} else {
	  ret+=",";
	  ret+=IndentComment(Settings.CommentColumn,current+1,comment,true);
	  comment=QString::null;
	  ret+=indentString(level+maxlev);
	}
	current=level+maxlev;
	any=false;
	lineList=true;
      } else if ((*i).Type==statement::Keyword&&(upp=="LOOP"||upp=="THEN"||upp=="AS"||upp=="IS")) {
	if (!Settings.BlockOpenLine) {
	  if (ret.length()>0) {
	    if (toIsIdent(ret.at(ret.length()-1))||
		ret.at(ret.length()-1)=='\"'||
		ret.at(ret.length()-1)=='\''||
		Settings.OperatorSpace) {
	      ret+=" ";
	      current++;
	    }
	  }
	  ret+=Settings.KeywordUpper?(*i).String.upper():(*i).String;
	  current+=(*i).String.length();
	} else {
	  ret+=IndentComment(Settings.CommentColumn,current,comment,true);
	  comment=QString::null;
	  ret+=indentString(level);
	  ret+=Settings.KeywordUpper?(*i).String.upper():(*i).String;
	  current=level+(*i).String.length();
	}
	any=false;
      } else if (any&&(*i).Type==statement::Keyword&&!noKeyBreak) {
	if (first)
	  first=false;
	else {
	  ret+=IndentComment(Settings.CommentColumn,current,comment,true);
	  current=0;
	  comment=QString::null;
	}
	if (current==0) {
	  ret+=indentString(level);
	  current=level;
	} else
	  while(current<level) {
	    ret+=" ";
	    current++;
	  }
	maxlev=maxlevorig;
	QString word=Settings.KeywordUpper?(*i).String.upper():(*i).String;
	if (ret.length()) {
	  ret+=QString("%1").arg(word,
				 Settings.RightSeparator?maxlev-1:1-maxlev);
	  current=level+max(int(word.length()),maxlev-1);
	} else {
	  ret+=word;
	  current=level+word.length();
	}
	any=false;
	lineList=false;
      } else {
	QString t=(*i).String;
	bool add=false;
	if ((*i).Type==statement::Keyword) {
	  if (!lineList&&
	      !any&&
	      (*i).Type==statement::Keyword&&
	      !noKeyBreak&&
	      upp=="BY")
	    add=true;
	} else {
	  any=true;
	}
	if (syntax.reservedWord(upp)&&Settings.KeywordUpper)
	  t=upp;

	int extra;
	if (first) {
	  first=false;
	  any=false;
	  extra=0;
	} else {
	  if (ret.length()>0&&
	      !ret.at(ret.length()-1).isSpace()&&
	      (Settings.OperatorSpace||((toIsIdent(t[0])||
					 t[0]=='\"'||t[0]=='\'')&&
					(toIsIdent(ret.at(ret.length()-1))||
					 ret.at(ret.length()-1)=='\"'||
					 ret.at(ret.length()-1)=='\'')
					)
	       )
	      ) {
	    if (t!=";"&&
		t!="."&&
		ret.at(ret.length()-1)!='.'&&
		current!=0) {
	      current++;
	      ret+=" ";
	    }
	  } else if (ret.length()>2&&ret.at(ret.length()-2)=='*'&&ret.at(ret.length()-1)=='/') {
	    current++;
	    ret+=" ";
	  }
	  extra=maxlev;
	}
	if (current<level+maxlev) {
	  if (current==0)
	    ret+=indentString(level+maxlev);
	  else
	    while(current<level+maxlev) {
	      ret+=" ";
	      current++;
	    }
	  current=level+maxlev;
	}
	ret+=t;
	current+=t.length();
	if(t.startsWith("<<")) {
	  ret+="\n";
	  current=0;
	}

	if (add)
	  maxlev+=t.length()+1;
      }
    }
    if (stat.Type==statement::Statement) {
      ret+=IndentComment(Settings.CommentColumn,current,comment,true);
      comment=QString::null;
      if (Settings.EndBlockNewline&&
	  level==0&&
	  stat.subTokens().begin()!=stat.subTokens().end()&&
	  (*stat.subTokens().rbegin()).String==";")
	ret+="\n";
    } else if (!comment.isEmpty()) {
      ret+=IndentComment(Settings.CommentColumn,current,comment,true);
      comment=QString::null;
      ret+=indentString(level-(Settings.OperatorSpace?2:1));
    }
    break;
  }
  return ret;
}

QString toSQLParse::indent(const QString &str)
{
  std::list<toSQLParse::statement> blk=parse(str);
  int pos=0;
  int level=countIndent(str,pos);

  QString ret;
  for(std::list<toSQLParse::statement>::iterator i=blk.begin();
      i!=blk.end();
      i++) {
    ret+=indentStatement(*i,level);
  }
  pos=ret.length();
  while(pos>0&&ret[pos-1].isSpace()) {
    pos--;
  }
  return ret.mid(0,pos)+"\n";
}
