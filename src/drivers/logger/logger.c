/*
 * drivers/logger/logger.c
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
#include <spinlock.h>
#include <logger/logger.h>

struct logger_list_t
{
	struct logger_t * logger;
	struct list_head entry;
};

static struct logger_list_t __logger_list = {
	.entry = {
		.next	= &(__logger_list.entry),
		.prev	= &(__logger_list.entry),
	},
};
static spinlock_t __logger_list_lock = SPIN_LOCK_INIT();

static struct logger_t * search_logger(const char * name)
{
	struct logger_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__logger_list.entry), entry)
	{
		if(strcmp(pos->logger->name, name) == 0)
			return pos->logger;
	}

	return NULL;
}

bool_t register_logger(struct logger_t * logger)
{
	struct logger_list_t * ll;
	irq_flags_t flags;
	int i;

	if(!logger || !logger->name)
		return FALSE;

	if(search_logger(logger->name))
		return FALSE;

	ll = malloc(sizeof(struct logger_list_t));
	if(!ll)
		return FALSE;

	if(logger->init)
		(logger->init)(logger);

	if(logger->output)
	{
		for(i = 0; i < 5; i++)
		{
			logger->output(logger, xboot_character_logo_string(i), strlen(xboot_character_logo_string(i)));
			logger->output(logger, "\r\n", 2);
		}
		logger->output(logger, xboot_banner_string(), strlen(xboot_banner_string()));
		logger->output(logger, "\r\n", 2);
	}

	ll->logger = logger;

	spin_lock_irqsave(&__logger_list_lock, flags);
	list_add_tail(&ll->entry, &(__logger_list.entry));
	spin_unlock_irqrestore(&__logger_list_lock, flags);

	return TRUE;
}

bool_t unregister_logger(struct logger_t * logger)
{
	struct logger_list_t * pos, * n;
	irq_flags_t flags;

	if(!logger || !logger->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__logger_list.entry), entry)
	{
		if(pos->logger == logger)
		{
			spin_lock_irqsave(&__logger_list_lock, flags);
			list_del(&(pos->entry));
			spin_unlock_irqrestore(&__logger_list_lock, flags);

			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

int logger_print(const char * fmt, ...)
{
	struct logger_list_t * pos, * n;
	va_list ap;
	struct timeval tv;
	char buf[SZ_4K];
	int len = 0;

	va_start(ap, fmt);
	gettimeofday(&tv, 0);
	len += sprintf((char *)(buf + len), "[%5u.%06u]", tv.tv_sec, tv.tv_usec);
	len += vsnprintf((char *)(buf + len), (SZ_4K - len), fmt, ap);
	va_end(ap);

	list_for_each_entry_safe(pos, n, &(__logger_list.entry), entry)
	{
		if(pos->logger->output)
			pos->logger->output(pos->logger, (const char *)buf, len);
	}
	return len;
}
EXPORT_SYMBOL(logger_print);
