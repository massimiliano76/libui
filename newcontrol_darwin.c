// 7 april 2015
#include "uipriv_darwin.h"

typedef struct uiSingleWidgetControl uiSingleWidgetControl;

struct uiSingleViewControl {
	uiControl control;
	NSView *control;
	NSScrollView *scrollView;
	NSView *immediate;		// the control that is added to the parent container; either control or scrollView
	void *data;
};

#define S(c) ((uiSingleViewControl *) (c))

static uintptr_t singleHandle(uiControl *c)
{
	return (uintptr_t) (S(c)->control);
}

static void singleSetParent(uiControl *c, uintptr_t parent)
{
	NSView *parentView = (NSView *) parent;

	[parentView addSubview:S(c)->immediate];
}

static uiSize singlePreferredSize(uiControl *c, uiSizing *d)
{
	uiSize size;

	// TODO
	size.width = size.height = 0;
	return size;
}

static void singleResize(uiControl *c, intmax_t x, intmax_t y, intmax_t width, intmax_t height, uiSizing *d)
{
	NSRect frame;

	frame.origin.x = x;
	frame.origin.y = y;
	frame.size.width = width;
	frame.size.height = height;
	frame = [S(c)->immediate frameForAlignmentRect:frame];
	[S(c)->immediate setFrame:frame];
}

static void singleContainerShow(uiControl *c)
{
	[S(c)->immediate setHidden:NO];
}

static void singleContainerHide(uiControl *c)
{
	[S(c)->immediate setHidden:YES];
}

// TODO connect free function

uiControl *uiDarwinNewControl(Class class, BOOL inScrollView, BOOL scrollViewHasBorder, void *data)
{
	uiSingleViewControl *c;

	c = g_new0(uiSingleViewControl, 1);
	// thanks to autoxr and arwyn in irc.freenode.net/#macdev
	c->widget = (NSView *) [[class alloc] initWithFrame:NSZeroRect];
	c->immediate = c->control;

	// TODO turn into bit field?
	if (inScrollView) {
		c->scrollView = [[NSScrollView alloc] initWithFrame:NSZeroRect];
		[c->scrollView setDocumentView:c->control];
		[c->scrollView setHasHorizontalScroller:YES];
		[c->scrollView setHasVerticalScroller:YES];
		[c->scrollView setAutohidesScrollers:YES];
		if (scrollViewHasBorder)
			[c->scrollView setBorderType:NSBezelBorder];
		else
			[c->scrollView setBorderType:NSNoBorder];
		c->immediate = (NSView *) (c->scrolledWindow);
	}

	c->control.handle = singleHandle;
	c->control.setParent = singleSetParent;
	c->control.preferredSize = singlePreferredSize;
	c->control.resize = singleResize;
	c->control.containerShow = singleContainerShow;
	c->control.containerHide = singleContainerHide;

	c->data = data;

	return (uiControl *) c;
}

void *uiDarwinControlData(uiControl *c)
{
	return S(c)->data;
}
