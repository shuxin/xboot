/*
 * kernel/time/timer.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <clockevent/clockevent.h>
#include <time/timer.h>

static struct timer_base_t __timer_base;

static inline struct timer_t * next_timer(struct timer_base_t * base)
{
	return base->next;
}

static inline int add_timer(struct timer_base_t * base, struct timer_t * timer)
{
	struct rb_node ** p = &base->head.rb_node;
	struct rb_node * parent = NULL;
	struct timer_t * ptr;

	if(timer->state != TIMER_STATE_INACTIVE)
		return 0;

	while(*p)
	{
		parent = *p;
		ptr = rb_entry(parent, struct timer_t, node);
		if(timer->expires.tv64 < ptr->expires.tv64)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	rb_link_node(&timer->node, parent, p);
	rb_insert_color(&timer->node, &base->head);

	if(!base->next || timer->expires.tv64 < base->next->expires.tv64)
		base->next = timer;

	timer->state = TIMER_STATE_ENQUEUED;
	return (timer == base->next);
}

static inline int del_timer(struct timer_base_t * base, struct timer_t * timer)
{
	int ret = 0;

	if(timer->state != TIMER_STATE_ENQUEUED)
		return 0;

	if(base->next == timer)
	{
		struct rb_node * rbn = rb_next(&timer->node);
		base->next = rbn ? rb_entry(rbn, struct timer_t, node) : NULL;
		ret = 1;
	}
	rb_erase(&timer->node, &base->head);
	RB_CLEAR_NODE(&timer->node);

	timer->state = TIMER_STATE_INACTIVE;
	return ret;
}

void timer_init(struct timer_t * timer, int (*function)(struct timer_t *, void *), void * data)
{
	if(timer)
	{
		memset(timer, 0, sizeof(struct timer_t));
		RB_CLEAR_NODE(&timer->node);
		timer->base = &__timer_base;
		timer->state = TIMER_STATE_INACTIVE;
		timer->data = data;
		timer->function = function;
	}
}

void timer_start(struct timer_t * timer, ktime_t now, ktime_t interval)
{
	struct timer_base_t * base = timer->base;
	irq_flags_t flags;

	if(!timer)
		return;

	spin_lock_irqsave(&base->lock, flags);
	if(del_timer(base, timer))
	{
		struct timer_t * next = next_timer(base);
		if(next)
			clockevent_set_event_next(base->ce, base->gettime(), next->expires);
	}
	ktime_t expires = ktime_add_safe(now, interval);
	memcpy(&timer->expires, &expires, sizeof(ktime_t));
	if(add_timer(base, timer))
		clockevent_set_event_next(base->ce, base->gettime(), timer->expires);
	spin_unlock_irqrestore(&base->lock, flags);
}

void timer_start_now(struct timer_t * timer, ktime_t interval)
{
	if(timer)
		timer_start(timer, timer->base->gettime(), interval);
}

void timer_forward(struct timer_t * timer, ktime_t now, ktime_t interval)
{
	if(timer)
	{
		ktime_t expires = ktime_add_safe(now, interval);
		memcpy(&timer->expires, &expires, sizeof(ktime_t));
	}
}

void timer_forward_now(struct timer_t * timer, ktime_t interval)
{
	if(timer)
		timer_forward(timer, timer->base->gettime(), interval);
}

void timer_cancel(struct timer_t * timer)
{
	struct timer_base_t * base = timer->base;
	irq_flags_t flags;

	if(!timer)
		return;

	spin_lock_irqsave(&base->lock, flags);
	if(del_timer(base, timer))
	{
		struct timer_t * next = next_timer(base);
		if(next)
			clockevent_set_event_next(base->ce, base->gettime(), next->expires);
	}
	spin_unlock_irqrestore(&base->lock, flags);
}

static void timer_event_handler(struct clockevent_t * ce, void * data)
{
	struct timer_base_t * base = (struct timer_base_t *)(data);
	struct timer_t * timer;
	ktime_t now = base->gettime();
	irq_flags_t flags;
	int restart;

	spin_lock_irqsave(&base->lock, flags);
	while((timer = next_timer(base)))
	{
		if(now.tv64 < timer->expires.tv64)
			break;

		del_timer(base, timer);
		timer->state = TIMER_STATE_CALLBACK;
		restart = timer->function(timer, timer->data);
		timer->state = TIMER_STATE_INACTIVE;
		if(restart)
			add_timer(base, timer);
	}
	if((timer = next_timer(base)))
		clockevent_set_event_next(ce, now, timer->expires);
	spin_unlock_irqrestore(&base->lock, flags);
}

void subsys_init_timer(void)
{
	memset(&__timer_base, 0, sizeof(struct timer_base_t));
	spin_lock_init(&__timer_base.lock);
	__timer_base.head = RB_ROOT;
	__timer_base.next = NULL;
	__timer_base.ce = clockevent_best();
	__timer_base.gettime = ktime_get;
	clockevent_set_event_handler(__timer_base.ce, timer_event_handler, &__timer_base);
}
