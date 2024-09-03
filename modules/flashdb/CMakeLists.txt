# Copyright (c) 2024 Linumiz
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_FLASHDB)

zephyr_interface_library_named(FLASHDB)

zephyr_include_directories(include)
zephyr_include_directories(${ZEPHYR_FLASHDB_MODULE_DIR}/inc)

zephyr_library()
zephyr_library_sources(
	${ZEPHYR_FLASHDB_MODULE_DIR}/src/fdb.c
	${ZEPHYR_FLASHDB_MODULE_DIR}/src/fdb_file.c
	${ZEPHYR_FLASHDB_MODULE_DIR}/src/fdb_kvdb.c
	${ZEPHYR_FLASHDB_MODULE_DIR}/src/fdb_tsdb.c
	${ZEPHYR_FLASHDB_MODULE_DIR}/src/fdb_utils.c
)

zephyr_library_link_libraries(FLASHDB)
target_link_libraries(FLASHDB INTERFACE zephyr_interface)

endif()
