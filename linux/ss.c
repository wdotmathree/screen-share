#include "ss.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

static Display *disp;
static Window root;
static int width, height;

int ss_init(void) {
	disp = XOpenDisplay(NULL);
	if (!disp)
		return -1;
	root = DefaultRootWindow(disp);
	XWindowAttributes attr;
	XGetWindowAttributes(disp, root, &attr);
	width = attr.width;
	height = attr.height;
	return 0;
}

int ss_get_dimensions(int *w, int *h) {
	if (disp == NULL)
		return -1;
	*w = width;
	*h = height;
	return 0;
}

int ss_get_screenshot(char *buf) {
	XImage *img = XGetImage(disp, root, 0, 0, width, height, AllPlanes, ZPixmap);
	if (!img)
		return -1;
	int len = img->bytes_per_line * img->height;
	memcpy(buf, img->data, len);
	XDestroyImage(img);
	return len;
}

int ss_get_width(void) {
	return width;
}

int ss_get_height(void) {
	return height;
}
