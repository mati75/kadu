/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qbitmap.h>
#include <qpainter.h>
#include <qlayout.h>
#include "../config.h"

//
#include "kadu.h"
#include "about.h"
//

static const char * shade_sun_xpm[] = {
"167 122 17 1",
" 	c None",
".	c #020202",
"+	c #565202",
"@	c #525252",
"#	c #661306",
"$	c #4E4A02",
"%	c #464202",
"&	c #3F3D0D",
"*	c #33311B",
"=	c #2A2A12",
"-	c #26250E",
";	c #664C02",
">	c #201E10",
",	c #663802",
"'	c #4A4A4A",
")	c #665E02",
"!	c #191602",
".......................................................................................................................................................................",
".......................................................................................................................................................................",
".......................................................................................................................................................................",
".......................................................................................................................................................................",
".......................................................................................................................................................................",
"..........................................................................@@...........................................................................................",
"..........................................................................@@@..........................................................................................",
"...........................................................................@@@@........................................................................................",
"...........................................................................@@@@@..............................................@........................................",
"............................................................................@@@@@............................................@@........................................",
"............................................................................@@'@@@..........................................@@@........................................",
".............................................................................@@@@@..........................................@@@........................................",
".............................................................................@@@'@@........................................@@'@'.......................................",
"..............................................................................@@@@@@.......................................@@@@@.......................................",
"...............................................................................@@@@@......................................@@@@@@.......................................",
"...............................................................................@'@@@@....................................@@@@-@@.......................................",
"................................................................................@@'@@....................................@@'@@@........................................",
"................................................................................@@@@@@..................................@@@@'@@........................................",
"................................................................................@@@@@@..................................@@@@@@.........................................",
".................................................................................@'@@@.................................'@@@@@@.........................................",
".......................................@@@@@.....................................@@@'@................................@@@'@@@@.........................................",
"........................................@@@@@@@..................................@@@@@................................@@@@'@@..........................................",
"........................................@@'@@@@@@@................................@@@................................@@@@@@@...........................................",
"..........................................@'@@@@@@@................................@.................................@@@@@@@...........................................",
"...........................................@@'@'@@@@@..............................@................................@@'@'@@............................................",
"............................................@@@@@@@@@@..............................................................@@@@@@@............................................",
"..............................................@@@'@'@@@@............................................................@@@@@@@............................................",
"...............................................@@@@@@@@@@..........................................................@@@@'@@.............................................",
"................................................@@@@@'@@@@.....................@@@@@@-@@@@@@@@@@@@................@@'@@@@@.............................................",
"..................................................'@@@@@@@@...............@@@@@@@@@@@@@@@@@@@@@@@@@@@@............@@@@@@@..............................................",
"...................................................@@@@'@@@@..........@@@@@@@@@@'@'@'@@'@'@'@'@'@@@@@@@@...........@@@@'@..............................................",
"....................................................'@@@@@@@........@@@@@'@'@'@@@@@@@)))))))))))'@'@@@@@@@.........@'@@@@.........................................@@@..",
".....................................................@@@@'@@.....@@@@@@'@@@@@@+)))))))))))))))))))))'@'@@@@@.......@@@@@........................................@@@@@..",
"......................................................'@@@@@...@@@@@'@@@@@))))))))))))))))))))))))))))))@@@@@@......@@@@......................................@@@'@@...",
".......................................................@@@@@@@@@@@'@@@@)))))))))))))))))))))))))))))))))))'@@@@.....@@'......................................@@@@@@@...",
"........................................................'@@@@'@'@@@@')))))))))))))))))))))))))))))))))))))))@@@@@....@.....................................@@'@@@'*....",
".........................................................@@'@@@@@@)))))))))))))))))))))))))))))))))))))))))))'@@@@.......................................@@@@@@@@@.....",
".......................................................@'@@@@@@')))))))))))))))))))))))))))))))))))))))))))))))@@@@....................................@@@@@@'@@@......",
"......................................................@@@@@@@@@)))))))))))))))))))))))))))))))))))))))))))))))))'@@@.................................@@@@@'@@@@'.......",
"...................................................@@@@@@@@')))))))))))))@@@@))))))))))))))))))))))))))))))))))))@@@@..............................@@@'@@*@@@@@@.......",
"..................................................@@@@@@@'))))))))))))@@@@@@@@@@))))))))))))))))))))))))))))))))))'@@@..........................@@@@@@@@@@@@@@@........",
".................................................@'@'@'@@)))))))))))@@@@@@@@@@@@@))))))))))))))))))))))))))))))))))@@@@........................@@@'@@@@@@@@'@@@@.......",
"...............................................@@@@@@@@))))))))))@@@@@@'@@@'@@@@@@@)))))))))))))))))))))))))))))))))'@@@....................@@@@@@@@'@=@@'@@@@@........",
"..............................................@@@@@@@@)))))))))@@@@@@@@@@@))))))@@@@)))))))))))))))))))))))))))))))))@@@..............@..@@@'@'@@@@@@@@@*@.............",
".............................................@@@@@@@))))))))))@@@@@@'@@))))))))))@@@)))))))))))))))))))))))))))))))))'@@@..........@@@@@@@@@@@@@'@@@@@@@@..............",
"............................................@@'@'@')))))))))@@@'@@@@@))))))))))))@@@))))))))))))))))))))))))))))))))))@@@@.........@=@@@'@'@@@@@@&@'@'@@@..............",
"...........................................@@@@@@@)))))))))@@@@@@@@))))))))))))))@@@)))))))))))))))))))))))))))))))))))'@@.........@@@'@@@@@'@'@@@@@@..................",
"..........................................@@@@@@'))))))))@@@@@@@@)))@))))))))))))@@@)))))))))))))@@@)))))))))))))))))))@@@@........@@@@@@@@@@@@'@@@@@..................",
"..........................................@@'@@)))))))))@@@@@@@))+)@@)@))))))))))@@@))))))))))))@@@@@@))))))))))))))))))'@@.........@@@@@@@@@@@@@'@....................",
".........................................@@@@@+)))))))@@@@@@@')))))@@@@@))))))))))@@))))))))))))@@@@@@))))))))))))))))))@@@@........@@'@'@@@@@@.@@.....................",
"........................................@@'@@$)))))))@@@@@@@)))))))@@@@@@)))))))))))))))))))))))@@@@@@@)))))))))))))))))'@@@.........@@@@@'@''.........................",
".......................................@@@@@')))))))@@@@@@'))))))))@@@@@@)))))))))))))))))))))))@@@@@@@))))))))))))))))))@@@..........@@@@@@@..........................",
".......................................@@@@@$))))))@@@@'@@)))))))))@@'@@@)))))))))))))))))))))))@@@@@@@))))))))))))))))))'@@@..........@.@.............................",
"......................................@@@'@>))))))@'@@@'@))))))))))@@@@@@)))))))))))))))))))))))@@@@@@@))))))))))))))))))@@@@..........................................",
"......................................@@@@.*))))))@@@@@@)))))))))))@@@@@@)))))))))))))))))))))))@@@'@@))))))))))))))))))))'@@..........................................",
".....................................'@'@..&)))))@@@'@@))))))))))))@@@@@@)))))))))))))))))))))))@@@@@@))))))))))))))))))))@@@..........................................",
"......................................@@...&))))@@@@@@)))))))))))))@@'@@@))))))))))))))))))))))))@@@@@))))))))))))))))))))'@@@.........................................",
"......................................@....&)))@@@@@@))))))))))))))@@@@@@)))))))))))))))))))))))+@@@@@))))))))))))))))))))@@@@.........................................",
"...........................................*))@@@@@@)))))))))))))))@'@@@@))))))))))))))))))))))))@@@@@)))))))))))))))))))@'@@@.........................................",
"...........................................=))@@@'@@))))))))))))))))@@@@)))))))))))))))))))))))))@@@@@)))))))))))))))))))@@@@@.........................................",
"...........................................!+@@@@@@))))))))@@@@)))))@@@@)))))))))))))))))))))))))@@@@@)))))))))))))))))))@'@'@.........................................",
"............................................&@@@@@)))))))))@@@@@)))))@@@)))))))))))))))))))))))))'@@@@)))))))))))))))))))@@@@@.........................................",
"............................................@@'@@@)))))))))@@@@@@)))))@)))))))))))))))))))))))))))@@@@)))))))))))))))))))@'@@..........................................",
"...........................................@@@@@@)))))))))@@@@@@@@))))))))))))))))))))))))))))))))@@@))))))))))))))))))))@@@@..........................................",
"...........................................@@@@@)))))))))))@@@@@@@))))))))))))))))))))))))))))))))))@)))))))))))))))))))@@@'@..........................................",
"...........................................@@'@@)))))))))))+@@@@@@))))))))))))))))))))))))))))))))))))))))))))))))))))))@@@@@..........................................",
"..........................................@@@@@))))))))))))))@@@@@@)))))))))))))))))))))))))))))))))))))))))))))))))))))@'@@@..........................................",
"..........................................@@@@@))))))))))))))@@@@@@@)))))))))))))))))))))))))))))@@))))))))))))))))))))@@@@@...........................................",
".........................................@@@'@@)))))))))))))))@@@@@@@)))))))))))))))))))))))))))@@@@)))))))))))))))))))@@@'@...........................................",
".........................................@@@@@@)))))))))))))))))@@@@@@@)))))))))))))))))))))))@@@@@@))))))))))))))))))@@@@@............................................",
"..............................@@@@@@.....@@@@@+))))))))))))))))))@@@@@@@)))))))))))))))))))))@@@@@@))))))))))))))))))$@'@@'............................................",
"...........................@>@@@@@@@@@...'@@@@')))))))))))))))))))@@@@@@@@))))))))))))))))@@@@@@@@@)))))))))))))))))+@@@@@@............................................",
".........................@@@'@@'@'@@@@@..@@@@.%))))))))))))))))))))@@@@@@@@@))))))))@@@@@@@@@@@@@@))))))))))))))))))@@@@@@.............................................",
"........................@@@@@@@@@@'@@@@.@@@@@!')))))))))))))))))))))@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@)))))))))))))))))*@@@@@@.............................................",
"......................@@@@'@@@@@@@@@'@@.@@@'@.%))))))))))))))))))))))@@@@@@@@@@@@@@@@@@@@@@@@@@@))))))))))))))))))''@@'@@..............................................",
"....................@@@'-@@'@@@@@@@@@@@.@@@@@!+))))))))))))))))))))))@@@@@@@@@@@@@@@@@@@@@@@@@)))))))))))))))))))%@@@@@@...@@..........................................",
"...................@@@@@@@@@@'@'@@@@@@'.@@@@'.+))))))))))))))))))))))@@@@@@@@@@@@@@@@@@@@@@)))))))))))))))))))))&@@@@'@'..@@@@@........................................",
"..................@'@@@@@@@@@@@@.....@...@'@@!@))))))))))))))))))))))@@@@@@@***@@@@@@-----*)))))))))))))))))))+@@@@@@@@..@@@@@@@.......................................",
".................@@@@'@@'@@@@............@@@'.))))))))))))))))))))))))@@@@@@=##-=-=#######%))))))))))))))))))@-@'@@@@@...@@'@'@@@......................................",
"................@@@@@@@@@@@..............@@@@!@)))))))))))))))))))))))@@@@@@##############+)))))))))))))))))&@@@@@@'@.....@@@@@@@@.....................................",
"...............@@@@@@@@@.@...............@@@'@$))))))))))))))))))))))))@@@@@@############@@@)))))))))))))))'@@@@@'@@......@@@@@'@@@....................................",
"..............@'*@'@'@@...................@@@@&))))))))))))))))))))))))@@@@''###########*@@@))))))))))))))&@@@@@@@@.......@@'@@@@@@@@..................................",
".............@@@@@@@@@'...................@@@@@+)))))))))))))))))))))))@@@@@@'########''@@@@)))))))))))))@@@'@@@@@.........@@@@@-@@@@@.................................",
"............@@@@@@@@@@.....................@@@@&))))))))))))))))))))))))@@@@''#######@@@@@@))))))))))))@@@@@@@'@@...........'@@@@'@@@@@................................",
"...........@@@@@'@'@@......................@@'@@*)))))))))))))))))))))))@@@@@'######''@@@@)))))))))))@@@@@@@@@@@.............@@@@@@'@@@@...............................",
"...........'@'@@@@@@........................@@@@@&)))))))))))))))))))))))@@@'@&####''@@@@@)))))))))@@@@@@@@@@@................@@@@@@@@@@@..............................",
"..........@@@@@@@@...........................@@@@@>')))))))))))))))))))))@@@@@@'##''@@@@@))))))))@@@@@@@@@@@@...................@@@@@'@@@@.............................",
"..........@@@@@@@@............................@@@'@>&)))))))))))))))))))))@@@@''''@@@@@@)))@@@@@@@@@@@@@@@@......................@'@@@@@@@@............................",
".........@@@@@'@'.............................@@@@@@@@@))))))))))))))))))))'@@@@@@@@@@@@'@@@@@@@@@@@@@@@@.........................@@@@@'@@@@...........................",
".........@@'@@.@................................@@@@@@@@'@))))))))))))))))))@@@@@'@@'@@@@@@@@@@@@@@@@@@............................@'@@@@@@@@..........................",
".........@@@@....................................@@@@@@@@@@=*&'@+@$'$&'&%$&&%&@@@@@@@@@@@@@@@@@@@@@@!................................@@@@'@@>..........................",
".........@@@......................................!@@@@@@@@@@'@@@@.!..@.@.&@@@'@@'@@@@@@@@@@@@@@@@.....................................@@@'@@..........................",
".........@@@........................................!@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'@@@@@@@@@@@!!.......................................@@@@@@.........................",
".........'@@.........................................!.!@@@@@@'@@@@@@@@@@@@'@@@@@@@@@@@@@!!!!.............................................@@@..........................",
".........@@...............................................@@@@@@@@@@@@@@@@@@@@@@'@@@@@.................................................................................",
".........@@...................................................@@'@@'@@@@'@@'@'@........................................................................................",
"....................................................@@@@@..............................................................................................................",
"...................................................@@@@@@..............................................................................................................",
"..................................................@@@@@@@@.............................................................................................................",
"................................................@@'@'@@@@@...............................@@@...........................................................................",
"...............................................@@@@@@@@@@...............................@@@@@..........................................................................",
"...............................................@@@@@'@@@@...............................'@@@@@.........................................................................",
"..............................................@@'@@@@@@@'...............................@@'@@@.........................................................................",
"..............................................@@@'@@'@'@................................@@@@@@.........................................................................",
".............................................@@@@@@@@....................................@@@'@.........................................................................",
".............................................@@@@@@@.....................................'@@@@.........................................................................",
"............................................@@'@@@@......................................@@@@@@........................................................................",
"............................................@@@@'@@......................................@@@@@@........................................................................",
"...........................................@@@@@@@.......................................@@'@'@........................................................................",
"...........................................'@@@@@.........................................@@@@@........................................................................",
"...........................................@@'@@..........................................@@@@@@.......................................................................",
"...........................................@@@@@..........................................@@@@@@.......................................................................",
"...........................................@@@@............................................@@'@@@......................................................................",
"...........................................'@@.............................................@@@@@@@.....................................................................",
"...........................................@@...............................................@@@'@@.....................................................................",
"..........................................@@@...............................................@@@@@@@....................................................................",
"...........................................@.................................................@@@@@@@...................................................................",
".............................................................................................@@'@@@@@..................................................................",
"..............................................................................................@@@'@@@@@................................................................",
"...............................................................................................@@@@@@@.................................................................",
".................................................................................................@@'@..................................................................",
"......................................................................................................................................................................."};


QString aboutText;

About::About() : QDialog(0) {
    setCaption(i18n("About"));

aboutText.append("<CENTER><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR>"
"<H1><FONT COLOR=\"#ffd326\">Kadu ");
aboutText.append(PACKAGE_VERSION);
aboutText.append(__c2q("</FONT></H1><BR><BR>"
"bazuje na KDE3, QT3, libgadu<BR><BR>"
"<B>Ludzie:</B><BR>"
"Tomek \"tomee\" Jarzynka<BR>"
"tomee (at) cpi (dot) pl<BR><BR>"
"Tomasz Chili�ski<BR>"
"chilek (at) chilan (dot) com<BR><BR>"
"Adrian Smarzewski<BR>"
"adrians (at) aska (dot) com (dot) pl<BR><BR>"
"Dariusz Jagodzik<BR>"
"mast3r (at) kadu (dot) net<BR><BR>"
"Roman Krzystyniak<BR>"
"Ron_K (at) tlen (dot) pl<BR><BR>"
"<B>a tak�e:</B><BR>"
"Arek Mi�kiewicz<BR>"
"Krzysztof Godlewski<BR>"
"Mariusz Witkowski<BR>"
"Mateusz Papiernik<BR>"
"Artur Zabro�ski<BR>"

"i inni...<BR><BR><BR>"
"Kadu powsta�o w sierpniu 2001 roku, "
"jako ma�a wprawka do programowania "
"w KDE, na bazie libgg.so Wojtka Kaniewskiego. "
"Z czasem program przekszta�ci� si� "
"w jeden z najpopularniejszych klient�w "
"GG dla Linuxa...<BR><BR><BR><BR>"
"Pami�taj, Kadu to darmowe oprogramowanie. Autorzy po�wi�caj� sw�j prywatny czas "
"na rozw�j aplikacji. Miej to na wzgl�dzie, prosz�c ich o co� lub mieszaj�c z b�otem."
"</CENTER>"));


    posy = 0;

    QGridLayout *g = new QGridLayout(this,1,1,10,4);

    doc = new QSimpleRichText(aboutText,QFont("Arial",10));
    doc->setWidth(195);
    m_pfield = new QLabel(this);
    g->addWidget(m_pfield,0,0);

    m_pBackground = new QPixmap(doc->width(), doc->height());
    m_pBackground->fill(Qt::black);

    QPainter p(m_pBackground);

    QPixmap * m_ptmpSun = new QPixmap((const char **)shade_sun_xpm);
    m_pPixmap = new QPixmap(200,200);
    m_pPixmap->fill(Qt::black);
    bitBlt(m_pPixmap, 17, 39, m_ptmpSun, 0, 0, -1, -1);
    delete m_ptmpSun;

    QColorGroup cg = colorGroup();
    cg.setColor(QColorGroup::Text, QColor(255, 255, 255));

    doc->draw(&p, 0, 0, QRect(0,0,1000,20000), cg );
    m_pBackground->setMask(m_pBackground->createHeuristicMask(false));

    m_ppix_text = new QPixmap(200,200);
    m_pfield->setPixmap(*m_ppix_text);

    setFixedSize(sizeHint());

    timer = new QTimer(m_pfield);
    connect(timer,SIGNAL(timeout()),SLOT(scrollText()));
    
    timer->start(50, TRUE);

}

void About::scrollText() {
    posy++;
    repaint(false);

    if (posy <=  m_pBackground->height())
	timer->start(50, TRUE);
}


void About::paintEvent( QPaintEvent * e) {

    QPixmap tmp_pix(*m_pPixmap);
    QPainter p(&tmp_pix);
    bitBlt(&tmp_pix,0,0,m_pBackground,0,posy,200,200);

    p.end();
    bitBlt(m_pfield, 0, 0, &tmp_pix, 0, 0, 200, 200);
}

About::~About()
{
    delete m_ppix_text;
    delete m_pBackground;
    delete m_pPixmap;
    delete doc;
}

#include "about.moc"
