#ifndef __XBOOT_CONFIGS_H__
#define __XBOOT_CONFIGS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <configs.h>
#include <endian.h>

#if !defined(__ARCH__)
#define __ARCH__							"x64"
#endif

#if !defined(__MACH__)
#define __MACH__							"sandbox"
#endif

#if !defined(CONFIG_NO_LOG)
#define CONFIG_NO_LOG						(0)
#endif

#if !defined(CONFIG_AUTO_BOOT_DELAY)
#define CONFIG_AUTO_BOOT_DELAY				(1)
#endif

#if !defined(CONFIG_AUTO_BOOT_COMMAND)
#define CONFIG_AUTO_BOOT_COMMAND			""
#endif

#if !defined(CONFIG_MAX_BRIGHTNESS)
#define CONFIG_MAX_BRIGHTNESS				(1024 - 1)
#endif

#if !defined(CONFIG_EVENT_FIFO_LENGTH)
#define CONFIG_EVENT_FIFO_LENGTH			(256)
#endif

#if !defined(CONFIG_MAX_NUMBER_OF_VFS_BIO)
#define CONFIG_MAX_NUMBER_OF_VFS_BIO		(SZ_4K)
#endif

#if !defined(CONFIG_CMDLINE_LENGTH)
#define CONFIG_CMDLINE_LENGTH				(SZ_4K)
#endif

#if !defined(CONFIG_VARNAME_LENGTH)
#define CONFIG_VARNAME_LENGTH				(256)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __XBOOT_CONFIGS_H__ */
