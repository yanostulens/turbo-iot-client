################################################################################
#
# nit-sensors
#
################################################################################

NIT_SENSORS_VERSION = v1.2
NIT_SENSORS_SITE = git@gitlab.com:nalys-nit/traineeship/2022-ist-fall/nit-sensors.git
NIT_SENSORS_SITE_METHOD = git
NIT_SENSORS_DEPENDENCIES = libgpiod

define NIT_SENSORS_BUILD_CMDS
    $(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) all
endef

define NIT_SENSORS_INSTALL_TARGET_CMDS
    $(MAKE) -C $(@D) DESTDIR=$(TARGET_DIR)/usr/bin install
endef

NIT_SENSORS_MODULE_SUBDIRS = module

$(eval $(kernel-module))
$(eval $(generic-package))
