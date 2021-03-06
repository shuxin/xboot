/*
 * driver/clocksource/clocksource.c
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

#include <xboot.h>
#include <clocksource/clocksource.h>

struct clocksource_list_t
{
	struct clocksource_t * cs;
	struct list_head entry;
};

struct clocksource_list_t __clocksource_list = {
	.entry = {
		.next	= &(__clocksource_list.entry),
		.prev	= &(__clocksource_list.entry),
	},
};
static spinlock_t __clocksource_list_lock = SPIN_LOCK_INIT();

/*
 * Dummy clocksource, 10us - 100KHZ
 */
static bool_t __cs_dummy_init(struct clocksource_t * cs)
{
	return TRUE;
}

static u64_t __cs_dummy_read(struct clocksource_t * cs)
{
	static volatile u64_t __cs_dummy_cycle = 0;
	return __cs_dummy_cycle++;
}

static struct clocksource_t __cs_dummy = {
	.name	= "dummy",
	.mult	= 2621440000,
	.shift	= 18,
	.mask	= CLOCKSOURCE_MASK(64),
	.init	= __cs_dummy_init,
	.read	= __cs_dummy_read,
};

static struct kobj_t * search_class_clocksource_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "clocksource");
}

static ssize_t clocksource_read_mult(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	return sprintf(buf, "%u", cs->mult);
}

static ssize_t clocksource_read_shift(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	return sprintf(buf, "%u", cs->shift);
}

static ssize_t clocksource_read_period(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	u64_t period = ((u64_t)cs->mult) >> cs->shift;
	return sprintf(buf, "%llu.%09llu", period / 1000000000ULL, period % 1000000000ULL);
}

static ssize_t clocksource_read_deferment(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	u64_t max = clocksource_deferment(cs);
	return sprintf(buf, "%llu.%09llu", max / 1000000000ULL, max % 1000000000ULL);
}

static ssize_t clocksource_read_cycle(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	return sprintf(buf, "%llu", clocksource_cycle(cs));
}

static ssize_t clocksource_read_time(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	u64_t cycle = clocksource_cycle(cs);
	u64_t time = clocksource_delta2ns(cs, cycle);
	return sprintf(buf, "%llu.%09llu", time / 1000000000ULL, time % 1000000000ULL);
}

struct clocksource_t * search_clocksource(const char * name)
{
	struct clocksource_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__clocksource_list.entry), entry)
	{
		if(strcmp(pos->cs->name, name) == 0)
			return pos->cs;
	}

	return NULL;
}

bool_t register_clocksource(struct clocksource_t * cs)
{
	struct clocksource_list_t * cl;
	irq_flags_t flags;

	if(!cs || !cs->name)
		return FALSE;

	if(!cs->init || !cs->read)
		return FALSE;

	if(search_clocksource(cs->name))
		return FALSE;

	cl = malloc(sizeof(struct clocksource_list_t));
	if(!cl)
		return FALSE;

	cs->kobj = kobj_alloc_directory(cs->name);
	kobj_add_regular(cs->kobj, "mult", clocksource_read_mult, NULL, cs);
	kobj_add_regular(cs->kobj, "shift", clocksource_read_shift, NULL, cs);
	kobj_add_regular(cs->kobj, "period", clocksource_read_period, NULL, cs);
	kobj_add_regular(cs->kobj, "deferment", clocksource_read_deferment, NULL, cs);
	kobj_add_regular(cs->kobj, "cycle", clocksource_read_cycle, NULL, cs);
	kobj_add_regular(cs->kobj, "time", clocksource_read_time, NULL, cs);
	kobj_add(search_class_clocksource_kobj(), cs->kobj);
	cl->cs = cs;

	spin_lock_irqsave(&__clocksource_list_lock, flags);
	list_add_tail(&cl->entry, &(__clocksource_list.entry));
	spin_unlock_irqrestore(&__clocksource_list_lock, flags);

	return TRUE;
}

bool_t unregister_clocksource(struct clocksource_t * cs)
{
	struct clocksource_list_t * pos, * n;
	irq_flags_t flags;

	if(!cs || !cs->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__clocksource_list.entry), entry)
	{
		if(pos->cs == cs)
		{
			spin_lock_irqsave(&__clocksource_list_lock, flags);
			list_del(&(pos->entry));
			spin_unlock_irqrestore(&__clocksource_list_lock, flags);

			kobj_remove(search_class_clocksource_kobj(), pos->cs->kobj);
			kobj_remove_self(cs->kobj);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

inline __attribute__((always_inline)) struct clocksource_t * clocksource_dummy(void)
{
	return &__cs_dummy;
}

struct clocksource_t * clocksource_best(void)
{
	struct clocksource_t * cs, * best = &__cs_dummy;
	struct clocksource_list_t * pos, * n;
	u64_t period = ~0ULL;
	u64_t t;

	list_for_each_entry_safe(pos, n, &(__clocksource_list.entry), entry)
	{
		cs = pos->cs;
		if(!cs || !cs->init || !cs->read)
			continue;

		if(!cs->init(cs))
			continue;

		t = ((u64_t)cs->mult) >> cs->shift;
		if(t < period)
		{
			best = cs;
			period = t;
		}
	}
	return best;
}
