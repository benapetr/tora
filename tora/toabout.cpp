//***************************************************************************
/*
 * TOra - An Oracle Toolkit for DBA's and developers
 * Copyright (C) 2000 GlobeCom AB
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
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
 *      Qt/UNIX or Qt/Windows products of TrollTech. And you are not
 *      permitted to distribute binaries compiled against these libraries
 *      without written consent from GlobeCom AB. Observe that this does not
 *      disallow linking to the Qt Free Edition.
 *
 * All trademarks belong to their respective owners.
 *
 ****************************************************************************/

TO_NAMESPACE;

#include <stdio.h>

#include <qtextview.h>
#include <qpushbutton.h>
#include <qsizepolicy.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qprogressbar.h>

#include "toabout.h"
#include "toconf.h"

#include "toabout.moc"

#include "LICENSE.h"

#include "icons/largelogo.xpm"

static const char *AboutText="<IMG SRC=largelogo.xpm><BR>\n"
"Version %s\n"
"<P>\n"
"&copy; 2000 copyright of GlobeCom AB\n"
"(<A HREF=http://www.globecom.se>http://www.globecom.se/</a>).<P>\n"
"Written by Henrik Johnson.<P>\n"
"This program is available\n"
"under the GNU Public License.<P>\n"
"Uses the Qt library version 2.2 by TrollTech\n"
"(<A HREF=http://www.troll.no>http://www.troll.no/</A>).<P>\n"
"\n"
"<HR BREAK=ALL>\n"
"This program is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"GNU General Public License for more details.";

static const char *QuoteText="<H3>People who think they know everything tend to irritate those of us who do.</H3>\n"
"<DIV ALIGN=RIGHT>Oscar Wilde<DIV ALIGN=LEFT>\n"
"<H3>My taste is simple, the best.</H3>\n"
"<DIV ALIGN=RIGHT>Oscar Wilde<DIV ALIGN=LEFT>\n"
"<H3>Working weeks come to its end, party time is here again.</H3>\n"
"<DIV ALIGN=RIGHT>Come with me, Depeche Mode<DIV ALIGN=LEFT>\n"
"<H3>I want to run<BR>I want to hide<BR>I want to tear down the walls<BR>\n"
"That hold me inside</H3>\n"
"<DIV ALIGN=RIGHT>Where the streets have no name, U2<DIV ALIGN=LEFT>\n"
"<H3>Got a letter from the government the other day.<BR>\n"
"Opened it and read it. It said they were suckers.</H3>\n"
"<DIV ALIGN=RIGHT>Black steel, Tricky<DIV ALIGN=LEFT>\n"
"<H3>It's a small world and it smells funny<BR>\n"
"I'd buy another if it wasn't for the money</H3>\n"
"<DIV ALIGN=RIGHT>Vision Thing, Sisters of Mercy<DIV ALIGN=LEFT>\n"
"<H3>I'm a 21:st century digital boy<BR>\n"
"I don't know how to live, but I got a lot of toys</H3>\n"
"<DIV ALIGN=RIGHT>21:st century digital boy, Bad Religion\n"
"<DIV ALIGN=left>\n"
"<H3>When will the world listen to reason?<BR>\n"
"When will the truth come into season?<BR>\n"
"I have a feeling it will be a long time.</H3>\n"
"<DIV ALIGN=RIGHT>It'll be a long time, Offspring\n"
"<DIV ALIGN=left>\n"
"<H3>Happiness isn't something you experience, it's something you remember.</H3>\n"
"<DIV ALIGN=RIGHT>Oscar Levant\n"
"<DIV ALIGN=LEFT>\n"
"<H3>I didn't believe in reincarnation in any of my other lives.<BR>\n"
"I don't see why I should have to believe in it in this one.</H3>\n"
"<DIV ALIGN=RIGHT>Strange de Jim\n"
"<DIV ALIGN=LEFT>\n"
"<H3>If a man has a strong faith he can indulge in the luxury of skepticism.</H3>\n"
"<DIV ALIGN=RIGHT>Friedrich Nietzsche\n"
"<DIV ALIGN=LEFT>\n"
"<H3>I'm a firm believer in the concept of a ruling class,<BR>\n"
"especially since I rule.</H3>\n"
"<DIV ALIGN=RIGHT>The movie \"Clerks\"\n"
"<DIV ALIGN=LEFT>\n"
"<H3>Stay alert! Trust no one! Keep your laser handy!</H3>\n"
"<DIV ALIGN=RIGHT>Roleplaying game Paranoia\n"
"<DIV ALIGN=LEFT>\n"
"<H3>The best definition of a gentleman is a man who can play the accordion -- but doesn't.</H3>\n"
"<DIV ALIGN=RIGHT>Tom Crichton\n"
"<DIV ALIGN=LEFT>\n"
"<H3>The trouble with being punctual is that nobody's there to appreciate it.</H3>\n"
"<DIV ALIGN=RIGHT>Franklin P. Jones\n"
"<DIV ALIGN=LEFT>\n"
"<H3>Only two things are infinite, the universe and human stupidity, and I'm not sure about the former.</H3>\n"
"<DIV ALIGN=RIGHT>Albert Einstein\n"
"<DIV ALIGN=LEFT>\n"
"<H3>If there's no light at the end of the tunnel, get down there and light the darn thing yourself!</H3>\n"
"<DIV ALIGN=RIGHT>Lauri Watts\n"
"<DIV ALIGN=LEFT>\n"
"<H3>Unix is the most user friendly system I know, the point is the it is really selective about who is indeed its friend.</H3>\n"
"<DIV ALIGN=RIGHT>Luigi Genoni\n"
"<DIV ALIGN=LEFT>\n"
"<H3>The difference between theory and practice, is that in theory, there is no difference between theory and practice.</H3>\n"
"<DIV ALIGN=RIGHT>Richard Moore\n"
"<DIV ALIGN=LEFT>\n"
"<H3>Your mind is like a parachute. It works best when open.</H3>\n"
"<DIV ALIGN=RIGHT>Matthias Elter\n"
"<DIV ALIGN=LEFT>\n"
"<H3>I might disagree with what you have to say,\n"
"but I'll defend your right to say it to the death.</H3>\n"
"<DIV ALIGN=RIGHT>Voltaire\n"
"<DIV ALIGN=LEFT>\n"
"<H3>Software is like sex, it is better when it is free.</H3>\n"
"<DIV ALIGN=RIGHT>Linus Torvalds\n"
"<DIV ALIGN=LEFT>\n"
"<H3>The 3 great virtues of a programmer:<BR>\n"
"Laziness, Impatience, and Hubris.</H3>\n"
"<DIV ALIGN=RIGHT>Larry Wall\n"
"<DIV ALIGN=LEFT>\n"
"<H3>Life is a sexually transmittable desease with terminal outcome.</H3>\n"
"<DIV ALIGN=RIGHT>Unknown\n"
"<DIV ALIGN=LEFT>\n"
"<H3>If you don't know where you are going you are never lost.</H3>\n"
"<DIV ALIGN=RIGHT>Unknown\n"
"<DIV ALIGN=LEFT>\n"
"<H3>If you're right 90% of the time, why quibble about the remaining 3%?</H3>\n"
"<DIV ALIGN=RIGHT>Unknown\n"
"<DIV ALIGN=LEFT>\n"
"<H3>Hiroshima '45 Chernobyl '86 Windows '95</H3>\n"
"<DIV ALIGN=RIGHT>Unknown\n"
"<DIV ALIGN=LEFT>\n"
"<H3>Accepptance testing: An unsuccessfull attempt to find bugs.</H3>\n"
"<DIV ALIGN=RIGHT>Unknown\n"
"<DIV ALIGN=LEFT>\n"
"<H3>Jag spenderade mina pengar p� sprit kvinnor och s�ng.<BR>\n"
"Resten sl�sade jag bort.</H3>\n"
"<DIV ALIGN=RIGHT>Unknown Swede\n"
"<DIV ALIGN=LEFT>\n"
"<H3>\n"
"<UL>\n"
"<LI>Make it idiot proof, and someone will make a better idiot.<P>\n"
"<LI>I don't suffer from insanity. I enjoy every minute of it.<P>\n"
"<LI>Press any key to continue or any other key to quit<P>\n"
"<LI>The box said \"Requires Windows 95 or better.\" So I installed LINUX<P>\n"
"<LI>&lt;-- Your information went that way --&gt;<P>\n"
"<LI>Go away, or I will replace you with a very small shell script.<P>\n"
"<LI>Do not meddle in the affairs of sysadmins, for they are suddle and quick to anger.\n"
"</UL>\n"
"</H3>\n"
"<DIV ALIGN=RIGHT>Think Geek (http://www.thinkgeek.com/)\n"
"<DIV ALIGN=LEFT>\n"
"<H3>\n"
"<UL>\n"
"<LI>Luck can't last a lifetime unless you die young!<P>\n"
"<LI>Misfortune: While good fortune often eludes you, this kind never misses.<P>\n"
"<LI>For every winner, there are dozens of losers. Odds are you are one of them.<P>\n"
"<LI>Failure: When your best just isn't good enough.<P>\n"
"<LI>Quitters never win, winners never quit, but those who never win and never quit are idiots.<P>\n"
"<LI>If you can't learn to do something well, learn to enjoy doing it poorly.<P>\n"
"<LI>If at first you don't succeed, failure may be your style.<P>\n"
"<LI>There are no stupid questions, but there are a lot of inquisitive idiots.<P>\n"
"<LI>There is no joy greater than soaring high on the wings of your dreams,\n"
"	except mayby the joy of watching a dreamer who has nowhere to land but in the ocean of reality.<P>\n"
"<LI>The only consistent feature of all of your dissatisfying relationships is you.<P>\n"
"<LI>When you earnestly believe you can compensate for a lack of skill by doubling your efforts,\n"
"	there's no end to what you can't do.<P>\n"
"<LI>Never underestimate the power of stupid people in large groups.<P>\n"
"<LI>The downside of being better than everyone else is that people tend to assume you're pretentious.<P>\n"
"<LI>In the battle between you and the world, bet on the world.<P>\n"
"<LI>You'll always miss 100% of the shots you don't take, and, statistically speaking, 99% of the shots you do.<P>\n"
"<LI>The secret to success is knowing who to blame for your failures.<P>\n"
"<LI>When birds fly in the right formation, they need only exert half the effort.\n"
"	Even in nature, teamwork results in collective laziness.<P>\n"
"<LI>It's amazing how much easier it is for a team to work together when no one has any idea where they're going.<P>\n"
"<LI>Attitudes are contagious. Mine might kill you.<P>\n"
"<LI>When people are free to do as they please, they usually imitate each other.<P>\n"
"<LI>If we don't take care of the customer, mayby they'll stop bugging us.<P>\n"
"<LI>It takes months to find a customer, but only seconds to lose one.\n"
"	The good news is we should run out of them in no time.<P>\n"
"<LI>Mediocrity: It takes a lot less time, and most people won't notice the difference until it's too late.<P>\n"
"<LI>Laziness: Success is a jurney, not a destination. So stop running.<P>\n"
"<LI>Hard work often pays off after time, but laziness always pays off now.<P>\n"
"<LI>The tallest blade of grass is the first to be cut by the lawnmower.\n"
"</UL>\n"
"</H3>\n"
"<DIV ALIGN=RIGHT>Despair INC (http://www.despair.com/)\n"
"<DIV ALIGN=LEFT>";


#define ABOUT_CAPTION "TOra %s"

static QPixmap *toLogoPixmap=NULL;

void toAllocLogo(void)
{
  if (!toLogoPixmap) {
    toLogoPixmap=new QPixmap((const char **)largelogo_xpm);
    QMimeSourceFactory::defaultFactory()->setPixmap("largelogo.xpm",*toLogoPixmap);
  }
}

toSplash::toSplash(QWidget *parent,const char *name,WFlags f)
  : QVBox(parent,name,f)
{
  toAllocLogo();

  setBackgroundColor(white);
  QLabel *logo=new QLabel(this,"Logo");
  logo->setBackgroundColor(white);
  logo->setPixmap(*toLogoPixmap);
  Label=new QLabel("Loading plugins",this);
  Label->setBackgroundColor(white);
  Progress=new QProgressBar(this,"Progress");

  QWidget *d=QApplication::desktop();
  move((d->width()-width())/2,(d->height()-height())/2);
}

toAbout::toAbout(QWidget* parent,const char* name,bool modal,WFlags fl)
  : QDialog(parent,name,modal,fl)
{
  toAllocLogo();

  TextView=new QTextView(this);
  TextView->setTextFormat(RichText);
  TextView->setPaper(QBrush(QColor(0xFA,0xFA,0xFE)));
  resize(640,340);
  setMinimumSize(640,340);
  setMaximumSize(640,340);
  TextView->setGeometry(QRect(10,10,620,280));
  TextView->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
  
  ChangeButton=new QPushButton(this,"ChangeButton");
  ChangeButton->setGeometry(QRect(144,300,104,32)); 

  QPushButton *OkButton=new QPushButton(this,"OkButton");
  OkButton->setGeometry(QRect(392,300,104,32)); 
  OkButton->setText(tr("&Ok"));
  OkButton->setDefault(true);

  if (!parent) {
    Page=1;
    changeView();
    QPushButton *CancelButton=new QPushButton(this,"CanceButton");
    CancelButton->setGeometry(QRect(506,300,104,32)); 
    CancelButton->setText(tr("&Cancel"));
    connect(CancelButton,SIGNAL(clicked()),SLOT(reject()));
  } else {
    char buffer[10000];
    sprintf(buffer,ABOUT_CAPTION,TOVERSION);
    setCaption(buffer);
    sprintf(buffer,AboutText,TOVERSION);
    TextView->setText(buffer);
    TextView->setPaper(QColor(227,184,54));
    ChangeButton->setText(tr("&Quotes"));
    Page=0;
  }

  connect(ChangeButton,SIGNAL(clicked()),SLOT(changeView()));
  connect(OkButton,SIGNAL(clicked()),SLOT(accept()));
}

void toAbout::changeView(void)
{
  switch (Page) {
  case 0:
    ChangeButton->setText(tr("&License"));
    TextView->setText(QuoteText);
    setCaption("Favourite Quotes");
    TextView->setPaper(QColor(255,255,255));
    Page++;
    break;
  case 1:
    ChangeButton->setText(tr("&About"));
    TextView->setText(LicenseText);
    TextView->setPaper(QColor(255,255,255));
    setCaption("License");
    Page++;
    break;
  default:
    char buffer[10000];
    sprintf(buffer,ABOUT_CAPTION,TOVERSION);
    setCaption(buffer);
    sprintf(buffer,AboutText,TOVERSION);
    TextView->setText(buffer);
    TextView->setPaper(QColor(227,184,54));
    ChangeButton->setText(tr("&Quotes"));
    Page=0;
    break;
  }
}
