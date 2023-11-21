/*
 * XWindow.h
 *****************************************************************************/
#ifndef XWINDOW_H
#define XWINDOW_H

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

#define XW_FONT_NAME  "-*-times-medium-r-*-*-24-*-*-*-*-*-*-*"

void    XWMakePixmap   (Pixmap *pixmap, int     nlines);
void    XWDrawString   (Pixmap  pixmap, char*   str);
void    XWDrawPixmap   (Pixmap  pixmap);
void    XWFreePixmap   (Pixmap  pixmap);
void    XWNextXEvent   (XEvent *xevent);
void    XWMoveScroll   (int     nlines);
int     XWInitWindow   (char   *title);

#endif
