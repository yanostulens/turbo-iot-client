################################################################################
#
# cpp-sensors
#
################################################################################

CPP_SENSORS_VERSION = v1.0
CPP_SENSORS_SITE = /work/cpp-sensors
CPP_SENSORS_SITE_METHOD = local
CPP_SENSORS_DEPENDENCIES = libgpiod

define CPP_SENSORS_BUILD_CMDS
    $(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) build
endef

define CPP_SENSORS_INSTALL_TARGET_CMDS
    $(MAKE) -C $(@D) DESTDIR=$(TARGET_DIR)/usr/bin install
endef

CPP_SENSORS_MODULE_SUBDIRS = module

$(eval $(kernel-module))
$(eval $(generic-package))
