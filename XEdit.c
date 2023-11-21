/*
 * XEdit.c
 *****************************************************************************/
#include "XWindow.h"

void
die()
{
	perror("xedit");
	exit(1);
}

char*
load_file(char *filename)
{
	FILE *fp;
	size_t len;
	char *buf;

	fp = fopen(filename, "r");
	if (fp == NULL)
		die();
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buf = malloc(len + 1);
	if (buf == NULL)
		die();
	if (fread(buf, sizeof(char), len, fp) != len) {
		die();
	}
	buf[len] = '\0';
	fclose(fp);

	return buf;
}

int
main(int argc, char **argv)
{
	Pixmap pixmap;
	XEvent xevent;
	KeySym keysym;
	char  *buffer;

	buffer = load_file(argv[1]);
	if (XWInitWindow(argv[1])) {
		exit(1);
	}
	XWMakePixmap(&pixmap, 100);
	XWDrawString(pixmap, buffer);

	while (1) {
		XWNextXEvent(&xevent);
		switch (xevent.type) {
		case KeyPress:
			keysym = XLookupKeysym(&xevent.xkey, 0);
			switch (keysym) {
			case XK_Up:
				XWMoveScroll(-1);
				break;
			case XK_Down:
				XWMoveScroll(1);
				break;
			}
			/* fallthrough */
		case Expose:
			XWDrawPixmap(pixmap);
		}
	}

	return 0;
}
