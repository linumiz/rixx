/*
 * Copyright (c) 2024 Linumiz
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief configuration file
 */

#ifndef _FDB_CFG_H_
#define _FDB_CFG_H_

/* using KVDB feature */
#if defined CONFIG_FLASHDB_KEYVALUE
#define FDB_USING_KVDB

#ifdef FDB_USING_KVDB
/* Auto update KV to latest default when current KVDB version number is changed.
 * @see fdb_kvdb.ver_num */
/* #define FDB_KV_AUTO_UPDATE */
#endif

#endif /* CONFIG_FLASHDB_KEYVALUE */

#if defined CONFIG_FLASHDB_TIMESERIES

/* using TSDB (Time series database) feature */
#define FDB_USING_TSDB

#endif /* CONFIG_FLASHDB_TIMESERIES */

/* Using file storage mode by POSIX file API, like open/read/write/close */
/* For now, we are using in POSIX file mode for zephyr */
#define FDB_USING_FILE_POSIX_MODE

#if defined CONFIG_FLASHDB_DEBUG_ENABLE
/* log print macro. default EF_PRINT macro is printf() */
#define FDB_PRINT(...)              printk(__VA_ARGS__)

/* print debug information */
#define FDB_DEBUG_ENABLE

#endif /* CONFIG_FLASHDB_DEBUG_ENABLE */

#endif /* _FDB_CFG_H_ */
