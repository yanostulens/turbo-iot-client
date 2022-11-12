TURBO_IOT_CLIENT_SITE = git@gitlab.com:nalys-nit/traineeship/2022-ist-fall/turbo-iot-client.git
TURBO_IOT_CLIENT_SITE_METHOD = git
TURBO_IOT_CLIENT_DEPENDENCIES = python3

define TURBO_IOT_CLIENT_INSTALL_TARGET_CMDS
    $(MAKE) -C $(@D) install DESTDIR=$(TARGET_DIR)/opt
endef

$(eval $(generic-package))
