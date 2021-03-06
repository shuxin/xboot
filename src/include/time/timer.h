#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <rbtree_augmented.h>
#include <rbtree.h>

struct timer_base_t;
struct timer_t;

enum timer_state_t {
	TIMER_STATE_INACTIVE = 0,
	TIMER_STATE_ENQUEUED = 1,
	TIMER_STATE_CALLBACK = 2,
};

struct timer_base_t {
	struct rb_root head;
	struct timer_t * next;
	struct clockevent_t * ce;
	spinlock_t lock;
	ktime_t (*gettime)(void);
};

struct timer_t {
	struct rb_node node;
	struct timer_base_t * base;
	enum timer_state_t state;
	ktime_t expires;
	void * data;
	int (*function)(struct timer_t *, void *);
};

void timer_init(struct timer_t * timer, int (*function)(struct timer_t *, void *), void * data);
void timer_start(struct timer_t * timer, ktime_t now, ktime_t interval);
void timer_start_now(struct timer_t * timer, ktime_t interval);
void timer_forward(struct timer_t * timer, ktime_t now, ktime_t interval);
void timer_forward_now(struct timer_t * timer, ktime_t interval);
void timer_cancel(struct timer_t * timer);
void subsys_init_timer(void);

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_H__ */
