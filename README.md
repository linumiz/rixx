
EN 

## Introduction

 This to help someone who need to integrate [FlashDB](https://github.com/armink/FlashDB) on Visual Studio Code on NCS SDK.
 The NCS SDK on Visual Studio use a Nordic customized version of zephyr and FlashDB can be integrate to zephyr as an external module.
 the easiest way to this is to use west command.

## Installing west
 make sure you have west installed. [see the how to install west](https://docs.nordicsemi.com/bundle/ncs-2.2.0/page/zephyr/develop/west/install.html) depending on your OS.

## Installing the nRF Connect SDK
 make sure you have nRF Connect SDK installed. [see the how to install the nRF Connect SDK](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/installation/install_ncs.html)

## Add FlashDB as external module
 To do this you must modify you **west.yml** file inside your the directory "nrf" locate in your sdk installed repository. for example in my case in installed the sdk verison **v2.8.0** on windows so my file is locate on **/ncs/v2.8.0/nrf**.
 open the **west.yml** file with your text editer and add the following lines in the **remotes** paragraph.

```shell

    #add FlashDB external module
    - name: flashDB
      url-base: https://github.com/armink
```

 Then in Projects paragraph add the following lines
 
```shell

    # FlashDB
    - name: FlashDB
      remote: flashDB
      revision: 523fd2052d13baf72fd1d8e177c2a162a03392be
      path: modules/lib/flashdb
```

save your **west.yml** file and the directory **/ncs/v2.8.0** in a terminal like PowerShell and hit the following west command to update your repository. 
```shell
    west update -n
```
after the update, you will find **flashdb** directory inside **/ncs/v2.8.0/modules/lib**

now clone the [rixx project](https://github.com/linumiz/rixx).
- copy the directory flashdb from **/rixx/modules/**  to  **/ncs/v2.8.0/zephyr/modules/**
- copy also the directory flashdb from **/rixx/sample/modules/** to **ncs/v2.8.0/zephyr/samples/modules/**

**Now you have FlashDB as nRF Connect SDK zephyr external module**.

## Usage 

as you can see in file **fdb_cfg.h** locate at **ncs/v2.8.0/zephyr/modules/flashdb/include**, FlashDB is configurate to use **file storage mode by POSIX file API**.
so you need to enable the **POSIX file system API support** with following declaration on your **prj.conf**
```shell

	CONFIG_MAIN_STACK_SIZE=2048

	# Filesystem Configs
	CONFIG_FILE_SYSTEM=y
	CONFIG_FILE_SYSTEM_LITTLEFS=y
	CONFIG_FS_LITTLEFS_NUM_FILES=10
	CONFIG_FS_LITTLEFS_FMP_DEV=y
	
	#POSIX Configs
	CONFIG_POSIX_API=y
	CONFIG_POSIX_FILE_SYSTEM=y
	CONFIG_POSIX_THREADS=y

	#in case you want to use file system in external falsh
	CONFIG_PM_PARTITION_REGION_LITTLEFS_EXTERNAL=y
	
	################### External Storage ##################
	CONFIG_SPI_NOR=y
	CONFIG_SPI_NOR_FLASH_LAYOUT_PAGE_SIZE=4096
	CONFIG_STREAM_FLASH=y	
	
	# Flashdb Configs
	CONFIG_FLASHDB=y
	CONFIG_FLASHDB_TIMESERIES=y
	CONFIG_FLASHDB_KEYVALUE=y
```

one thing to know is that nRF Connect SDK automaticaly enable partition manager.
so you need to explicitly inside **pm_static.yml** and you must have a partition named "littlefs_storage" when you whant use littlefs API.

for example a have an 1Mbit SPI flash (w25q80dv) in my nrf52832 custom board.
I want to use the first half part of this external flash for **mcuboot image storage** and second half part for **file system data storage**.
so my **pm_static.yml** file look like following.

```shell
app:
  address: 0xc200
  end_address: 0x7a000
  region: flash_primary
  size: 0x6e000
external_flash:
  address: 0x0
  end_address: 0x100000
  region: external_flash
  size: 0x100000
sram_primary:
  address: 0x20000000
  end_address: 0x20010000
  region: sram_primary
  size: 0x10000

mcuboot:
  address: 0x0
  end_address: 0xc000
  placement:
    before:
    - mcuboot_primary
  region: flash_primary
  size: 0xc000
mcuboot_pad:
  address: 0xc000
  end_address: 0xc200
  placement:
    align:
      start: 0x1000
    before:
    - mcuboot_primary_app
  region: flash_primary
  size: 0x200
mcuboot_primary:
  address: 0xc000
  end_address: 0x7a000
  orig_span: &id001
  - app
  - mcuboot_pad
  region: flash_primary
  sharers: 0x1
  size: 0x6e000
  span: *id001

mcuboot_primary_app:
  address: 0xc200
  end_address: 0x7a000
  orig_span: &id002
  - app
  region: flash_primary
  size: 0x6de00
  span: *id002

mcuboot_secondary:
  address: 0x0
  end_address: 0x6e000
  device: W25Q80DV
  region: external_flash
  share_size:
  - mcuboot_primary
  size: 0x6e000

lfs2_partition:
  address: 0x7a000
  end_address: 0x80000
  region: flash_primary
  size: 0x6000

littlefs_storage:
  address: 0x6e000
  end_address: 0x100000
  device: W25Q80DV
  region: external_flash
  size: 0x92000

```

you also may decribe your flash partitions and add ** Description of pre-defined file systems.** inside your project board overlay file to make access to them easier.

** my overlay file content looks like this **
```shell
/delete-node/ &boot_partition;
/delete-node/ &slot0_partition;
/delete-node/ &slot1_partition;
/delete-node/ &storage_partition;
/delete-node/ &externFlash_partition;

/ {
	chosen {
		nordic,pm-ext-flash = &w25q80dv;
	};

	fstab {
		compatible = "zephyr,fstab";
		lfs1: lfs1 {
			compatible = "zephyr,fstab,littlefs";
			mount-point = "/lfs1";
			partition = <&storage_partition>;
			automount;
			read-size = <16>;
			prog-size = <16>;
			cache-size = <64>;
			lookahead-size = <32>;
			block-cycles = <512>;
		};
	};	

};


&flash0 {
	partitions {
		compatible = "fixed-partitions";
		#address-cells = <1>;
		#size-cells = <1>;

		boot_partition: partition@0 {
			label = "mcuboot";
			reg = <0x0 0xc000>;
		};
		slot0_partition: partition@c000 {
			label = "image-0";
			reg = <0xc000 0x6e000>;
		};

		intern_storage_partition: partition@7a000{
			label = "storage-internal";
			reg = <0x7a000 0x6000>;
		};
	};
};

&w25q80dv {
	status = "okay";
	partitions {
		compatible = "fixed-partitions";
		#address-cells = <1>;
		#size-cells = <1>;

		slot1_partition: partition@0 {
			label = "image-1";
			reg = <0x0 0x6e000>;
		};

		/*
		 * Storage partition will be used by FCB/LittleFS/NVS
		 * if enabled.
		 */
		storage_partition: partition@6e000{
			label = "storage-external";
			reg = <0x6e000 0x92000>;
		};
	};
};
```

you may add following line to your project **CMakeLists.txt** file to avoid to copy  ** kvdb_basic_sample.c ,kvdb_type_blob_sample.c, kvdb_type_string_sample.c and tsdb_sample.c ** files to your project source location.
```shell
target_sources_ifdef(CONFIG_FLASHDB app PRIVATE 
  ${ZEPHYR_BASE}/samples/modules/flashdb/src/kvdb_basic_sample.c
  ${ZEPHYR_BASE}/samples/modules/flashdb/src/kvdb_type_blob_sample.c
  ${ZEPHYR_BASE}/samples/modules/flashdb/src/kvdb_type_string_sample.c
  ${ZEPHYR_BASE}/samples/modules/flashdb/src/tsdb_sample.c
)
```

now to test copy the **rixx\samples\modules\flashdb\src\main.c** code to your main code and this code after the include files.

```shell
LOG_MODULE_DECLARE(main, CONFIG_FS_LOG_LEVEL);
```
this allow to use log module functions.

now enjoy!
