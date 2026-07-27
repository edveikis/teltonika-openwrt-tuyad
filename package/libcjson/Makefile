include $(TOPDIR)/rules.mk

PKG_NAME:=libcjson
PKG_RELEASE:=1
PKG_VERSION:=1.0.0
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/DaveGamble/cJSON.git
PKG_SOURCE_VERSION:=fb16e5cf358798aabb049655975cde8427101056

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libcjson
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=cJSON
	DEPENDS:=+libc
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/cjson $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcjson.so* $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libcjson.pc $(1)/usr/lib/pkgconfig/ 2>/dev/null || true
endef

define Package/libcjson/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcjson.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libcjson))
