#include <stdio.h>

#include <qfontdialog.h>
#include <qcolordialog.h>

#include "tohighlightedtext.h"
#include "tosyntaxsetup.h"
#include "toconf.h"

#include "tosyntaxsetup.ui.moc"
#include "tosyntaxsetup.ui.cpp"

toSyntaxSetup::toSyntaxSetup(QWidget *parent,const char *name,WFlags fl)
  : toSyntaxSetupUI(parent,name,fl),Analyzer(toDefaultAnalyzer())
{
  if (!toTool::globalConfig(CONF_KEYWORD_UPPER,"").isEmpty())
    KeywordUpper->setChecked(true);
  if (!toTool::globalConfig(CONF_HIGHLIGHT,"Yes").isEmpty())
    SyntaxHighlighting->setChecked(true);

  QFont font( "Courier", 12, QFont::Bold);
  QString fnt=toTool::globalConfig(CONF_TEXT,"");
  if (fnt.length())
    font.setRawName(fnt);
  Text=font.rawName();
  CodeExample->setFont(font);
  Colors[Analyzer.typeString(toSyntaxAnalyzer::NormalBkg)]=Analyzer.getColor(toSyntaxAnalyzer::NormalBkg);
  Colors[Analyzer.typeString(toSyntaxAnalyzer::ErrorBkg)]=Analyzer.getColor(toSyntaxAnalyzer::ErrorBkg);
  Colors[Analyzer.typeString(toSyntaxAnalyzer::CurrentBkg)]=Analyzer.getColor(toSyntaxAnalyzer::CurrentBkg);
  Colors[Analyzer.typeString(toSyntaxAnalyzer::Keyword)]=Analyzer.getColor(toSyntaxAnalyzer::Keyword);
  Colors[Analyzer.typeString(toSyntaxAnalyzer::Comment)]=Analyzer.getColor(toSyntaxAnalyzer::Comment);
  Colors[Analyzer.typeString(toSyntaxAnalyzer::Normal)]=Analyzer.getColor(toSyntaxAnalyzer::Normal);
  Colors[Analyzer.typeString(toSyntaxAnalyzer::String)]=Analyzer.getColor(toSyntaxAnalyzer::String);
  Colors[Analyzer.typeString(toSyntaxAnalyzer::Error)]=Analyzer.getColor(toSyntaxAnalyzer::Error);

  SyntaxComponent->insertItem(Analyzer.typeString(toSyntaxAnalyzer::NormalBkg));
  SyntaxComponent->insertItem(Analyzer.typeString(toSyntaxAnalyzer::Comment));
  SyntaxComponent->insertItem(Analyzer.typeString(toSyntaxAnalyzer::CurrentBkg));
  SyntaxComponent->insertItem(Analyzer.typeString(toSyntaxAnalyzer::ErrorBkg));
  SyntaxComponent->insertItem(Analyzer.typeString(toSyntaxAnalyzer::Keyword));
  SyntaxComponent->insertItem(Analyzer.typeString(toSyntaxAnalyzer::Normal));
  SyntaxComponent->insertItem(Analyzer.typeString(toSyntaxAnalyzer::String));
  SyntaxComponent->insertItem(Analyzer.typeString(toSyntaxAnalyzer::Error));

  Example->setAnalyzer(Analyzer);
  Example->setReadOnly(true);
  Example->setText(
"create procedure CheckObvious as
begin
  GlobeCom:='Great'; -- This variable doesn't exist
  if GlobeCom = 'Great' then
    Obvious(true);
  end if;
end;");
  Example->setCurrent(4);
  map<int,QString> Errors;
  Errors[2]="Unknown variable";
  Example->setErrors(Errors);

  Current=NULL;
}

void toSyntaxAnalyzer::readColor(const QColor &def,infoType typ)
{
  QString str=typeString(typ);
  QString conf(CONF_COLOR);
  conf+="\\";
  conf+=str;
  QString res=toTool::globalConfig(conf,"");
  if (res.isEmpty())
    Colors[typ]=def;
  else {
    int r,g,b;
    if (sscanf(res,"%d,%d,%d",&r,&g,&b)!=3)
      throw QString("Wrong format of color in setings");
    QColor col(r,g,b);
    Colors[typ]=col;
  }
}

toSyntaxAnalyzer::infoType toSyntaxAnalyzer::typeString(const QString &str)
{
  if(str=="Normal")
    return Normal;
  if(str=="Keyword")
    return Keyword;
  if(str=="String")
    return String;
  if(str=="Unfinished string")
    return Error;
  if(str=="Comment")
    return Comment;
  if(str=="Error background")
    return ErrorBkg;
  if(str=="Background")
    return NormalBkg;
  if(str=="Current background")
    return CurrentBkg;
  throw QString("Unknown type");
}

QString toSyntaxAnalyzer::typeString(infoType typ)
{
  switch(typ) {
  case Normal:
    return "Normal";
  case Keyword:
    return "Keyword";
  case String:
    return "String";
  case Error:
    return "Unfinished string";
  case Comment:
    return "Comment";
  case ErrorBkg:
    return "Error background";
  case NormalBkg:
    return "Background";
  case CurrentBkg:
    return "Current background";
  }
  throw QString("Unknown type");
}

void toSyntaxAnalyzer::updateSettings(void)
{
  const QColorGroup &cg=qApp->palette().active();
  readColor(cg.base(),NormalBkg);
  readColor(Qt::darkRed,ErrorBkg);
  readColor(Qt::darkGreen,CurrentBkg);
  readColor(Qt::blue,Keyword);
  readColor(cg.text(),Normal);
  readColor(Qt::red,String);
  readColor(Qt::red,Error);
  readColor(Qt::green,Comment);
}

void toSyntaxSetup::selectFont(void)
{
  bool ok=true;
  QFont font;
  font.setRawName(Text);
  font=QFontDialog::getFont (&ok,font,this);
  if (ok) {
    Text=font.rawName();
    CodeExample->setFont(font);
    Example->setFont(font);
  }
}

void toSyntaxSetup::changeLine(QListBoxItem *item)
{
  Current=item;
  if (Current) {
    QColor col=Colors[Current->text()];
    ExampleColor->setBackgroundColor(col);
  }
}

void toSyntaxSetup::selectColor(void)
{
  if (Current) {
    QColor col=QColorDialog::getColor(Colors[Current->text()]);
    if (col.isValid()) {
      Colors[Current->text()]=col;
      ExampleColor->setBackgroundColor(col);
      Example->analyzer().Colors[toSyntaxAnalyzer::typeString(Current->text())]=col;
      Example->repaint();
    }
  }
}

void toSyntaxSetup::saveSetting(void)
{
  toTool::globalSetConfig(CONF_TEXT,Text);
  toTool::globalSetConfig(CONF_HIGHLIGHT,SyntaxHighlighting->isChecked()?"Yes":"");
  toTool::globalSetConfig(CONF_KEYWORD_UPPER,KeywordUpper->isChecked()?"Yes":"");
  for (map<QString,QColor>::iterator i=Colors.begin();i!=Colors.end();i++) {
    QString str(CONF_COLOR);
    str+="\\";
    str+=(*i).first;
    QString res;
    res.sprintf("%d,%d,%d",
		(*i).second.red(),
		(*i).second.green(),
		(*i).second.blue());
    toTool::globalSetConfig(str,res);
  }
  toDefaultAnalyzer().updateSettings();
}
