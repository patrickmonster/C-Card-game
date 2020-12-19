//===================================================================
//  File:        pclaf.cpp
//  Authors:     Timothy P. Justice, Yves Peneveyre
//  Created:     March 24, 1992
//  Revised:     April 5, 1995
//               Spring 1999 (adapt to Win-NT environment & Visual-C++ 5.0
//               & Borland C++ (5?)
//               September 2006 (adapt to VC++ 6,7,8)
//  Description: This file contains the implementation of the
//               classes for the Little Application Framework.
//----------------------------------------------------------------------
//  Copyright (c) 1992, 1993, 1995 by Timothy P. Justice, Oregon State U.
//  Copyright (c) 1999, by Yves Peneveyre,
//                      Ecole d'Ingenieurs du Canton de Vaud, Switzerland
//  Copyright (c) 2006, by Doohun Eum
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
//===================================================================

// Header Files
#include <math.h>			// C++ math library
#include <time.h>			// C++ time library
#include <ctype.h>			// C++ character classification
#include "pclaf.h"			// Little Application Framework



// Function Declarations

static char * menuText(char * text, char choice);



// --- Debug Output String Stream -----------------------------------
//
//  The "dout" variable is available externally to the user.  It
//  allows the programmer to use familiar stream output to the stream
//  associated with the debug message box.  The character buffer
//  "debugBuffer" holds the message and is local to this module.
//-------------------------------------------------------------------

static char debugBuffer[1024];


// --- Counter for Controls -----------------------------------------
//
//  This variabl is used to create unique ID's for buttons and
//  menu items.
//-------------------------------------------------------------------

static const int baseControlID = 100;
static       int nextControlID = baseControlID;



// --- Array of Windows ---------------------------------------------
//
//  This variable maintains an array of windows.  "lafWindow[0]"
//  always represents the main application window.
//-------------------------------------------------------------------

static const int maxWindows = 10;

static window *lafWindow[maxWindows] = { 0 };



// --- The Null Window ----------------------------------------------
//
//  The Null Window is used internally for an unspecified window.
//-------------------------------------------------------------------

static window nullWindow("NULL WINDOW", 10, 10, 0, 0);



// --- Arguments To WinMain -----------------------------------------
//
//  The following variables are the arguments sent the WinMain
//  function.
//-------------------------------------------------------------------

static HINSTANCE thisHandle  = 0;
static HINSTANCE prevHandle  = 0;
static LPSTR  commandLine = 0;
static int    commandShow = 0;



// --- Return value from WinMain ------------------------------------
//
//  The following variable holds the return value from the WinMain
//  function.  This variable is set in the application::run() method.
//-------------------------------------------------------------------

static int returnFromWinMain = 0;



// --- Is message loop executing? -----------------------------------
//
//  The following variable holds a Boolean value, where true
//  (non-zero) indicates that the application is executing the
//  message loop, i.e., the application instance as received the
//  "run" message.
//-------------------------------------------------------------------

static int msgLoopExecuting = 0;



// --- Prototype for Main LAF Function ------------------------------
//
//  The following function must be provided in the user code.
//-------------------------------------------------------------------

int   mainLAF(void);

// --- Prototype for WndProd ----------------------------------------

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


// --- Windows Program Entry Point ----------------------------------
//
//  A Windows application begins execution in a function called
//  WinMain.
//  Unfortunately, there is some unusual syntax (e.g., int PASCAL)
//  required.  In order to hide this from the programmer the WinMain
//  function is provided here.  After performing some initialization
//  it calls a programmer supplied function mainLAF().
//-------------------------------------------------------------------

int APIENTRY WinMain(HINSTANCE thisInst,
                   HINSTANCE prevInst,
                   LPSTR cmdArgs,
                   int cmdShow)
{
  // copy arguments to global variables
  thisHandle  = thisInst;
  prevHandle  = prevInst;
  commandLine = cmdArgs;
  commandShow = cmdShow;

  // call user LAF function
  (void) mainLAF();

  // return resulting value from message loop
  return returnFromWinMain;
}



// --- Application Window  Function -----------------------------
//
//  Windows associates a C++ function with each window.  This
//  function is called each time a message is sent to the window and
//  determines how each message is handled.  Windows requires a
//  specific format.
//  LAF uses the following function along with the global variable
//  "theApplication" to simply pass the arguments over to the
//  "process" method in the application for handling messages sent to
//  the main application window.  Note that the "_export" modifier
//  MUST be used to prevent an application runtime error.
//-------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND win,
                        UINT msg,
                        WPARAM wParam,
                        LPARAM lParam)
{
  for(int i = 0; i < maxWindows; i++)
  {
    if(lafWindow[i] != 0 && lafWindow[i]->handle() == win)
	    return lafWindow[i]->process(win, msg, wParam, lParam);
  }

  if(lafWindow[0] != 0)
    return lafWindow[0]->process(win, msg, wParam, lParam);

  return 1;
}



// --- Window Output Stream -----------------------------------------

winostream::winostream()
{ /* do nothing */ }

winostream::winostream(window *w)
{
  fieldWidth = -1;
  floatPrecision = -1;
  numberBase = decBase;
  winptr = w;
}


streamBase winostream::base() const
{ return numberBase; }


streamBase winostream::base(streamBase b)
{ return numberBase = b; }


static winostream &base(winostream &wos, streamBase b)
{
  wos.base(b);
  return wos;
}


WO_MANIP<streamBase> setbase(streamBase b)
{ return WO_MANIP<streamBase>(&base, b); }


winostream &dec(winostream &wos)
{ return base(wos, decBase); }


winostream &hex(winostream &wos)
{ return base(wos, hexBase); }


winostream &oct(winostream &wos)
{ return base(wos, octBase); }


static winostream &setTextColor(winostream &wos, color c)
{
  wos.outputWindow()->setTextColor(c);
  return wos;
}


WO_MANIP<color> setcolor(color c)
{ return WO_MANIP<color>(&setTextColor, c); }


static winostream &setTextPosition(winostream &wos, int x, int y)
{
  wos.outputWindow()->setTextPosition(x, y);
  return wos;
}


WO_MANIP2<int> setpos(int x, int y)
{ return WO_MANIP2<int>(&setTextPosition, x, y); }


int winostream::precision() const
{ return floatPrecision; }


int winostream::precision(int n)
{ return floatPrecision = n; }


static winostream &precision(winostream &wos, int n)
{
  wos.precision(n);
  return wos;
}


WO_MANIP<int> setprecision(int n)
{ return WO_MANIP<int>(&precision, n); }


int winostream::width() const
{ return fieldWidth; }


int winostream::width(int n)
{ return fieldWidth = n; }


static winostream &width(winostream &wos, int n)
{
  wos.width(n);
  return wos;
}


WO_MANIP<int> setw(int n)
{ return WO_MANIP<int>(&width, n); }


winostream &endl(winostream &wos)
{ return wos << '\n'; }


winostream &eraseToEol(winostream &wos)
{
  int i, x, y;
  int w = wos.outputWindow()->avgCharWidth();

  wos.outputWindow()->textPosition(x, y);

  for(i = wos.outputWindow()->right() * 2; i > 0; i -= w)
    wos << ' ';

  return wos << setpos(x, y);
}


window *winostream::outputWindow()
{ return winptr; }


winostream & operator << (winostream &wos, winostream &(*fn)(winostream &))
{ return fn(wos); }


winostream &winostream::operator << (char c)
{
  char str[2];

  str[0] = c;
  str[1] = '\0';
  winptr->print(str);
  return *this;
}


winostream &winostream::operator << (char *s)
{
  winptr->print(s);
  return *this;
}


winostream &winostream::operator << (double d)
{
  char str[80];

  if(fieldWidth < 0)
  {
    if(floatPrecision < 0)
	    wsprintf(str, "%lg", d);
    else
	    wsprintf(str, "%.*lf", floatPrecision, d);
  }
  else
  {
    if(floatPrecision < 0)
	    wsprintf(str, "%*lg", fieldWidth, d);
    else
	    wsprintf(str, "%*.*lf", fieldWidth, floatPrecision, d);
  }

  winptr->print(str);
  return *this;
}


winostream &winostream::operator << (float f)
{
  char str[80];

  if(fieldWidth < 0)
  {
    if(floatPrecision < 0)
	    wsprintf(str, "%g", f);
    else
	    wsprintf(str, "%.*f", floatPrecision, f);
  }
  else
  {
    if(floatPrecision < 0)
      wsprintf(str, "%*g", fieldWidth, f);
    else
      wsprintf(str, "%*.*f", fieldWidth, floatPrecision, f);
  }

  winptr->print(str);
  return *this;
}


winostream &winostream::operator << (int n)
{
  char str[80];
  char fmt[16];

  if(fieldWidth < 0)
    lstrcpy(fmt, "%");
  else
    wsprintf(fmt, "%%%d", fieldWidth);

  switch(numberBase)
  {
    case octBase : lstrcat(fmt, "o");  break;
    case decBase : lstrcat(fmt, "d");  break;
    case hexBase : lstrcat(fmt, "x");  break;
  }

  wsprintf(str, fmt, n);
  winptr->print(str);
  return *this;
}


winostream &winostream::operator << (long n)
{
  char str[80];
  char fmt[16];

  if(fieldWidth < 0)
    lstrcpy(fmt, "%");
  else
    wsprintf(fmt, "%%%d", fieldWidth);

  switch(numberBase)
  {
    case octBase : lstrcat(fmt, "lo");  break;
    case decBase : lstrcat(fmt, "ld");  break;
    case hexBase : lstrcat(fmt, "lx");  break;
  }

  wsprintf(str, fmt, n);
  winptr->print(str);
  return *this;
}



// --- Window Class -------------------------------------------------

window::window(char * aTitle,
               int wdth,
               int hght,
               int x,
               int y) // : wout(this)
{
  wout = this;
  int i;

  // if width and height are specified, calculate window size
  if(wdth != CW_USEDEFAULT && hght != CW_USEDEFAULT)
  {
    RECT r = { 0, 0, wdth - 1, hght - 1 };
    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, 0);
    wdth = r.right  - r.left + 1;
    hght = r.bottom - r.top  + 1;
  }

  // initialize instance variables
  brushColor     = RGB(255, 255, 255);
  cartesianColor = black;
  cartesianStyle = nullLine;
  cartesianWidth = 0;
  charHeight     = 0;
  charWidth      = 0;
  ctlCount       = 0;
  maxHScroll     = 0;
  maxVScroll     = 0;
  currentBrush   = 0;
  currentPen     = 0;
  gridColor      = black;
  gridStyle      = nullLine;
  gridWidth      = 0;
  device         = 0;
  hmenu          = 0;
  hScrollPos     = 0;
  hwnd           = 0;
  initialWidth   = wdth;
  initialHeight  = hght;
  initialX       = x;
  initialY       = y;
  lowerRight.x   = 0;
  lowerRight.y   = 0;
  maxX           = 0;
  maxY           = 0;
  mnuCount       = 0;
  nextPos.x      = -1;
  nextPos.y      = -1;
  penColor       = RGB(0, 0, 0);
  penStyle       = PS_SOLID;
  penWidth       = 1;
  vScrollPos     = 0;
  prevPos.x      = -1;
  prevPos.y      = -1;
  textColor      = RGB(0, 0, 0);
  name           = 0;
  title          = new char[strlen(aTitle)+1];
  lstrcpy(title, aTitle);
  thisInstance   = thisHandle;
  upperLeft.x    = 0;
  upperLeft.y    = 0;
  prevInstance   = prevHandle;

  for(i = 0; i < MAXMENUS; i++)
    mnu[i] = 0;

  for(i = 0; i < MAXCONTROLS; i++)
    ctl[i] = 0;
}


window::~window()
{
  // destroy window
  if (hwnd)
    DestroyWindow(hwnd);

  // deallocate memory for name
  if (name)
    delete [] name;

  // deallocate memory for title
  if (title)
    delete [] title;
}

void window::drawBitmap(bitmap image,int x,int y,int width,int height)
{
	HWND hwnd = handle();
	HDC hdc = GetDC(hwnd);
	HDC hmemdc = CreateCompatibleDC(hdc);
	bitmap holdbmp = (bitmap) SelectObject(hmemdc, image);
	BitBlt(hdc, x, y, width+1, height+1, hmemdc, 0, 0, SRCCOPY);
	SelectObject(hmemdc, holdbmp);
	DeleteDC(hmemdc);
	ReleaseDC(hwnd, hdc);
}

bitmap window::loadBitmap(int id)
{
	return LoadBitmap(instance(), MAKEINTRESOURCE(id));
}

bitmap window::loadBitmap(const char * name)
{
	return LoadBitmap(instance(), name);
}

void window::add(control *aControl)
{
  if(ctlCount < MAXCONTROLS - 1)
    ctl[ctlCount++] = aControl;
}


void window::add(menu *aMenu)
{
  if(mnuCount < MAXMENUS - 1)
    mnu[mnuCount++] = aMenu;
}


int window::avgCharWidth()
{ return metric.tmAveCharWidth; }


int window::bottom()
{ return lowerRight.y; }


void window::cartesianOff()
{
  cartesianColor  = black;
  cartesianStyle  = nullLine;
  cartesianWidth  = 0;
}


void window::cartesianOn(color c, lineStyle s, int w)
{
  cartesianColor = c;
  cartesianStyle = s;
  cartesianWidth = w;
}


void window::checkMenuItem(int item)
{
  if (hmenu)
  	CheckMenuItem(hmenu, item, MF_CHECKED);
}


void window::circle(const int x, const int y, const int radius)
{ Ellipse(device, x - radius, y - radius, x + radius, y + radius); }


void window::circle(const POINT &center, const int radius)
{ this->circle(center.x, center.y, radius); }


void window::clearAndUpdate()
{ InvalidateRect(hwnd, NULL, TRUE); }


void window::command(int anID)
{
  int n = anID - baseControlID;

  if(n >= 0 && n < MAXCONTROLS && ctl[n] != 0)
    ctl[n]->select();
}


void window::create()
{ /* do nothing */ }


editText window::createEditText(int ctlId,
                                int ctlWidth,
                                int ctlHeight,
                                int x,
                                int y,
                                textAlign align,
                                int border,
                                char *str)
{
  return editText(this,
                  ctlId,
                  ctlWidth,
                  ctlHeight,
                  x,
                  y,
                  align,
                  border,
                  str);
}


listBox window::createListBox(int ctlId,
                              int ctlWidth,
                              int ctlHeight,
                              int x,
                              int y,
                              int sort)
{
  return listBox(this, ctlId, ctlWidth, ctlHeight, x, y, sort);
}


staticText window::createStaticText(int ctlWidth,
                                    int ctlHeight,
                                    int x,
                                    int y,
                                    textAlign align,
                                    int border,
                                    char *str)
{
  return staticText(this,
                    ctlWidth,
                    ctlHeight,
                    x,
                    y,
                    align,
                    border,
                    str);
}


void window::createBrush()
{
  // you must select new brush before deleting current brush
  HBRUSH previousBrush = currentBrush;

  currentBrush = CreateSolidBrush(brushColor);
  SelectObject(device, currentBrush);

  if (previousBrush)
    DeleteObject(previousBrush);
}


void window::deleteBrush()
{
  // first select a stock brush before deleting the current brush
  SelectObject(device, GetStockObject(WHITE_BRUSH));

  if (currentBrush)
    DeleteObject(currentBrush);

  currentBrush = 0;
}


void window::createPen()
{
  // you must select new pen before deleting current pen
  HPEN previousPen = currentPen;

  currentPen = CreatePen(penStyle, penWidth, penColor);
  SelectObject(device, currentPen);

  if (previousPen)
    DeleteObject(previousPen);
}


void window::deletePen()
{
  // first select a stock pen before deleting the current pen
  SelectObject(device, GetStockObject(BLACK_PEN));

  if (currentPen)
    DeleteObject(currentPen);

  currentPen = 0;
}


void window::doubleClick(int, int)
{ /* do nothing */ }


void window::gridOff()
{
  gridColor  = black;
  gridStyle  = nullLine;
  gridWidth  = 0;
}


void window::gridOn(color c, lineStyle s, int w)
{
  gridColor = c;
  gridStyle = s;
  gridWidth = w;
}


HWND window::handle()
{ return hwnd; }


int window::height()
{ return clientY; }


void window::_initialize_()
{
  int  i;
  RECT rect;

  device = GetDC(hwnd);
  GetClientRect(hwnd, &rect);
  upperLeft.x  = rect.left;
  upperLeft.y  = rect.top;
  clientX = lowerRight.x = rect.right;
  clientY = lowerRight.y = rect.bottom;
  GetTextMetrics(device, &metric);
  charWidth = metric.tmAveCharWidth;
  charHeight = metric.tmHeight + metric.tmExternalLeading;
  nextPos.x = charWidth;
  nextPos.y = charHeight;
  prevPos = nextPos;

  for(i = 0; i < sizeof(tabStop)/sizeof(tabStop[0]); i++)
    tabStop[i] = (i + 1) * 8 * metric.tmAveCharWidth;

  if(cartesianWidth)
  {
    SetMapMode(device, MM_LOENGLISH);
    SetViewportOrgEx(device, clientX / 2, clientY / 2, NULL);
	  DPtoLP(device, (LPPOINT) &rect, 2);
	  upperLeft.x  = rect.left;
	  upperLeft.y  = rect.top;
	  lowerRight.x = rect.right;
	  lowerRight.y = rect.bottom;
  }


  // send "create" message to all menus
  if(mnuCount > 0)
  {
    if(!hmenu)
    {
      hmenu = CreateMenu();
      SetMenu(hwnd, hmenu);
    }

    for(i = 0; i < mnuCount; i++)
    {
      if(mnu[i])
        mnu[i]->create();
    }
  }

  // send "create" message to all controls
  for(i = 0; i < ctlCount; i++)
  {
    if(ctl[i])
      ctl[i]->create();
  }

  // send "create" message to yourself
  create();


  ReleaseDC(hwnd, device);
  device = 0;
}


HINSTANCE window::instance()
{ return thisInstance; }


int window::left()
{ return upperLeft.x; }


void window::line(const int fromX,
                  const int fromY,
                  const int toX,
                  const int toY)
{
  MoveToEx(device, fromX, fromY, NULL);
  LineTo(device, toX, toY);
}


void window::line(const POINT &from, const POINT &to)
{ this->line(from.x, from.y, to.x, to.y); }


HMENU window::menuHandle()
{ return hmenu; }


void window::mouseDown(int, int)
{ /* do nothing */ }

void window::mouseRDown(int, int)
{ /* do nothing */ }

void window::mouseMDown(int, int)
{ /* do nothing */ }

void window::paint()
{
  /* do nothing */ }


void window::point(const int x, const int y)
{ this->circle(x, y, 1); }


void window::polygon(POINT point[], int count)
{ Polygon(device, point, count); }


void window::print(char *str)
{
  int tabCount = sizeof(tabStop) / sizeof(tabStop[0]);
  int tabStart = metric.tmAveCharWidth;
  char *newline;
  int tmpDevice = 0;
  PAINTSTRUCT paintStruct;


  if(!device)
  {
    if(!hwnd)
      return;

    tmpDevice = 1;
    device = BeginPaint(hwnd, &paintStruct);
  }

  SetTextColor(device, textColor);

  while((newline = strchr(str, '\n')) != NULL)
  {
    if(newline != str)
	    TabbedTextOut(device,
                    nextPos.x,
                    nextPos.y,
                    str,
	                  (int) (newline - str),
                    tabCount,
                    tabStop,
                    tabStart);


    nextPos.x = metric.tmAveCharWidth;
    nextPos.y += metric.tmHeight + metric.tmExternalLeading;
    str = newline + 1;
  }

  if(*str)
  {
    TabbedTextOut(device,
                  nextPos.x,
                  nextPos.y,
                  str,
                  (int) strlen(str),
                  tabCount,
                  tabStop,
                  tabStart);

    nextPos.x += GetTabbedTextExtent(device,
                                     str,
                                     (int) strlen(str),
		                                 tabCount,
                                     tabStop);
  }

  if(tmpDevice)
  {
    EndPaint(hwnd, &paintStruct);
    device = 0;
  }
}


LRESULT WINAPI window::process(HWND win,
                               UINT msg,
                               WPARAM wParam,
                               LPARAM lParam)
{
  int         atBottom;
  int         chgH, chgV;
  PAINTSTRUCT paintStruct;
  POINT       point;
  COLORREF    saveColor;
  int         saveStyle;
  int         saveWidth;
  RECT        rect;

  switch(msg)
  {
    case WM_COMMAND :
	    this->command((int) wParam);
	    return 1;

    case WM_CREATE :
      device = GetDC(win);
      GetClientRect(win, &rect);
      ReleaseDC(win, device);
      device = 0;
      return 1;

	  case WM_LBUTTONDOWN :
      device = GetDC(win);
      point.x = LOWORD(lParam);
      point.y = HIWORD(lParam);

      if(cartesianWidth)
  		  DPtoLP(device, &point, 1);

      this->mouseDown(point.x, point.y);
      ReleaseDC(win, device);
      device = 0;
      return 1;

	  case WM_RBUTTONDOWN :
      device = GetDC(win);
      point.x = LOWORD(lParam);
      point.y = HIWORD(lParam);

      if(cartesianWidth)
  		  DPtoLP(device, &point, 1);

      this->mouseRDown(point.x, point.y);
      ReleaseDC(win, device);
      device = 0;
      return 1;

	  case WM_MBUTTONDOWN :
      device = GetDC(win);
      point.x = LOWORD(lParam);
      point.y = HIWORD(lParam);

      if(cartesianWidth)
  		  DPtoLP(device, &point, 1);

      this->mouseMDown(point.x, point.y);
      ReleaseDC(win, device);
      device = 0;
      return 1;

	  case WM_LBUTTONDBLCLK :
	  case WM_MBUTTONDBLCLK :
	  case WM_RBUTTONDBLCLK :
      device = GetDC(win);
      point.x = LOWORD(lParam);
      point.y = HIWORD(lParam);

      this->doubleClick(point.x, point.y);
      ReleaseDC(win, device);
      device = 0;
      return 1;


    case WM_NCPAINT :
      if(wParam != 1)
      {
        RedrawWindow(win, NULL, NULL, RDW_INVALIDATE |
                                      RDW_FRAME |
                                      RDW_UPDATENOW);
        return 0;
      }
      else
        return DefWindowProc(win, WM_NCPAINT, wParam, lParam);

    case WM_PAINT :
      device = BeginPaint(win, &paintStruct);
      GetClientRect(win, &rect);

      if(cartesianWidth)
      {
  		  saveColor = penColor;
		    saveStyle = penStyle;
		    saveWidth = penWidth;
		    setPen(cartesianColor, cartesianStyle, 1);
		    DPtoLP(device, (LPPOINT) &rect, 2);
		    vector(0, rect.bottom, 0, rect.top);
		    vector(rect.left, 0, rect.right, 0);
		    penColor = saveColor;
		    penStyle = saveStyle;
		    penWidth = saveWidth;
	    }

      if(gridWidth)
      {
  		  saveColor = penColor;
		    saveStyle = penStyle;
		    saveWidth = penWidth;
		    setPen(gridColor, gridStyle, 1);
		    GetClientRect(win, &rect);
			int i;

		    for(i = rect.left + gridWidth;
            i < rect.right;
            i += gridWidth)
          line(i, rect.top, i, rect.bottom);

  		  for(i = rect.top + gridWidth;
            i < rect.bottom;
            i += gridWidth)
          line(rect.left, i, rect.right, i);

		    penColor = saveColor;
		    penStyle = saveStyle;
		    penWidth = saveWidth;
      }

      this->createPen();
      this->createBrush();
      prevPos = nextPos;
      this->paint();
      this->deletePen();
      this->deleteBrush();

      EndPaint(win, &paintStruct);
      device = 0;
      return 1;

	  case WM_SIZE :
      atBottom = maxY != 0 && vScrollPos + clientY >= maxY;
      clientX = LOWORD(lParam);
      clientY = HIWORD(lParam);
      lowerRight.x = upperLeft.x + clientX - 1;
      lowerRight.y = upperLeft.y + clientY - 1;

      if(maxY > clientY && lowerRight.y > maxY)
      {
  		  lowerRight.y = maxY;
		    upperLeft.y  = lowerRight.y - clientY + 1;
      }

      if(maxX > clientX && lowerRight.x > maxX)
      {
		    lowerRight.x = maxX;
		    upperLeft.x  = lowerRight.x - clientX + 1;
      }

      maxVScroll = maximum(0, maxY - clientY);
      maxHScroll = maximum(0, maxX - clientX);
      vScrollPos = minimum(vScrollPos, maxVScroll);
      hScrollPos = minimum(hScrollPos, maxHScroll);
      SetScrollRange(win, SB_VERT, 0, maxVScroll, FALSE);
      SetScrollRange(win, SB_HORZ, 0, maxHScroll, FALSE);
      SetScrollPos(win, SB_VERT, vScrollPos, TRUE);
      SetScrollPos(win, SB_HORZ, hScrollPos, TRUE);

      if(atBottom)
		    scrollToBottom();

      nextPos = prevPos;
      device = GetDC(win);
      GetClientRect(win, &rect);

      if(cartesianWidth)
      {
  		  SetMapMode(device, MM_LOENGLISH);
		    SetViewportOrgEx(device, clientX / 2, clientY / 2, NULL);
	      DPtoLP(device, (LPPOINT) &rect, 2);
        upperLeft.x  = rect.left;
	      upperLeft.y  = rect.top;
	      lowerRight.x = rect.right;
	      lowerRight.y = rect.bottom;
      }
      else
      {
        SetMapMode(device, MM_TEXT);
        SetViewportOrgEx(device, 0, 0, NULL);
      }

      this->size((int) LOWORD(lParam), (int) HIWORD(lParam));
      ReleaseDC(win, device);
      device = 0;
      return 1;

	  case WM_VSCROLL :
      switch(wParam)
      {
  		  case SB_TOP :
		      chgV = -vScrollPos;
		      break;
		    case SB_BOTTOM :
		      chgV = maxVScroll - vScrollPos;
		      break;
		    case SB_LINEUP :
		      chgV = -charHeight;
		      break;
		    case SB_LINEDOWN :
		      chgV = charHeight;
		      break;
		    case SB_PAGEUP :
		      chgV = minimum(-charHeight, -clientY);
		      break;
		    case SB_PAGEDOWN :
		      chgV = maximum(charHeight, clientY);
		      break;
		    case SB_THUMBTRACK :
		      chgV = LOWORD(lParam) - vScrollPos;
		      break;
		    default :
		      chgV = 0;
		      break;
      }

	    chgV = maximum(-vScrollPos,
                     minimum(chgV, maxVScroll - vScrollPos));

	    if(chgV != 0)
	    {
  		  upperLeft.y  += chgV;
		    lowerRight.y += chgV;

		    if(upperLeft.y < 0)
		    {
		      upperLeft.y = 0;
		      lowerRight.y = clientY - 1;
		    }
		    else
          if(maxY > clientY && lowerRight.y > maxY)
		      {
  		      lowerRight.y = maxY;
		        upperLeft.y  = lowerRight.y - clientY + 1;
		      }

		    vScrollPos   += chgV;
		    ScrollWindow(win, 0, -chgV, NULL, NULL);
		    SetScrollPos(win, SB_VERT, vScrollPos, TRUE);
		    UpdateWindow(win);
      }

	    return 1;

	  case WM_HSCROLL :
	    switch(wParam)
	    {
  		  case SB_TOP :
		      chgH = -hScrollPos;
		      break;
	  	  case SB_BOTTOM :
		      chgH = maxHScroll - hScrollPos;
		      break;
		    case SB_LINEUP :
		      chgH = -charWidth;
		      break;
		    case SB_LINEDOWN :
		      chgH = charWidth;
		      break;
		    case SB_PAGEUP :
		      chgH = minimum(-charWidth, -clientX);
		      break;
		    case SB_PAGEDOWN :
          chgH = maximum(charWidth, clientX);
		      break;
		    case SB_THUMBTRACK :
		      chgH = LOWORD(lParam) - hScrollPos;
		      break;
		    default :
		      chgH = 0;
		      break;
	    }

	    chgH = maximum(-hScrollPos,
                     minimum(chgH, maxHScroll - hScrollPos));

      if(chgH != 0)
	    {
		    upperLeft.x  += chgH;
		    lowerRight.x += chgH;

		    if(upperLeft.x < 0)
		    {
		      upperLeft.x = 0;
		      lowerRight.x = clientX - 1;
		    }
		    else
          if(maxX > clientX && lowerRight.x > maxX)
		      {
		        lowerRight.x = maxX;
		        upperLeft.x  = lowerRight.x - clientX + 1;
		      }

		    hScrollPos   += chgH;
		    ScrollWindow(win, -chgH, 0, NULL, NULL);
		    SetScrollPos(win, SB_HORZ, hScrollPos, TRUE);
		    UpdateWindow(win);
	    }

      return 1;

	  case WM_TIMER :
      device = GetDC(win);
      this->timer();
      ReleaseDC(win, device);
      device = 0;
      return 1;

    case WM_ERASEBKGND :
      device = GetDC(win);

      GetClientRect(win, &rect);
      FillRect(device, &rect, (HBRUSH) GetStockObject(WHITE_BRUSH));
      ReleaseDC(win, device);
      device = 0;
      return 0;

    case WM_ACTIVATE :
	  case WM_ACTIVATEAPP :
      if(wParam)
      {
        device = GetDC(win);
        ReleaseDC(win, device);
        BringWindowToTop(win);
        device = 0;
      }
      return 1;

    case WM_DESTROY:
      if(this == lafWindow[0])
      {
        PostQuitMessage(0);
        return 1;
      }
      else
      {
        // free up window slot
        for(int i = 1; i < maxWindows; i++)
        {
          if (lafWindow[i] != 0 && lafWindow[i]->handle() == win)
            lafWindow[i] = 0;
        }
      }

      break;
    default :
      return DefWindowProc(win, msg, wParam, lParam);
    }

  return 1;
}


void window::quit()
{
  DestroyWindow(hwnd);
}


void window::rectangle(const int x,
                       const int y,
                       const int wdth,
                       const int hght)
{ Rectangle(device, x, y, x + wdth, y + hght); }


int window::right()
{ return lowerRight.x; }


void window::run()
{ /* do nothing */ }


void window::scrollToBottom()
{
  lowerRight.y = maximum(maxY, clientY);
  upperLeft.y  = lowerRight.y - clientY + 1;
  vScrollPos = maximum(0, maxY - clientY);
  SetScrollPos(hwnd, SB_VERT, vScrollPos, TRUE);
}


void window::scrollToLeft()
{
  upperLeft.x  = 0;
  lowerRight.x = clientX - 1;
  hScrollPos = 0;
  SetScrollPos(hwnd, SB_HORZ, hScrollPos, TRUE);
}


void window::scrollToRight()
{
  lowerRight.x = maximum(maxX, clientX);
  upperLeft.x  = lowerRight.x - clientX + 1;
  hScrollPos = maximum(0, maxX - clientX);
  SetScrollPos(hwnd, SB_HORZ, hScrollPos, TRUE);
}


void window::scrollToTop()
{
  upperLeft.y  = 0;
  lowerRight.y = clientY - 1;
  vScrollPos = 0;
  SetScrollPos(hwnd, SB_VERT, vScrollPos, TRUE);
}


void window::setBrush(color c)
{
  switch(c)
  {
    case black         : brushColor = RGB(  0,   0,   0); break;
    case blue          : brushColor = RGB(  0,   0, 128); break;
    case green         : brushColor = RGB(  0, 128,   0); break;
    case cyan          : brushColor = RGB(  0, 128, 128); break;
    case red           : brushColor = RGB(128,   0,   0); break;
    case magenta       : brushColor = RGB(128,   0, 128); break;
    case brown         : brushColor = RGB(128, 128,   0); break;
    case gray          : brushColor = RGB(128, 128, 128); break;
    case brightBlue    : brushColor = RGB(  0,   0, 255); break;
    case brightGreen   : brushColor = RGB(  0, 255,   0); break;
    case brightCyan    : brushColor = RGB(  0, 255, 255); break;
    case brightRed     : brushColor = RGB(255,   0,   0); break;
    case brightMagenta : brushColor = RGB(255,   0, 255); break;
    case brightYellow  : brushColor = RGB(255, 255,   0); break;
    case brightGray    : brushColor = RGB(192, 192, 192); break;
    case white         : brushColor = RGB(255, 255, 255); break;
    default            : brushColor = RGB(  0,   0,   0); break;
  }

  this->createBrush();
}


void window::setMaxX(int n)
{ maxX = n; }


void window::setMaxY(int n)
{ maxY = n; }


void window::setPen(color c, lineStyle s, int w)
{
  switch(c)
  {
    case black         : penColor = RGB(  0,   0,   0); break;
    case blue          : penColor = RGB(  0,   0, 128); break;
    case green         : penColor = RGB(  0, 128,   0); break;
    case cyan          : penColor = RGB(  0, 128, 128); break;
    case red           : penColor = RGB(128,   0,   0); break;
    case magenta       : penColor = RGB(128,   0, 128); break;
    case brown         : penColor = RGB(128, 128,   0); break;
    case gray          : penColor = RGB(128, 128, 128); break;
    case brightBlue    : penColor = RGB(  0,   0, 255); break;
    case brightGreen   : penColor = RGB(  0, 255,   0); break;
    case brightCyan    : penColor = RGB(  0, 255, 255); break;
    case brightRed     : penColor = RGB(255,   0,   0); break;
    case brightMagenta : penColor = RGB(255,   0, 255); break;
    case brightYellow  : penColor = RGB(255, 255,   0); break;
    case brightGray    : penColor = RGB(192, 192, 192); break;
    case white         : penColor = RGB(255, 255, 255); break;
    default            : penColor = RGB(  0,   0,   0); break;
  }

  switch(s)
  {
    case solidLine  : penStyle = PS_SOLID; break;
    case dashedLine : penStyle = PS_DASH;  break;
    case dottedLine : penStyle = PS_DOT;   break;
    case nullLine   : penStyle = PS_NULL;  break;
    default         : penStyle = PS_SOLID; break;
  }

  penWidth = w;
  this->createPen();
}


void window::setTextColor(color c)
{
  switch (c)
  {
    case black         : textColor = RGB(  0,   0,   0); break;
    case blue          : textColor = RGB(  0,   0, 128); break;
    case green         : textColor = RGB(  0, 128,   0); break;
    case cyan          : textColor = RGB(  0, 128, 128); break;
    case red           : textColor = RGB(128,   0,   0); break;
    case magenta       : textColor = RGB(128,   0, 128); break;
    case brown         : textColor = RGB(128, 128,   0); break;
    case gray          : textColor = RGB(128, 128, 128); break;
    case brightBlue    : textColor = RGB(  0,   0, 255); break;
    case brightGreen   : textColor = RGB(  0, 255,   0); break;
    case brightCyan    : textColor = RGB(  0, 255, 255); break;
    case brightRed     : textColor = RGB(255,   0,   0); break;
    case brightMagenta : textColor = RGB(255,   0, 255); break;
    case brightYellow  : textColor = RGB(255, 255,   0); break;
    case brightGray    : textColor = RGB(192, 192, 192); break;
    case white         : textColor = RGB(255, 255, 255); break;
    default            : textColor = RGB(  0,   0,   0); break;
  }
}


void window::setTextPosition(int x, int y)
{
  nextPos.x = x;
  nextPos.y = y;
  prevPos = nextPos;
}


int window::show(int flag)
{ return ShowWindow(hwnd, flag); }


void window::size(int, int)
{ /* do nothing */ }


int window::textHeight()
{ return metric.tmHeight + metric.tmExternalLeading; }


int window::textWidth(char *str)
{
  HDC hdc = GetDC(hwnd);
  SIZE width;

  if(hdc)
  {
    GetTextExtentPoint32(hdc, str, (int) strlen(str), &width);
	  ReleaseDC(hwnd, hdc);
  }

  return width.cx;
}


void window::textPosition(int &x, int &y)
{
  x = nextPos.x;
  y = nextPos.y;
}


void window::timer()
{ /* do nothing */ }


int window::top()
{ return upperLeft.y; }


void window::uncheckMenuItem(int item)
{
  if(hmenu)
	  CheckMenuItem(hmenu, item, MF_UNCHECKED);
}


void window::update()
{ InvalidateRect(hwnd, NULL, FALSE); }


void window::vector(const int fromX,
                    const int fromY,
                    const int toX,
                    const int toY)
{
  POINT newOrigin;
  POINT origin;
  double theta;
  int newFromX, newFromY, newToX, newToY;
  int x, y;

  MoveToEx(device, fromX, fromY, NULL);
  LineTo(device, toX, toY);

  newOrigin.x = toX;
  newOrigin.y = toY;
  LPtoDP(device, &newOrigin, 1);
  GetViewportOrgEx(device, &origin);
  SetViewportOrgEx(device, newOrigin.x, newOrigin.y, NULL);

  newFromX = fromX - toX;
  newFromY = fromY - toY;
  newToX = newToY = 0;

  if(newFromX == 0)
  {
    if(newFromY <= newToY)
	    theta = 3.0 * M_PI / 2.0;
    else
	    theta = M_PI / 2.0;
  }
  else
    if(newToX > newFromX)
      theta = atan(double(newFromY)/double(newFromX)) + M_PI;
    else
      theta = atan(double(newFromY)/double(newFromX));

  x = (int)ceil(10.0 * cos(theta + M_PI/6.0) + 0.5);
  y = (int)ceil(10.0 * sin(theta + M_PI/6.0) + 0.5);
  MoveToEx(device, 0, 0, NULL);
  LineTo(device, x, y);

  x = (int)ceil(10.0 * cos(theta - M_PI/6.0) + 0.5);
  y = (int)ceil(10.0 * sin(theta - M_PI/6.0) + 0.5);
  MoveToEx(device, 0, 0, NULL);
  LineTo(device, x, y);

  SetViewportOrgEx(device, origin.x, origin.y, NULL);
  MoveToEx(device, toX, toY, NULL);
}


int window::width()
{ return clientX; }



// --- Application -----------------------------------------------------

application::application(char *aTitle,
                         int wdth,
                         int hght,
                         int x,
                         int y) : window(aTitle,
                                         wdth,
                                         hght,
                                         x,
                                         y)
{
  static char *aName = "LAFAPP";


  // initialize timer
  timerInterval  = 0;


  // initialize application name
  name = new char[strlen(aName)+1];
  lstrcpy(name, aName);


  // store a pointer to this instance in the window array
  lafWindow[0] = this;
}


application::~application()
{ /* everything taken care of by superclass */ }


void application::create()
{ /* do nothing */ }


void application::doubleClick(int, int)
{ /* do nothing */ }


void application::mouseDown(int, int)
{ /* do nothing */ }

void application::mouseRDown(int, int)
{ /* do nothing */ }

void application::mouseMDown(int, int)
{ /* do nothing */ }


int application::okBox(const char *msg, char *caption)
{
  int saveTimer = timerInterval;
  int result;

  if(saveTimer)
    stopTimer();

  result = MessageBox(hwnd, msg, caption, MB_OK | MB_TASKMODAL);

  if(saveTimer)
    startTimer(saveTimer);

  return result;
}


int application::debugBox(char *msg)
{
  int saveTimer = timerInterval;
  int result;

  if(saveTimer)
    stopTimer();

  result = MessageBox(hwnd,
                      msg,
                      "DEBUG",
                      MB_OKCANCEL |MB_TASKMODAL);

  if(result == IDCANCEL)
    abort();

  if(saveTimer)
    startTimer(saveTimer);

  return result;
}


void application::paint()
{ /* do nothing */ }


void application::quit()
{
  DestroyWindow(hwnd);
}


void application::run()
{
  int    cmdShow = SW_SHOWNORMAL;
  DWORD  style   = WS_OVERLAPPEDWINDOW |
                   WS_CLIPCHILDREN |
                   WS_VSCROLL |
                   WS_HSCROLL;
  static WNDCLASS wndClass;
  MSG    message;
  int    i;

  // start all child windows running
  for(i = 1; i < maxWindows; i++)
  {
    if(lafWindow[i] != 0)
      lafWindow[i]->run();
  }

  // register the window class
  if(!prevHandle)
  {
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = 0;
    wndClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hInstance     = thisInstance;
    wndClass.lpfnWndProc   = WndProc;
    wndClass.lpszClassName = name;
    wndClass.lpszMenuName  = NULL;
    wndClass.style         = CS_HREDRAW |
                             CS_VREDRAW |
                             CS_OWNDC |
                             CS_DBLCLKS;

    RegisterClass(&wndClass);
  }


  // reset handle values (in case instance defined as a global)
  thisInstance = thisHandle;
  prevInstance = prevHandle;

  // create the window
  hwnd = CreateWindow(name,			// window class name
                      title,			// main window title
                      style,			// window style
                      initialX,		// initial x position
                      initialY,		// initial y position
                      initialWidth,		// initial x size
                      initialHeight,		// initial y size
                      NULL,			// parent window
                      NULL,			// menu
                      thisHandle,		// application instance
                      NULL);			// creation parameters

  // send "initialize" message to yourself
  _initialize_();


  // display the window
  ShowWindow(hwnd, cmdShow);
  UpdateWindow(hwnd);


  // set global variable, indicating that the message loop is
  // executing
  msgLoopExecuting = 1;


  // start message loop
  while(GetMessage(&message, 0, 0, 0))
  {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }

  returnFromWinMain = (int) message.wParam;
}


void application::size(int, int)
{ /* do nothing */ }


void application::startTimer(unsigned int ms)
{
  if(timerInterval != 0)
    KillTimer(hwnd, 1);

  timerInterval = ms;

  if(SetTimer(hwnd, 1, timerInterval, NULL) == 0)
  {
    MessageBox(hwnd,
               "Too many timers.  Close some applications",
               "Little Application Framework",
               MB_ICONEXCLAMATION | MB_OK);
    timerInterval = 0;
  }
}


void application::stopTimer()
{
  if(timerInterval != 0)
  {
    KillTimer(hwnd, 1);
    timerInterval = 0;
  }
}


void application::sendTimerMessage()
{
  PostMessage(hwnd, WM_TIMER, 1, 0);
}


void application::timer()
{ /* do nothing */ }



// --- childWindow --------------------------------------------------

childWindow::childWindow(char *aTitle,
                         int wdth,
                         int hght,
                         int x,
                         int y) : window(aTitle,
                                         wdth,
                                         hght,
                                         x,
                                         y)
{
  static char *aName = "LAFCHILD";
  int i;


  // initialize application name
  name = new char[strlen(aName)+1];
  lstrcpy(name, aName);


  // store a pointer to this instance in the window array
  for(i = 1; i < maxWindows; i++)
  {
    if(lafWindow[i] == 0)
    {
      lafWindow[i] = this;
      break;
    }
  }


  // check for too many windows
  if(i >= maxWindows)
    FatalAppExit(0, "Too many windows");


  // if the message loop is executing, start running immediately 
  if(msgLoopExecuting)
    this->run();
}


childWindow::~childWindow()
{ /* everything taken care of by superclass */ }


void childWindow::create()
{ /* do nothing */ }


void childWindow::doubleClick(int, int)
{ /* do nothing */ }


void childWindow::mouseDown(int, int)
{ /* do nothing */ }

void childWindow::mouseRDown(int, int)
{ /* do nothing */ }

void childWindow::mouseMDown(int, int)
{ /* do nothing */ }


void childWindow::paint()
{ /* do nothing */ }


void childWindow::run()
{
  int    cmdShow = SW_SHOWNORMAL;
  DWORD  style   = (WS_OVERLAPPEDWINDOW & ~WS_SYSMENU) |
                    WS_CLIPCHILDREN | WS_VISIBLE |
                    WS_VSCROLL | WS_HSCROLL;
  static WNDCLASS wndClass;
  static int firstTime = 1;


  // register the window class
  if(!prevHandle && firstTime)
  {
    firstTime = 0;

    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = 0;
    wndClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hInstance     = thisInstance;
    wndClass.lpfnWndProc   = WndProc;
    wndClass.lpszClassName = name;
    wndClass.lpszMenuName  = NULL;
    wndClass.style         = CS_HREDRAW |
                             CS_VREDRAW |
                             CS_OWNDC |
                             CS_DBLCLKS;

    RegisterClass(&wndClass);
  }


  // reset handle values (in case instance defined as a global)
  thisInstance = thisHandle;
  prevInstance = prevHandle;


  // create the window
  hwnd = CreateWindow(name,			// window class name
                      title,			// main window title
                      style,			// window style
                      initialX,		// initial x position
                      initialY,		// initial y position
                      initialWidth,		// initial x size
                      initialHeight,		// initial y size
                      HWND_DESKTOP,           // parent window
                      NULL,			// menu
                      thisHandle,		// application instance
                      NULL);			// creation parameters


  // send "initialize" message to yourself
  _initialize_();


  // display the window
  ShowWindow(hwnd, cmdShow);
}


void childWindow::size(int, int)
{ /* do nothing */ }


void childWindow::timer()
{ /* do nothing */ }



// --- Menu ---------------------------------------------------------

menu::menu(window &aWindow,
           char *text,
           char choice) : theWindow(aWindow)
{
  itemCount   = 0;
  textValue   = menuText(text, choice);
  choiceValue = choice;
  handleValue = 0;
  theWindow.add(this);
}


menu::~menu()
{
    // do nothing
}


void menu::add(basicMenuItem &anItem)
{
  if(itemCount < MAXITEMS - 1)
  {
    item[itemCount] = &anItem;

    if(handleValue)
    {
      AppendMenu(handleValue,
                 MF_STRING,
                 item[itemCount]->idValue,
                 item[itemCount]->textValue);
    }

    itemCount++;
  }
}


void menu::add(window &aWindow)
{
  int i;

  handleValue = CreateMenu();
  theWindow = aWindow;

  AppendMenu(theWindow.menuHandle(),
             MF_POPUP,
             (UINT_PTR)handleValue,
             textValue);

  for(i = 0; i < itemCount; i++)
  {
    AppendMenu(handleValue,
               MF_STRING,
               item[i]->idValue,
               item[i]->textValue);
  }
}


void menu::create()
{
  int i;

  handleValue = CreateMenu();
  AppendMenu(theWindow.menuHandle(),
             MF_POPUP,
             (UINT_PTR)handleValue,
             textValue);

  for(i = 0; i < itemCount; i++)
  {
    AppendMenu(handleValue,
               MF_STRING,
               item[i]->idValue,
               item[i]->textValue);
  }
}



// --- Control ------------------------------------------------------

control::control(window &aWindow)
                : theWindow(aWindow)
{
  idValue = nextControlID++;
  hwnd    = 0;
  aWindow.add(this);
}


int control::operator == (const control &right)
{
  return idValue == right.idValue;
}


int control::id() const
{
  return idValue;
}



// --- Menu Item ----------------------------------------------------

basicMenuItem::basicMenuItem(window &aWindow,
                             char *text,
                             char choice,
                             menu &aMenu) : control(aWindow),
                                            menuValue(aMenu)
{
  textValue   = menuText(text, choice);
  choiceValue = choice;
  menuValue.add(*this);
}


basicMenuItem::~basicMenuItem()
{
  if(textValue)
    delete [] textValue;
}


void basicMenuItem::select()
{
    // do nothing
}


void basicMenuItem::create()
{
    // do nothing
}



// --- Button -------------------------------------------------------

basicButton::basicButton(window &aWindow,
                         char *aTitle,
                         int aWidth,
                         int aHeight,
                         int x,
                         int y) : control(aWindow)
{
  titleValue  = aTitle;
  widthValue  = aWidth;
  heightValue = aHeight;
  upperLeft.x = x;
  upperLeft.y = y;
}


basicButton::basicButton(const basicButton &initial)
                        : control(initial.theWindow)
{
  titleValue  = initial.titleValue;
  idValue     = initial.idValue;
  widthValue  = initial.widthValue;
  heightValue = initial.heightValue;
  upperLeft   = initial.upperLeft;
  hwnd        = initial.hwnd;
}


basicButton::~basicButton()
{ }


void basicButton::create()
{
  DWORD style = WS_CHILD |
                WS_CLIPCHILDREN |
                WS_VISIBLE |
                BS_PUSHBUTTON;

  hwnd = CreateWindow("button",		// window class name
                      titleValue,		// window title
                      style,			// window style
                      upperLeft.x,		// initial x position
                      upperLeft.y,		// initial y position
                      widthValue,		// initial x size
                      heightValue,		// initial y size
                      theWindow.handle(),	// parent window
                      (HMENU)(INT_PTR)idValue,		// menu
                      thisHandle,             // application instance
                      NULL);			// creation parameters
}


basicButton &basicButton::operator = (const basicButton &right)
{
  theWindow   = right.theWindow;
  titleValue  = right.titleValue;
  idValue     = right.idValue;
  widthValue  = right.widthValue;
  heightValue = right.heightValue;
  upperLeft   = right.upperLeft;
  hwnd        = right.hwnd;

  return *this;
}


void basicButton::select()
{
    // do nothing
}



// --- EditText -----------------------------------------------------

editText::editText()
{
  win         = 0;
  idValue     = 0;
  upperLeft.x = 0;
  upperLeft.y = 0;
  heightValue = 0;
  widthValue  = 0;
  style       = 0;
  hwnd        = 0;
}


editText::editText(window *parentWin,
                   int ctlId,
                   int ctlWidth,
                   int ctlHeight,
                   int x,
                   int y,
                   textAlign align,
                   int border,
                   char *str)
{
  style = WS_CHILD | WS_VISIBLE | ES_MULTILINE;

  switch(align)
  {
    case leftText :    style |= ES_LEFT;    break;
    case centerText :  style |= ES_CENTER;  break;
    case rightText :   style |= ES_RIGHT;   break;
    default :          style |= ES_LEFT;    break;
  }

  if(border)
    style |= WS_BORDER;

  win         = parentWin;
  idValue     = ctlId;
  widthValue  = ctlWidth;
  heightValue = ctlHeight;
  upperLeft.x = x;
  upperLeft.y = y;

  hwnd = CreateWindow("edit",			// window class name
                      str,			// window title
                      style,			// window style
                      upperLeft.x,		// initial x position
                      upperLeft.y,		// initial y position
                      widthValue,		// initial x size
                      heightValue,		// initial y size
                      win->handle(),		// parent window
                      (HMENU)(INT_PTR)idValue,		// menu
                      thisHandle,             // application instance
                      NULL);			// creation parameters

 if(hwnd)
   SendMessage(hwnd, EM_FMTLINES, 0, 0);
}


editText::editText(const editText &initial)
{
  char str[512];

  GetWindowText(initial.hwnd, str, sizeof(str));

  win         = initial.win;
  idValue     = initial.idValue;
  upperLeft   = initial.upperLeft;
  heightValue = initial.heightValue;
  widthValue  = initial.widthValue;
  style       = initial.style;

  hwnd = CreateWindow("edit",			// window class name
                      str,			// window title
                      style,			// window style
                      upperLeft.x,		// initial x position
                      upperLeft.y,		// initial y position
                      widthValue,		// initial x size
                      heightValue,		// initial y size
                      win->handle(),		// parent window
                      0,			// menu
                      win->instance(),	// application instance
                      NULL);			// creation parameters

  if(hwnd)
    SendMessage(hwnd, EM_FMTLINES, 0, 0);
}


editText::~editText()
{
  if(hwnd)
	  DestroyWindow(hwnd);
}


void editText::setFocus()
{
  if(hwnd)
  	SetFocus(hwnd);
}


int editText::size()
{
  int cols, rows;
  HDC device;
  TEXTMETRIC metric;

  SIZE textExtent;
  
  if (hwnd)
  {
    device = GetDC(hwnd);
    GetTextMetrics(device, &metric);
    GetTextExtentPoint32(device, "i", 1, &textExtent);
    ReleaseDC(hwnd, device);

    cols = widthValue / textExtent.cx;
    rows = heightValue / (metric.tmHeight +
                          metric.tmExternalLeading);
    return rows * cols;
  }
  else
    return 0;
}


char *editText::text(char *str)
{
  int i, n;

  if(hwnd)
  {
    n = GetWindowText(hwnd, str, size());
    str[n] = '\0';

// strip trailing spaces
    for(i = (int)strlen(str) - 1; i >= 0 && str[i] == ' '; i--)
      str[i] = '\0';
  }
  else
    *str = '\0';

  return str;
}


editText &editText::operator = (const editText &right)
{
  char str[512] = {0};

  GetWindowText(right.hwnd, str, sizeof(str));

  win         = right.win;
  idValue     = right.idValue;
  upperLeft   = right.upperLeft;
  heightValue = right.heightValue;
  widthValue  = right.widthValue;
  style       = right.style;

  hwnd = CreateWindow("edit",			// window class name
                      str,			// window title
		                  style,			// window style
		                  upperLeft.x,		// initial x position
		                  upperLeft.y,		// initial y position
		                  widthValue,		// initial x size
		                  heightValue,		// initial y size
		                  win->handle(),		// parent window
		                  0,			// menu
		                  win->instance(),	// application instance
		                  NULL);			// creation parameters

  if(hwnd)
    SendMessage(hwnd, EM_FMTLINES, 0, 0);

  return *this;
}


editText &editText::operator >> (char *str)
{
  int n;

  if(hwnd)
  {
    n = GetWindowText(hwnd, str, size());
    str[n] = '\0';
  }
  else
    *str = '\0';

  return *this;
}


editText &editText::operator << (char *str)
{
  int  currentLength = GetWindowTextLength(hwnd);
  char newText[512];

  if(!hwnd || currentLength + strlen(str) + 1 > sizeof(newText))
    return *this;

  GetWindowText(hwnd, newText, sizeof(newText));
  lstrcpy(newText + currentLength, str);
  SetWindowText(hwnd, newText);

  return *this;
}


editText &editText::operator << (char ch)
{
  char str[2];

  if(ch == '\f' && hwnd)
  {
    SetWindowText(hwnd, "");
    return *this;
  }
  else
  {
    wsprintf(str, "%c", ch);
    return (*this) << str;
  }
}


editText &editText::operator << (int n)
{
  char str[32];

  wsprintf(str, "%d", n);
  return (*this) << str;
}


editText &editText::operator << (editText &(*fn)(editText &))
{ return fn(*this); }


editText &eraseText(editText &txt)
{ return txt << '\f'; }



// --- staticText ---------------------------------------------------

staticText::staticText()
{
  win         = 0;
  upperLeft.x = 0;
  upperLeft.y = 0;
  heightValue = 0;
  widthValue  = 0;
  hwnd        = 0;
}


staticText::staticText(window *parentWin,
                       int ctlWidth,
                       int ctlHeight,
                       int x,
                       int y,
                       textAlign align,
                       int border,
                       char *str)
{
  style = WS_CHILD | WS_VISIBLE;

  switch(align)
  {
    case leftText:    style |= SS_LEFT;    break;
    case centerText:  style |= SS_CENTER;  break;
    case rightText:   style |= SS_RIGHT;   break;
    default:          style |= SS_LEFT;    break;
  }

  if(border)
    style |= WS_BORDER;

  win         = parentWin;
  widthValue  = ctlWidth;
  heightValue = ctlHeight;
  upperLeft.x = x;
  upperLeft.y = y;

  hwnd = CreateWindow("static",		// window class name
                      str,			// window title
                      style,			// window style
                      upperLeft.x,		// initial x position
                      upperLeft.y,		// initial y position
                      widthValue,		// initial x size
                      heightValue,		// initial y size
                      win->handle(),		// parent window
                      0,			// menu
                      win->instance(),	// application instance
                      NULL);			// creation parameters
}


staticText::staticText(const staticText &initial)
{
  char str[512];

  GetWindowText(initial.hwnd, str, sizeof(str));

  win         = initial.win;
  upperLeft   = initial.upperLeft;
  heightValue = initial.heightValue;
  widthValue  = initial.widthValue;
  style       = initial.style;

  hwnd = CreateWindow("static",		// window class name
		                  str,			// window title
		                  style,			// window style
		                  upperLeft.x,		// initial x position
		                  upperLeft.y,		// initial y position
		                  widthValue,		// initial x size
		                  heightValue,		// initial y size
		                  win->handle(),		// parent window
		                  0,			// menu
		                  win->instance(),	// application instance
		                  NULL);			// creation parameters
}


staticText::~staticText()
{
  if (hwnd)
    DestroyWindow(hwnd);
}


staticText &staticText::operator = (const staticText &right)
{
  char str[512];

  GetWindowText(right.hwnd, str, sizeof(str));

  win         = right.win;
  upperLeft   = right.upperLeft;
  heightValue = right.heightValue;
  widthValue  = right.widthValue;
  style       = right.style;

  hwnd = CreateWindow("static",		// window class name
		                  str,			// window title
		                  style,			// window style
		                  upperLeft.x,		// initial x position
		                  upperLeft.y,		// initial y position
		                  widthValue,		// initial x size
		                  heightValue,		// initial y size
		                  win->handle(),		// parent window
		                  0,			// menu
		                  win->instance(),	// application instance
		                  NULL);			// creation parameters

  return *this;
}


int staticText::operator == (const staticText &right)
{
  if(win == right.win &&
     upperLeft.x == right.upperLeft.x  &&
     upperLeft.y == right.upperLeft.y  &&
     heightValue == right.heightValue  &&
     widthValue  == right.widthValue)
    return 1;
  else
    return 0;
}


staticText &staticText::operator << (char *str)
{
  int  currentLength = GetWindowTextLength(hwnd);
  char newText[512];

  if(!hwnd || currentLength + strlen(str) + 1 > sizeof(newText))
    return *this;

  GetWindowText(hwnd, newText, sizeof(newText));
  lstrcpy(newText + currentLength, str);
  SetWindowText(hwnd, newText);

  return *this;
}


staticText &staticText::operator << (char ch)
{
  char str[2];

  if(ch == '\f' && hwnd)
  {
    SetWindowText(hwnd, "");
    return *this;
  }
  else
  {
    wsprintf(str, "%c", ch);
    return (*this) << str;
  }
}


staticText &staticText::operator << (int n)
{
  char str[32];

  wsprintf(str, "%d", n);
  return (*this) << str;
}


staticText &staticText::operator << (staticText &(*fn)(staticText &))
{ return fn(*this); }


staticText &eraseText(staticText &txt)
{ return txt << '\f'; }



// --- listBox ------------------------------------------------------

listBox::listBox()
{
  win         = 0;
  idValue     = 0;
  upperLeft.x = 0;
  upperLeft.y = 0;
  heightValue = 0;
  widthValue  = 0;
  style       = 0;
  hwnd        = 0;
}


listBox::listBox(window *parentWin,
                 int ctlId,
                 int ctlWidth,
                 int ctlHeight,
                 int x,
                 int y,
                 int sort)
{
  style = WS_CHILD |
          WS_VISIBLE |
          WS_VSCROLL |
          WS_BORDER |
          LBS_NOTIFY;

  if(sort)
    style |= LBS_SORT;

  win         = parentWin;
  idValue     = ctlId;
  widthValue  = ctlWidth;
  heightValue = ctlHeight;
  upperLeft.x = x;
  upperLeft.y = y;

  hwnd = CreateWindow("listbox",		// window class name
	                    NULL,			// window title
	                    style,			// window style
	                    upperLeft.x,		// initial x position
	                    upperLeft.y,		// initial y position
	                    widthValue,		// initial x size
	                    heightValue,		// initial y size
	                    win->handle(),		// parent window
	                    (HMENU)(INT_PTR)idValue,		// menu
	                    win->instance(),	// application instance
	                    0);			// creation parameters
}


listBox::listBox(const listBox &initial)
{
  win         = initial.win;
  idValue     = initial.idValue;
  upperLeft   = initial.upperLeft;
  heightValue = initial.heightValue;
  widthValue  = initial.widthValue;
  style       = initial.style;

  hwnd = CreateWindow("listbox",		// window class name
	                    NULL,			// window title
	                    style,			// window style
	                    upperLeft.x,		// initial x position
	                    upperLeft.y,		// initial y position
	                    widthValue,		// initial x size
	                    heightValue,		// initial y size
	                    win->handle(),		// parent window
	                    (HMENU)(INT_PTR)idValue,		// menu
	                    win->instance(),	// application instance
	                    0);			// creation parameters
}


listBox &listBox::operator = (const listBox &right)
{
  win         = right.win;
  idValue     = right.idValue;
  upperLeft   = right.upperLeft;
  heightValue = right.heightValue;
  widthValue  = right.widthValue;
  style       = right.style;

  hwnd = CreateWindow("listbox",		// window class name
                      NULL,			// window title
                      style,			// window style
                      upperLeft.x,		// initial x position
                      upperLeft.y,		// initial y position
                      widthValue,		// initial x size
                      heightValue,		// initial y size
                      win->handle(),		// parent window
                      (HMENU)(INT_PTR)idValue,		// menu
                      win->instance(),	// application instance
                      0);			// creation parameters

  return *this;
}


listBox::~listBox()
{
  if(hwnd)
    DestroyWindow(hwnd);
}


void listBox::clear()
{
  SendMessage(hwnd, LB_RESETCONTENT, 0, 0);

  // trick to get scroll bar to disappear
  SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM) " ");
  SendMessage(hwnd, LB_RESETCONTENT, 0, 0);
}


char *listBox::add(char *str)
{
  SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM) str);
  return str;
}



// --- Miscellaneous Functions --------------------------------------

// display a debug box
void debugBox()
{
  if(lafWindow[0])
  {
    // display message box
    ((application *) lafWindow[0])->debugBox(debugBuffer);
  }
}



// generate a random number from lo though hi
int randomInt(int lo, int hi)
{
  static int firstTime = 1;
  int tmp;

  if(firstTime)		// seed random generator 1st time only
  {
//    time_t t;
//    srand(time(&t));
    srand(GetTickCount());
    firstTime = 0;
  }

  if(hi < lo)		// make sure lo <= hi
  {
    tmp = hi;
    hi  = lo;
    lo  = tmp;
  }

  return (rand() % (hi - lo + 1)) + lo;
}



// create menu text
static char *menuText(char *text, char choice)
{
  char c = tolower(choice);
  char *newText;
  int i, j;

  if(choice != '\0')
  {
    newText = new char[strlen(text) + 2];

    if(newText)
    {
      for(i = j = 0; text[i] != '\0'; i++, j++)
      {
        if(tolower(text[i]) == c)
          newText[j++] = '&';

        newText[j] = text[i];
      }

    newText[j] = '\0';
    }
  }
  else
  {
    newText = new char[strlen(text) + 1];

    if(newText)
      lstrcpy(newText, text);
  }

  return newText;

}

