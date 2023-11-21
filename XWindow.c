/*
 * XWindow.c
 *****************************************************************************/
#include "XWindow.h"
/*
 * Static variable declarations:
 *****************************************************************************/
static  Display        *display;
static  XFontStruct    *font;
static  Window          window;
static  Window          root;
static  GC              gc;
static  unsigned long   black;
static  unsigned long   white;
static  int             width;
static  int             height;
static  int             scroll;
static  int             screen;
static  int             depth;
static  char           *title;
/*
 * Static function declarations:
 *****************************************************************************/
static  void    DrawStr(char *str, size_t len, Pixmap pixmap, int y);
static  short   TextOff(char *str, size_t len);
static  size_t  LineLen(char *str);
static  size_t  WordLen(char *str);
/*
 * Global function bodies:
 *****************************************************************************/
void
XWMakePixmap(Pixmap *pixmap, int nlines)
{
	int h = nlines * (font->ascent + font->descent);

	*pixmap = XCreatePixmap(display, window, width, h, depth);
	XSetForeground(display, gc, white);
	XFillRectangle(display, *pixmap, gc, 0, 0, width, h);
	XSetForeground(display, gc, black);
}
/*
 * Draw a string to a pixmap as wrapped text that does not break words.
 */
void
XWDrawString(Pixmap pixmap, char *str)
{
	size_t len = 0;
	size_t i   = 0;
	int y = font->ascent;

	while ((len = LineLen(str + i)) > 0) {
		DrawStr(str + i, len, pixmap, y);
		y += font->ascent + font->descent;
		i += len;
	}
}
/*
 * Copy the contents of a pixmap to the window based on the scroll value.
 */
void
XWDrawPixmap(Pixmap pixmap)
{
	int y = scroll * (font->ascent + font->descent);
	XCopyArea(display, pixmap, window, gc, 0, y, width, height, 0, 0);
}
/*
 * This is just a wrapper for XFreePixmap.
 */
void
XWFreePixmap(Pixmap pixmap)
{
	XFreePixmap(display, pixmap);
}
/*
 * Handle any events that should be handled internally by this file,
 * such as window resizing, and return all other events to the caller.
 */
void
XWNextXEvent(XEvent *xevent)
{
	XNextEvent(display, xevent);
}
/*
 * Add a value to scroll, and ensure that scroll is never negative. Then, update the
 * window's name to reflect the current topmost line.
 */
void
XWMoveScroll(int nlines)
{
	char name[320];

	scroll += nlines;
	if (scroll < 0)
		scroll = 0;
	snprintf(name, sizeof(name), "%s - Line %d", title, scroll + 1);
	XStoreName(display, window, name);
}
/*
 * Initialize the X11 window and all static variables.
 */
int
XWInitWindow(char *name)
{
	title   = name;
	width   = 800;
	height  = 817;
	scroll  = 0;
	/*
	 * Connect to the X11 server.
	 */
	display = XOpenDisplay(NULL);
	if (display == NULL) {
		fprintf(stderr, "XWindow: failed to open display\n");
		return 1;
	}
	screen = DefaultScreen(display);
	depth  = DefaultDepth (display, screen);
	black  = BlackPixel   (display, screen);
	white  = WhitePixel   (display, screen);
	root   = RootWindow   (display, screen);
	/*
	 * Create a window.
	 */
	window = XCreateSimpleWindow(display,
			root,
			0, 0,
			width,
			height,
			0,     /* border width */
			black, /* border pixel */
			white  /* background   */);
	XSelectInput   (display, window, ExposureMask | KeyPressMask);
	XStoreName     (display, window, title);
	XMapWindow     (display, window);
	/*
	 * Create a graphics context.
	 */
	gc = XCreateGC(display, window, 0, 0);
	XSetBackground(display, gc, white);
	XSetForeground(display, gc, black);
	/*
	 * Load a font.
	 */
	font = XLoadQueryFont(display, XW_FONT_NAME);
	if (font == NULL) {
		fprintf(stderr, "XWindow: failed to load font\n");
		return 1;
	}
	XSetFont(display, gc, font->fid);

	return 0;
}
/*
 * Static function bodies:
 *****************************************************************************/
/*
 * Draw len characters from str as a single line of text.
 */
static void
DrawStr(char *str, size_t len, Pixmap pixmap, int y)
{
	size_t i;
	int x = 0;

	for (i = 0; i < len; ++i) {
		if (iscntrl(str[i]))
			continue;
		XDrawString(display, pixmap, gc, x, y, str + i, 1);
		x += TextOff(str + i, 1);
	}
}

/*
 * Return the width of the extents of len characters from str.
 */
static short
TextOff(char *str, size_t len)
{
	XCharStruct extents;
	int dir, asc, des;
	short off = 0;
	size_t i;

	for (i = 0; i < len; ++i) {
		if (iscntrl(str[i]))
			continue;
		XTextExtents(font, str + i, 1, &dir, &asc, &des, &extents);
		off += extents.width;
	}

	return off;
}
/*
 * Return the size of the next line. A line is a string with one or more whole
 * words, with the all the words fitting withing the width of the window if
 * there is more than one word.
 */
static size_t
LineLen(char* str)
{
	size_t line;
	size_t word;
	int off = 0;

	line = WordLen(str);
	off += TextOff(str, line);
	while (str[line] != '\n' && str[line] != '\0') {
		word = WordLen(str + line);
		off += TextOff(str + line, word);
		if (off >= width)
			break;
		line += word;
	}

	return line;
}
/*
 * Return the length of the next word. A word is either a series of 
 * graphical characters followed by a series of white spaces, or 
 * a single control character that is not a null byte.
 */
static size_t 
WordLen(char* str)
{
	size_t len = 1;

	if (str[0] == '\0')
		return 0;

	if (isgraph(str[0])) {
		for (; isgraph(str[len]); ++len);
		for (; ' ' ==  str[len];  ++len);
	}

	return len;
}
