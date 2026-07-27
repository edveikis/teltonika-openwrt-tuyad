include $(TOPDIR)/rules.mk

PKG_NAME:=libtuyacore
PKG_RELEASE:=1
PKG_VERSION:=1.0.0
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/tuya/tuya-iot-core-sdk.git
PKG_SOURCE_VERSION:=0208badefb2003d30e70b5b875395e3187c850d9

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_BINARY_SUBDIR:=build

define Package/libtuyacore
	CATEGORY:=tuyad
	TITLE:=libtuyacore
	DEPENDS:=+libc
endef

define Build/Install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/include/. $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/interface/. $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/utils/*.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/include/mbedtls
	$(CP) $(PKG_BUILD_DIR)/libraries/mbedtls/include/mbedtls/. $(1)/usr/include/mbedtls/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/build/lib/*.so $(1)/usr/lib/
endef

define Package/libtuyacore/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/build/lib/*.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libtuyacore))