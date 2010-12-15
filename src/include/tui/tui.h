#ifndef __TUI_H__
#define __TUI_H__

#include <configs.h>
#include <default.h>
#include <console/console.h>


#define TUI_WIDGET(widget)			( (struct tui_widget *)(widget) )

/*
 * forward declare
 */
struct tui_widget;

enum tui_widget_align
{
	TUI_WIDGET_ALIGN_NONE			= 0,
	TUI_WIDGET_ALIGN_LEFT			= 1,
	TUI_WIDGET_ALIGN_TOP			= 2,
	TUI_WIDGET_ALIGN_RIGHT			= 3,
	TUI_WIDGET_ALIGN_BOTTOM			= 4,
	TUI_WIDGET_ALIGN_ALL			= 5,
};

struct tui_cell
{
	/* code pointer */
	x_u32 cp;

	/* foreground color */
	enum tcolor fg;

	/* background color */
	enum tcolor bg;

	/* dirty flag */
	x_bool dirty;
};

struct tui_event
{
	x_bool valid;
};

struct tui_widget_ops
{
	/* get minimal width and height */
	x_bool (*minsize)(struct tui_widget * widget, x_s32 * width, x_s32 * height);

	/* get a valid usable region */
	x_bool (*region)(struct tui_widget * widget, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h);

	/* set bounds */
	x_bool (*setbounds)(struct tui_widget * widget, x_s32 ox, x_s32 oy, x_s32 width, x_s32 height);

	/* get bounds */
	x_bool (*getbounds)(struct tui_widget * widget, x_s32 * ox, x_s32 * oy, x_s32 * width, x_s32 * height);

	/* set widget property */
	x_bool (*setproperty)(struct tui_widget * widget, x_u32 cmd, void * arg);

	/* paint widget with area */
	x_bool (*paint)(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h);

	/* process events */
	x_bool (*process)(struct tui_widget * widget, struct tui_event * event);

	/* destroy widget */
	x_bool (*destroy)(struct tui_widget * widget);
};

struct tui_widget
{
	/* widget's id */
	x_s8 * id;

	/* widget's alignment */
	enum tui_widget_align align;

	/* widget's offset with relative to parent widget */
	x_s32 ox, oy;

	/* widget's width and height */
	x_s32 width, height;

	/* widget focus flag */
	x_bool focus;

	/* tui cell buffer length */
	x_s32 clen;

	/* tui cell buffer */
	struct tui_cell * cell;

	/* widget operations */
	struct tui_widget_ops * ops;

	/* the widget entry */
	struct list_head entry;

	/* the parent widget */
	struct tui_widget * parent;

	/* child widget list head */
	struct list_head child;

	/* priv data */
	void * priv;
};


struct tui_widget * find_tui_widget_by_id(struct tui_widget * widget, const x_s8 * id);
x_bool tui_widget_setparent(struct tui_widget * widget, struct tui_widget * parent);
struct tui_widget * tui_widget_getparent(struct tui_widget * widget);
x_bool tui_widget_addchild(struct tui_widget * widget, struct tui_widget * child);
x_bool tui_widget_removechild(struct tui_widget * widget, struct tui_widget * child);
x_bool tui_widget_alignment(struct tui_widget * widget, enum tui_widget_align align);
x_bool tui_widget_minsize(struct tui_widget * widget, x_s32 * width, x_s32 * height);
x_bool tui_widget_region(struct tui_widget * widget, x_s32 * x, x_s32 * y, x_s32 * w, x_s32 * h);
x_bool tui_widget_setbounds(struct tui_widget * widget, x_s32 ox, x_s32 oy, x_s32 width, x_s32 height);
x_bool tui_widget_getbounds(struct tui_widget * widget, x_s32 * ox, x_s32 * oy, x_s32 * width, x_s32 * height);
x_bool tui_widget_getpos(struct tui_widget * widget, x_s32 * x, x_s32 * y);
x_bool tui_widget_setproperty(struct tui_widget * widget, x_u32 cmd, void * arg);
x_bool tui_widget_paint(struct tui_widget * widget, x_s32 x, x_s32 y, x_s32 w, x_s32 h);
x_bool tui_widget_destroy(struct tui_widget * widget);

#endif /* __TUI_H__ */