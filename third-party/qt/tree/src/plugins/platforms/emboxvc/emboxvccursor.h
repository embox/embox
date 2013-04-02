#ifndef QEMBOXVCCURSOR_H
#define QEMBOXVCCURSOR_H

#include <QtGui/QPlatformWindow>
#include <QtGui/QImage>
#include <drivers/video/fb.h>
#include <string.h>
#include <stdio.h>

QT_BEGIN_NAMESPACE

class QEmboxCursor
{
public:
	QEmboxCursor();
    ~QEmboxCursor();

	void emboxCursorRedraw(struct fb_info *fb, int x, int y);
	void emboxCursorReset(struct fb_info *fb);

private:
	void storeDirtyRect(struct fb_info *fb, unsigned char *begin);
	void flushDirtyRect(struct fb_info *fb, unsigned char *begin);
	void drawCursor(struct fb_info *fb, unsigned char *begin);
	int imageChanged(struct fb_info *fb, unsigned char *begin);

    int mouseX, mouseY;
    int inited;
    QImage cursor;
    int cursor_H, cursor_W;
    unsigned char *dirtyRect;
};

QT_END_NAMESPACE

#endif
