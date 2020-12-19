//======================================================================
//  File:        pclaf.h
//  Authors:     Timothy P. Justice, Yves Peneveyre
//  Created:     March 24, 1992
//  Revised:     April 5, 1995
//               Spring 1999 (adapt to Win-NT environment & Visual-C++ 5.0
//               & Borland C++ (5?)
//               September 2006 (adapt to VC++ 6,7,8)
//  Description: This file contains the interface of the
//               classes for the Little Application Framework.
//----------------------------------------------------------------------
//  Copyright (c) 1992, 1993, 1995 by Timothy P. Justice, Oregon State U.
//  Copyright (c) 1999, by Yves Peneveyre,
//                      Ecole d'Ingenieurs du Canton de Vaud, Switzerland
//  Copyright (c) 2006, by Doohun Eum,
//                      Duksung Women's Univ., Seoul, Korea
//
//  This file is part of LAF, the Little Application Framework.
//
//  Permission is hereby granted to everyone to copy and distribute
//  without fee verbatim copies of this file.  This copyright notice
//  must be retained on all copies.  No warranty expressed or implied
//  is made concerning this software.
//
//  Authors e-mail addresses: justict@cs.orst.edu
//                            Yves.Peneveyre@eivd.ch (www.eivd.ch)
//                            dheum@duksung.ac.kr
//======================================================================

#ifndef LAF_H
#define LAF_H

// Header Files

#include <WINDOWS.H>			// Microsoft Windows interface
#include <stdio.h>			// C++ standard i/o library
#include <stdlib.h>			// C++ standard library

// Definitions

#define MAXCONTROLS 100
#define MAXITEMS     20
#define MAXMENUS     20

#define MAKEPOINT(l) (*((POINT FAR *) & (l)))
#define M_PI 3.14

// Class Hierarchy: Little Application Framework

class  winostream;
class  window;
class  application;
class  childWindow;
class  menu;
class  control;
class  basicMenuItem;
class  basicButton;
class  editText;
class  staticText;
class  listBox;

// Other Types

enum streamBase {octBase, decBase, hexBase};

enum color
{
  black, blue, green, cyan, red, magenta, brown, gray,
  white, brightBlue, brightGreen, brightCyan, brightRed,
  brightMagenta, brightYellow, brightGray
};

enum lineStyle {solidLine, dashedLine, dottedLine, nullLine};

enum textAlign {leftText, centerText, rightText};

typedef HBITMAP bitmap;
typedef POINT point;

// --- Window Stream Class ---------------------------------------------
//
//  winostream is used to simulate the behavior of C++ output
//  streams in a window.
//----------------------------------------------------------------------
class winostream
{
public:
  winostream();
  winostream(window *);
  streamBase base() const;
  streamBase base(streamBase);
  int        precision() const;
  int        precision(int);
  int        width() const;
  int        width(int);
  window     *outputWindow();

  winostream   &operator << (char);
  winostream   &operator << (char *);
  winostream   &operator << (double);
  winostream   &operator << (float);
  winostream   &operator << (int);
  winostream   &operator << (long);
private:
  int        fieldWidth;
  int        floatPrecision;
  streamBase numberBase;
  window     *winptr;
};

// --- Window Class ----------------------------------------------------
//
//  Window is an abstract class that defines the behavior common
//  to all windows.
//----------------------------------------------------------------------
class window
{
public:
  // constructor
  window(char *title, int width, int height, int x, int y);

  // destructor
  virtual ~window();

   // add a control to the window
  void add(control *aControl);

 // add a menu to the window
  void add(menu *aMenu);

  // return the average character width
  int avgCharWidth();

  // return the y coordinate for the bottom of the window
  int bottom();

  void           cartesianOff();
  void           cartesianOn(color, lineStyle, int);
  void           circle(const int, const int, const int);
  void           circle(const POINT &, const int);
  void           checkMenuItem(int);
  void           clearAndUpdate();
  virtual void   command(int);
  virtual void   create();
  editText       createEditText(int, int, int, int, int,
                                textAlign = leftText, int = 1, char * = 0);
  listBox        createListBox(int, int, int, int, int, int = 1);
  staticText     createStaticText(int, int, int, int,
                                  textAlign = leftText, int = 0, char * = 0);
  virtual void   doubleClick(int, int);
  void           gridOn(color, lineStyle, int);
  void           gridOff();
  HWND           handle();
  int            height();
  HINSTANCE      instance();
  int            left();
  void           line(const int, const int, const int, const int);
  void           line(const POINT &, const POINT &);
  HMENU          menuHandle();
  virtual void   mouseDown(int, int);
  virtual void   mouseRDown(int, int);
  virtual void	 mouseMDown(int, int);
  virtual void   paint();
  void           point(const int, const int);
  void           polygon(POINT [], int);
  LRESULT WINAPI process(HWND, UINT, WPARAM, LPARAM);
  virtual void   quit();
  void           rectangle(const int, const int, const int, const int);
  int            right();
  virtual void   run();
  void           scrollToBottom();
  void           scrollToLeft();
  void           scrollToRight();
  void           scrollToTop();
  void           setBrush(color);
  void           setMaxX(int);
  void           setMaxY(int);
  void           setPen(color, lineStyle=solidLine, int=1);
  void           setTextColor(color);
  void           setTextPosition(int, int);
  int            show(int);
  virtual void   size(int, int);
  int            textHeight();
  int            textWidth(char *);
  void           textPosition(int &, int &);
  virtual void   timer();
  int            top();
  void           uncheckMenuItem(int);
  void           update();
  void           vector(const int, const int, const int, const int);
  int            width();

  void			 drawBitmap(bitmap,int,int,int,int);
  bitmap		 loadBitmap(int);
  bitmap		 loadBitmap(const char *);


  winostream       wout;

  // friends
  friend class winostream;
  friend class menu;

protected:
  void           createBrush();
  void           createPen();
  void           deleteBrush();
  void           deletePen();
  void           _initialize_();
  void           print(char *);

  COLORREF     brushColor;
  color        cartesianColor;
  lineStyle    cartesianStyle;
  int          cartesianWidth;
  int          charHeight;
  int          charWidth;
  int          clientX;
  int          clientY;
  control      *ctl[MAXCONTROLS];
  int          ctlCount;         
  HBRUSH       currentBrush;
  HPEN         currentPen;
  HDC          device;
  color        gridColor;
  lineStyle    gridStyle;
  int          gridWidth;
  HMENU        hmenu;
  int          hScrollPos;
  HWND         hwnd;
  int          initialHeight;
  int          initialWidth;
  int          initialX;
  int          initialY;
  POINT        lowerRight;
  int          maxHScroll;
  int          maxVScroll;
  int          maxX;
  int          maxY;
  TEXTMETRIC   metric;
  menu         *mnu[MAXMENUS];
  int          mnuCount;
  char         *name;
  POINT        nextPos;
  COLORREF     penColor;
  int          penStyle;
  int          penWidth;
  POINT        upperLeft;
  int          vScrollPos;
  HINSTANCE    prevInstance;
  POINT        prevPos;
  int          tabStop[10];
  COLORREF     textColor;
  HINSTANCE    thisInstance;
  char         *title;
};

// --- Application Class -----------------------------------------------
//
//  Application is the basic framework class.  It is a special kind
//  of window that is opened when the application is started.  By
//  itself, application is not very functional.  If you create an
//  instance of application and send it the message "run", a blank
//  window is displayed.  The only functionality available is to
//  resize, move, minimize, maximize, and close the window (i.e.,
//  terminate the application).  The main work done by an application
//  that is visible to the user is performed by the "paint" method.
//  Normally, you will create a subclass of application and override
//  the "paint" method (and probably other methods) to perform the
//  processing required by your application.
//----------------------------------------------------------------------
class application : public window
{
public:
  // constructor
  application(char *title,
              int width = CW_USEDEFAULT, int height = CW_USEDEFAULT,
              int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);

  // destructor
  virtual ~application();

  // event handling
  virtual void create();
  virtual void doubleClick(int, int);
  virtual void mouseDown(int, int);
  virtual void mouseRDown(int, int);
  virtual void mouseMDown(int, int);
  virtual void paint();
  virtual void size(int, int);
  virtual void timer();

  // non-event behavior
  virtual int  okBox(const char *, char * = 0);
  virtual int  debugBox(char *);
  virtual void run();
  virtual void quit();
  virtual void startTimer(unsigned int = 1000);
  virtual void stopTimer();
  virtual void sendTimerMessage();

private:
  int timerInterval;
};

// --- childWindow Class -----------------------------------------------
//
//  A childWindow is a separate window from the main application
//  window.
//----------------------------------------------------------------------
class childWindow : public window
{
public:
  // constructor
  childWindow(char *title,
              int width = CW_USEDEFAULT, int height = CW_USEDEFAULT,
              int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);

  // destructor
  virtual ~childWindow();

  // event handling
  void    create();
  void    doubleClick(int, int);
  void    mouseDown(int, int);
  void    mouseRDown(int, int);
  void	  mouseMDown(int, int);
  void    paint();
  void    size(int, int);
  void    timer();

  // non-event behavior
  void    run();
};

// --- Menu Class ------------------------------------------------------
//
//  The menu class implements menus.
//----------------------------------------------------------------------
class menu
{
public:
  // constructors
  menu(window &aWindow, char *text, char choice = '\0');

  // destructor
  virtual ~menu();

  // add the menu item to menu
  virtual void add(basicMenuItem & anItem);

  // add the menu to a window
  virtual void add(window & aWindow);

  // create the control
  virtual void create();

  // friends
  friend class window;

private:
  window        &theWindow;
  char          *textValue;
  char           choiceValue;
  basicMenuItem *item[MAXITEMS];
  int            itemCount;
  HMENU          handleValue;
};

// --- Control Class ---------------------------------------------------
//
//  The control class is an abstract superclass for all controls.
//----------------------------------------------------------------------
class control
{
public:
  // constructors
  control(window &aWindow);

  // equality operator
  int operator == (const control &);

  // return the identifier
  virtual int id() const;

  // select the control
  virtual void select() = 0;

  // create the control
  virtual void create() = 0;

protected:
  window &theWindow;
  int    idValue;
  HWND   hwnd;
};

// --- Menu Item Class -------------------------------------------------
//
//  The menu items class implements individual menu items.
//----------------------------------------------------------------------
class basicMenuItem : public control
{
public:
  // constructors
  basicMenuItem(window &aWindow, char *text, char choice,
                menu &aMenu);

  // destructor
  virtual ~basicMenuItem();

  // select the control
  virtual void select();

  // create the control
  virtual void create();

  // friends
  friend class menu;

protected:
  menu &menuValue;
  char *textValue;
  char choiceValue;
};


template <class T> class menuItem : public basicMenuItem
{
public:
  menuItem(T &anApp, char *text, char choice, menu &aMenu)
          : basicMenuItem(anApp, text, choice, aMenu), app(anApp)
  { /* do nothing */ }

  // select the menu item
  void select()
  { /* do nothing */ }

protected:
  T &app;
};



// --- Button Classes --------------------------------------------------
//
//  The button class implements buttons.
//----------------------------------------------------------------------
class basicButton : public control
{
public:
  // constructors
  basicButton(window &aWindow, char *aTitle = "Button",
              int aWidth = 50, int aHeight = 50, int x = 0, int y = 0);
  basicButton(const basicButton &);

  // destructor
  virtual ~basicButton();

  // assignment operator
  basicButton & operator = (const basicButton &);

  // select the control
  virtual void select();

  // create the control
  virtual void create();

protected:
  char  *titleValue;
  POINT upperLeft;
  int   heightValue;
  int   widthValue;
};


template <class T> class button : public basicButton
{
public:
  // constructor
  button(T &anApp, char *aTitle,
         int aWidth, int aHeight, int x, int y)
        : basicButton(anApp, aTitle, aWidth, aHeight, x, y), app(anApp)
  { /* do nothing */ }

  // select the button
  void select()
  { /* do nothing */ }

protected:
  T &app;
};


// --- EditText Class --------------------------------------------------
//
//  The editText class implements fields of editable text.
//----------------------------------------------------------------------
class editText
{
public:
  // constructors
  editText();
  editText(window *, int, int, int, int, int,
	         textAlign = leftText, int = 1, char * = 0);
  editText(const editText &);

  // destructor
  ~editText();

  // set input focus on
  void setFocus();

  // get the maximum number of characters
  int  size();

  // copy text to string
  char *text(char *);

  // assignment operator
  editText & operator = (const editText &);

  // "get from" operator
  editText & operator >> (char *);

  // "put to" operator
  editText & operator << (char *);
  editText & operator << (char);
  editText & operator << (int);
  editText & operator << (editText & (*)(editText &));

private:
  window *win;
  int    idValue;
  POINT  upperLeft;
  int    heightValue;
  int    widthValue;
  DWORD  style;
  HWND   hwnd;
};



// --- Static Text Class -----------------------------------------------
//
//  The staticText class implements fields of fixed text.
//----------------------------------------------------------------------
class staticText
{
public:
  // constructors
  staticText();
  staticText(window *, int, int, int, int,
             textAlign = leftText, int = 0, char * = 0);
  staticText(const staticText &);

  // destructor
  ~staticText();

  // assignment operator
  staticText & operator =  (const staticText &);

  // equality operator
  int operator == (const staticText &);

  // text output
  staticText & operator << (char *);
  staticText & operator << (char);
  staticText & operator << (int);
  staticText & operator << (staticText & (*)(staticText &));

protected:
  window *win;
  POINT  upperLeft;
  int    heightValue;
  int    widthValue;
  DWORD  style;
  HWND   hwnd;
};



// --- List Box Class --------------------------------------------------
//
//  The listBox class implements a scrollable list of strings.
//----------------------------------------------------------------------
class listBox
{
public:
  // constructors
  listBox();
  listBox(window *, int, int, int, int, int, int = 1);
  listBox(const listBox &);

  // destructor
  ~listBox();

  // list manipulation
  void clear();
  char *add(char *);

  // assignment operator
  listBox & operator =  (const listBox &);

protected:
  window *win;
  int    idValue;
  POINT  upperLeft;
  int    heightValue;
  int    widthValue;
  DWORD  style;
  HWND   hwnd;
};



//--- Templates --------------------------------------------------------

template <class T>
T maximum(T x, T y)
{
  return (x > y) ? x : y;
};

template <class T>
T minimum(T x, T y)
{
  return (x < y) ? x : y;
};


template <class T>
class WO_MANIP
{
public:
  WO_MANIP(winostream &(*ff)(winostream &, T), T ii)
          : f(ff), i(ii)
  { }

  friend winostream & operator << (winostream &wos, WO_MANIP &m)
	{ return (*m.f)(wos, m.i); }

private:
  T i;

  winostream &(*f)(winostream &, T);
};


template <class T>
class WO_MANIP2
{
public:
  WO_MANIP2(winostream & (*ff)(winostream &, T, T), T aa, T bb)
           : f(ff), a(aa), b(bb)
  { }

  friend winostream & operator << (winostream &wos, WO_MANIP2 &m)
	{ return (*m.f)(wos, m.a, m.b); }

private:
    T a;
    T b;

    winostream &(*f)(winostream &, T, T);
};



//--- Function Prototypes ----------------------------------------------

editText   &eraseText(editText &);
staticText &eraseText(staticText &);

winostream &dec(winostream &);
winostream &hex(winostream &);
winostream &oct(winostream &);

winostream &endl(winostream &);
winostream &eraseToEol(winostream &);
winostream &operator << (winostream &wos, winostream &(*)(winostream &));

WO_MANIP<streamBase> setbase(streamBase);
WO_MANIP<color>      setcolor(color);
WO_MANIP2<int>       setpos(int, int);
WO_MANIP<int>        setprecision(int);
WO_MANIP<int>        setw(int);

void debugBox();
int  randomInt(int, int);



//--- External Variables -----------------------------------------------

//extern ostrstream dout;                 // debug output stream

#endif