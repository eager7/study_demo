REVISION_OPENWRT = 20141127-30965ec3
REVISION_OPENWRT_FEEDS = 20141127-30965ec3
REVISION_LUCI = mtk-20141127-30965ec3

OPENWRT_DIR = Tonly_OpenWRT
INSTALL_PACKAGES = rsync luci avahi-dnsconfd mtd-utils mrd6 freeradius2 openssh-sftp-server \
tcpdump python pyserial libiwinfo picocom gw6c atftp libftdi libsqlite3 libavahi-client radvd \
libxml2

PATCHES_COMMON_DIR = ../nxp_backfire/backfire-patches/

DOWNLOAD_DIR = ../downloads/

.PHONY: 

define install_package
	cd $(OPENWRT_DIR) && ./scripts/feeds install $(1)
endef

default:bulid

install_depends:
	sudo apt-get install gcc g++ git binutils patch bzip2 flex bison make autoconf gettext texinfo unzip sharutils subversion libncurses5-dev ncurses-term zlib1g-dev gawk libssl-dev

install_packages:
	cd  $(OPENWRT_DIR);./scripts/feeds install $(INSTALL_PACKAGES)
	touch install_packages		

copy_feeds:
	rsync -av --exclude .svn --exclude "*~" tonly_backfire/ $(OPENWRT_DIR)/ 
	touch copy_feeds
	
copy_config:
	cp config/config $(OPENWRT_DIR)/.config

patch_target:
	cd $(OPENWRT_DIR) && ln -s $(DOWNLOAD_DIR) dl
	touch patch_target
	
build_project:
	cd $(OPENWRT_DIR) && make defconfig && make V=s
	
all:  copy_feeds install_packages  copy_config  patch_target build_project

distclean:
	$(RM) install_packages patch_target copy_feeds
	cp $(OPENWRT_DIR)/feeds tonly_backfire -rf
	cd $(OPENWRT_DIR) ; make distclean
	
clean:
	cd $(OPENWRT_DIR); make clean

menuconfig:
	cd $(OPENWRT_DIR); make menuconfig
	
bulid:
	cd $(OPENWRT_DIR); make V=s
