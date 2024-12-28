# Tuya Linux SDK
# Copyright (C) 2020 Tuya Smart, Inc.

SHELL := /bin/bash

export TZ = UTC
export LANG = C
export LC_ALL = C

-include .local.conf
-include .config
-include build.conf
include build/definitions.mk

###############################################
#                 global env                  #
###############################################
TOPDIR := $(CURDIR)

BUILD_SYSTEM := $(TOPDIR)/build

include $(BUILD_SYSTEM)/env.mk

###############################################
#                 rules                       #
###############################################
.PHONY: all
#all:target-images
all:all_modules install_includes

TUYA_SDK_INC :=				 # 加载至CFLAGS中提供给其他组件使用；打包进SDK产物中；
TUYA_SDK_CFLAGS :=			 # 模块对外CFLAGS：其他组件编译时可感知到

XMAKE_OUTDIR := $(TARGET_OUT)
XMAKE_TARGET_DIR := $(TARGET_ROOTFS)
include $(BUILD_SYSTEM)/xmake.mk
include $(BUILD_SYSTEM)/images.mk
include $(shell find components -type f -name local.mk)

#TuyaOS兼容
KERNEL_INC_ALL_SUBDIRS := $(foreach dir,$(TUYA_SDK_INC),$(shell find $(dir) -type d))
KERNEL_INC_ALL_SUBDIRS += $(shell find $(TOPDIR)/../../../adapter -name include)
KERNEL_INC_ALL_SUBDIRS += $(TOPDIR)/../../../include/base/include
CFLAGS += $(addprefix -I,$(KERNEL_INC_ALL_SUBDIRS))
CFLAGS += $(TUYA_SDK_CFLAGS)

.PHONY: all_modules

all_modules: $(ALL_MODULES)
	@mkdir -p $(TARGET_BSP_SDK_INCS_DIR) $(TARGET_BSP_SDK_LIBS_DIR) $(TARGET_KMOD_DIR)


install_includes:
	@$(foreach inc,$(KERNEL_INC_ALL_SUBDIRS),$(shell cp -arf $(inc)/* $(TARGET_BSP_SDK_INCS_DIR)/))


$(TARGET_ROOTFS): target-prepare

.PHONY: target-prepare
target-prepare: $(TARGET_OUT)/.rootfs $(IOTOS_AUTOHEADER)
	$(foreach hook,$(ALL_PREPARE_HOOKS),$($(hook))$(sep))

$(TARGET_OUT)/.rootfs:
	@mkdir -p $(DOWNLOAD_DIR) $(DIST_DIR) $(TARGET_RELEASE) $(TARGET_IMAGES) $(TARGET_ROOTFS) \
		$(TARGET_BSP_SDK_INCS_DIR) $(TARGET_BSP_SDK_LIBS_DIR) $(TARGET_KMOD_DIR)
	@ln -snf $(TARGET_SYSROOT) $(TARGET_OUT)/sysroot
	$(call do_rsync,$(TARGET_SKELETON),$(TARGET_ROOTFS))
	touch $@


.PHONY: target-images
target-images: target-rootfs
	$(foreach hook,$(ALL_IMAGE_BUILDER_HOOKS),$($(hook))$(sep))

.PHONY: target-rootfs
target-rootfs: target-finalize

.PHONY: target-finalize
target-finalize: $(TARGET_ROOTFS) all_modules
	@$(foreach hook,$(ALL_FINALIZE_HOOKS),$($(hook))$(sep))
	@$(foreach d, $(call qstrip,$(CONFIG_TUYA_ROOTFS_OVERLAY)), \
		$(call do_rsync,$(d),$(TARGET_ROOTFS))$(sep))
	@$(foreach inc,$(KERNEL_INC_ALL_SUBDIRS),$(shell cp -a $(inc)/* $(TARGET_BSP_SDK_INCS_DIR)))
	@rm -rf $(TARGET_ROOTFS)/usr/include $(TARGET_ROOTFS)/usr/share/aclocal \
		$(TARGET_ROOTFS)/usr/lib/pkgconfig $(TARGET_ROOTFS)/usr/share/pkgconfig \
		$(TARGET_ROOTFS)/usr/lib/cmake $(TARGET_ROOTFS)/usr/share/cmake \
		$(TARGET_ROOTFS)/usr/share/doc $(TARGET_ROOTFS)/usr/share/gtk-doc \
		$(TARGET_ROOTFS)/usr/share/man $(TARGET_ROOTFS)/usr/man $(TARGET_ROOTFS)/usr/share/bash-completion \
		$(TARGET_ROOTFS)/usr/share/info
	@find $(TARGET_ROOTFS)/usr/{lib,share}/ -name '*.cmake' -print0 | xargs -0 rm -f
	@find $(TARGET_ROOTFS)/lib/ $(TARGET_ROOTFS)/usr/lib/ $(TARGET_ROOTFS)/usr/libexec/ \
		\( -name '*.a' -o -name '*.la' -o -name '*.prl' \) -print0 | xargs -0 rm -f
#	@STRIP=$(STRIP) SYSROOT=$(TARGET_SYSROOT) $(BUILD_SYSTEM)/scripts/rstrip.sh $(TARGET_ROOTFS)

%:: target-prepare $(TARGET_OUT)/.objs/package/%/.tuya-build
	@echo "Building $@ successfully"

pack:
	@if [ -d $(OUT_DIR)/$(TARGET_PRODUCT)/release ]; then \
		PACKAGE_FULLNAME=$(CI_PACKAGE_FULLNAME) ;\
		if [ -z $${PACKAGE_FULLNAME} ];then \
			PACKAGE_FULLNAME=$(DIST_DIR)/$(TARGET_PLATFORM)_$(TARGET_PRODUCT)_$(TARGET_BUILD_VERSION).tar.gz;\
		fi; \
		FLASH_SIZE=`printf "%d" $(CONFIG_TUYA_TARGET_FLASH_SIZE) | awk '{ byte =$$1 /1024/1024 ; print byte "m" }'`;\
		if [ -z $(CONFIG_TUYA_TARGET_MILESTONE) ]; then\
			MS="v1"; \
		else \
			MS=$(CONFIG_TUYA_TARGET_MILESTONE);\
		fi; \
		RELEASE_NAME=$(CONFIG_TUYA_PRODUCT_RELEASE_NAME)_$${FLASH_SIZE}_$${MS}; \
		cd $(OUT_DIR)/$(TARGET_PRODUCT); \
		tar -zcvf $${PACKAGE_FULLNAME} release  --transform=s/release/$${RELEASE_NAME}/; \
	fi

fw:
	@if [ -d $(TARGET_RELEASE) ]; then \
		user_fs=`cat $(TARGET_RELEASE)/partitions.json | jq '.partitions[] | select(.name == "user").filesystem'` ;\
		user_img=`cat $(TARGET_RELEASE)/partitions.json | jq '.partitions[] | select(.name == "user").filename' | tr -d '"'` ;\
		if [ -d $(TARGET_OUT)/testfs ]; then \
			mksquashfs $(TARGET_OUT)/testfs $(TARGET_RELEASE)/$${user_img} -noappend -root-owned -comp xz ;\
		fi; \
		cd $(TARGET_RELEASE) ;\
		./build.sh ;\
	fi

%-clean: $(TARGET_OUT)/.objs/package/%
	@rm -rf $<

clean:
	@rm -rf $(TARGET_OUT)
	@find $(TOPDIR)/components/base_bsp/package -name "source" -type d -print0 | xargs -0 rm -rf

distclean:
	@rm -rf $(OUT_DIR)
	@rm -rf .config*
	@rm -rf .local.conf

###############################################
#                 menuconfig                  #
###############################################
.PHONY: prepare-config
prepare-config:
	@mkdir -p $(dir $(IOTOS_AUTOHEADER))

TUYA_CONFIG := $(TOPDIR)/.config
DEFCONFIG = $(shell grep TUYA_DEFCONFIG $(TUYA_CONFIG) | cut -d= -f2)
COMMON_CONFIG_ENV = \
	TUYA_PRODUCT=$(TARGET_PRODUCT)

$(IOTOS_AUTOHEADER): $(BUILD_SYSTEM)/kconfiglib/conf2h.py
	@mkdir -p $(@D)
	@$(COMMON_CONFIG_ENV) KCONFIG_AUTOHEADER=$(IOTOS_AUTOHEADER) $< $(TUYA_CONFIG) config.h
	@sed \
		-e 's%@BUILD_DATE@%$(BUILD_DATE)%' \
		-e 's%@BUILD_TIME@%$(BUILD_TIME)%' \
		-e 's%@USER@%$(GIT_USER)%' \
		-e 's%@KERNEL_VER@%$(IOT_SDK_VER)%' \
		-e 's%@KERNEL_BETA_VER@%$(SDK_BETA_VER)%' \
		-e 's%@KERNEL_PLATFORM@%$(TARGET_PLATFORM)%' \
		-e 's%@KERNEL_FULL_ID@%$(SDK_ID)%' \
		-e '/@CUSTOM_SETTINGS@/ r config.h' \
		-e '/@CUSTOM_SETTINGS@/d' \
		$(BUILD_SYSTEM)/tuya_iot_config.h.in > $@
	@rm -rf config.h

menuconfig: $(BUILD_SYSTEM)/kconfiglib/menuconfig.py prepare-config
	@$(COMMON_CONFIG_ENV) TUYA_DEFCONFIG=$(DEFCONFIG) \
		$< IoTOSconfig

savedefconfig: $(BUILD_SYSTEM)/kconfiglib/savedefconfig.py prepare-config
	@grep "#include" $(DEFCONFIG) > $(DEFCONFIG).split || true
	@$(COMMON_CONFIG_ENV) TUYA_DEFCONFIG=$(DEFCONFIG) \
		$< --out=$(DEFCONFIG) --kconfig IoTOSconfig
	@sed -i -e '/TUYA_DEFCONFIG=/d' $(DEFCONFIG)
	@cat $(DEFCONFIG) >> $(DEFCONFIG).split
	@$(BUILD_SYSTEM)/scripts/defconfig_hook.py -s -r $(SDK_DIR) $(DEFCONFIG).split $(DEFCONFIG)
	@rm -rf $(DEFCONFIG).split

saveconfig: $(BUILD_SYSTEM)/kconfiglib/savedefconfig.py prepare-config
	@cp -v $(TUYA_CONFIG) $(DEFCONFIG)
	@sed -i -e '/TUYA_DEFCONFIG=/d' $(DEFCONFIG)

defconfig: $(BUILD_SYSTEM)/kconfiglib/olddefconfig.py $(BUILD_SYSTEM)/tuya_iot.config prepare-config
	@$(BUILD_SYSTEM)/scripts/defconfig_hook.py -m -r $(SDK_DIR) $(BUILD_SYSTEM)/tuya_iot.config $(TUYA_CONFIG)
	@$(COMMON_CONFIG_ENV) TUYA_DEFCONFIG=$(BUILD_SYSTEM)/tuya_iot.config \
		$< IoTOSconfig

allyesconfig: $(BUILD_SYSTEM)/kconfiglib/allyesconfig.py prepare-config
	@$(COMMON_CONFIG_ENV) TUYA_DEFCONFIG=$(DEFCONFIG) \
		$< IoTOSconfig

%.config: $(BUILD_SYSTEM)/kconfiglib/olddefconfig.py $(BUILD_SYSTEM)/config/%.config prepare-config
	@$(BUILD_SYSTEM)/scripts/defconfig_hook.py -m -r $(SDK_DIR) $(BUILD_SYSTEM)/config/$@ $(TUYA_CONFIG)
	@$(COMMON_CONFIG_ENV) TUYA_DEFCONFIG=$(BUILD_SYSTEM)/config/$@ \
		$< IoTOSconfig
