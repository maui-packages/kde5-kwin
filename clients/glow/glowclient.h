/***************************************************************************
                          glowclient.h  -  description
                             -------------------
    begin                : Thu Sep 6 2001
    copyright            : (C) 2001 by Henning Burchardt
    email                : h_burchardt@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GLOW_CLIENT_H
#define GLOW_CLIENT_H

#include <vector>
#include <map>
#include <kwin/client.h>
#include <kwin/kwinbutton.h>

class QPixmap;
class QBitmap;
class QTimer;
class QVBoxLayout;
class QSpacerItem;

namespace KWinInternal
{

class GlowButton;
class GlowButtonFactory;

//-----------------------------------------------------------------------------

class GlowClientConfig
{
public:
	GlowClientConfig();

	void load();

	QColor stickyButtonGlowColor;
	QColor helpButtonGlowColor;
	QColor iconifyButtonGlowColor;
	QColor maximizeButtonGlowColor;
	QColor closeButtonGlowColor;
};

//-----------------------------------------------------------------------------

class GlowClientGlobals : public QObject
{
	Q_OBJECT
public:
	enum PixmapType { StickyOn, StickyOff, Help, Iconify, MaximizeOn,
		MaximizeOff, Close };
	enum PixmapModifiers { Active=2, NotActive=4, PosLeft=8, PosRight=16,
		SizeNormal=32, SizeSmall=64 };

	static GlowClientGlobals *instance();

	~GlowClientGlobals();

	QPixmap *getPixmap(int type, bool isActive, bool isLeft, bool isSmall);
	std::vector<int> getPixmapTypes();
	std::vector<int> getPixmapModifiers();

	void reset();

	GlowClientConfig *config;
	GlowButtonFactory *buttonFactory;

private:
	static GlowClientGlobals *m_instance;

	GlowClientGlobals();

	void readConfig();
	void createPixmaps();
	void deletePixmaps();
	QPixmap *createPixmap(int type, int modifiers);

	std::map< int, std::map< int, QPixmap* > > m_pixmapMap;
};

//-----------------------------------------------------------------------------

class GlowClient : public KWinInternal::Client
{
	Q_OBJECT
public:
	GlowClient( KWinInternal::Workspace *ws, WId w,
		QWidget *parent=0, const char* name=0 );
	~GlowClient();

protected:
	virtual void resizeEvent( QResizeEvent * );
	virtual void paintEvent( QPaintEvent * );
	virtual void showEvent( QShowEvent * );
	virtual void mouseDoubleClickEvent( QMouseEvent * );
	virtual void captionChange( const QString& name );
	virtual void maximizeChange(bool m);
	virtual void activeChange(bool);
	virtual void iconChange();
	virtual void stickyChange(bool on);
	virtual MousePosition mousePosition(const QPoint &) const;

private:
	std::vector<GlowButton*> m_buttonList;
	std::vector<GlowButton*> m_leftButtonList;
	std::vector<GlowButton*> m_rightButtonList;
	GlowButton *m_stickyButton;
	GlowButton *m_helpButton;
	GlowButton *m_minimizeButton;
	GlowButton *m_maximizeButton;
	GlowButton *m_closeButton;
	QVBoxLayout *m_mainLayout;
	QSpacerItem *m_titleSpacer;

	void createButtons();
	void updateButtonPositions();
	/**
	 * Before this method is called we have to update the button
	 * positions with updateButtonPositions() because the pixmaps
	 * depend on the position
	 */
	void updateButtonPixmaps();
	void resetLayout();
	void doShape();
	bool isLeft(GlowButton *button);
	bool isRight(GlowButton *button);

protected slots:
	void slotReset();
	void slotMaximize(int button);
};

}; // namespace

#endif
