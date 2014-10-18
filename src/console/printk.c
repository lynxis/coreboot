/*
 *  blatantly copied from linux/kernel/printk.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 */

#include <console/console.h>
#include <console/streams.h>
#include <console/vtxprintf.h>
#include <smp/spinlock.h>
#include <smp/node.h>
#include <stddef.h>
#include <trace.h>
#include <stdlib.h>

DECLARE_SPIN_LOCK(console_lock)

void do_putchar(unsigned char byte)
{
	if (byte == '\n')
		console_tx_byte('\r');
	console_tx_byte(byte);
}

static void wrap_putchar(unsigned char byte, void *data)
{
	do_putchar(byte);
}

int do_printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	if (!console_log_level(msg_level))
		return 0;

#if IS_ENABLED (CONFIG_SQUELCH_EARLY_SMP) && defined(__PRE_RAM__)
	if (!boot_cpu())
		return 0;
#endif

	DISABLE_TRACE;
	spin_lock(&console_lock);

	va_start(args, fmt);
	i = vtxprintf(wrap_putchar, fmt, args, NULL);
	va_end(args);

	console_tx_flush();

	spin_unlock(&console_lock);
	ENABLE_TRACE;

	return i;
}

#if IS_ENABLED (CONFIG_CHROMEOS)
void do_vtxprintf(const char *fmt, va_list args)
{
	vtxprintf(wrap_putchar, fmt, args, NULL);
	console_tx_flush();
}
#endif /* CONFIG_CHROMEOS */

#ifdef __PRE_RAM__
/* sadly, pretty printing the call depth in pre-ram is not an option. How sad! */

void __print_func_entry(const char *func, const char *file)
{
	printk(BIOS_EMERG, "ENTER %s() in %s\n", func, file);
}
/* for tracing. You can call these on func entry and exit.
 * Or, better, you can use the spatch in utils to add
 * such tracing.
 */
void __print_func_exit(const char *func, const char *file)
{
	printk(BIOS_EMERG, "EXIT ---- %s()\n", func);
}

#else
int tab_depth = 0;

void __print_func_entry(const char *func, const char *file)
{
	char tentabs[] = "\t\t\t\t\t\t\t\t\t\t"; // ten tabs and a \0
	char *ourtabs = &tentabs[10 - MIN(tab_depth, 10)];
	printk(BIOS_EMERG, "%s%s() in %s\n", ourtabs, func, file);
	tab_depth++;
}
/* for tracing. You can call these on func entry and exit.
 * Or, better, you can use the spatch in utils to add
 * such tracing.
 */
void __print_func_exit(const char *func, const char *file)
{
	char tentabs[] = "\t\t\t\t\t\t\t\t\t\t"; // ten tabs and a \0
	char *ourtabs;
	tab_depth--;
	ourtabs = &tentabs[10 - MIN(tab_depth, 10)];
	printk(BIOS_EMERG, "%s---- %s()\n", ourtabs, func);
}

#endif
