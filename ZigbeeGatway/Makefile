include ./config/envrienment.config

GATWAY_DIR := $(shell pwd)
DEPENDS_APP_DIR := $(shell pwd)/DependsPackages
ROOMFS_DIR := $(ROMFS_DIR)

SQLITE_DIR = sqlite-autoconf-3080801
ZLIB_DIR = zlib-1.2.8
LIBXML2_DIR = libxml2-2.9.2
LIBLZMA_DIR = lzma-4.65
RADVD_DIR = radvd-1.9.1
LIBDAEMON_DIR = libdaemon-0.14
MRD6_DIR = mrd6-2013-11-30
LIBTECLA_DIR = libtecla
NCURSES_DIR = ncurses-5.9
APPWEB_DIR = appweb-3.4.2
JSON_DIR = json-c-0.11
OPENSSH_DIR = openssh-5.9p1
OPENSSL_DIR = openssl-1.0.1j
NANOMSG_DIR = nanomsg-0.5-beta

SRC_APP_DIR := $(shell pwd)/src
LibJIP_DIR = libJIP
ZCB_DIR = zigbee-jip-daemon/ZCB/Build
JIP_DIR = zigbee-jip-daemon/JIP

CP = cp -a
MAKE = make -C
.PHONY:lib app clean romfs all distclean modules_install 

default:all

all: depends_app 6LOWPAN ZIGBEE  PCT_APP roomfs

depends_app:
	#sqlite3
	@if [ -d $(DEPENDS_APP_DIR)/$(SQLITE_DIR) ]; then\
		echo "Compile sqlite3 package";\
		if [ "$(CONFIG_SQLITE)" = "y" ]; then\
			cd $(DEPENDS_APP_DIR)/$(SQLITE_DIR);\
			if [ -f ./Makefile ];then\
				make distclean;\
			fi;\
			./configure --host=$(RUN_PLATFORM) --target=mipsel-linux --build=x86_64-linux-gnu --prefix=$(GATWAY_DIR) --exec-prefix=$(GATWAY_DIR) --enable-shared --enable-static ;\
			cd -;\
			$(MAKE) $(DEPENDS_APP_DIR)/$(SQLITE_DIR) &&$(MAKE) $(DEPENDS_APP_DIR)/$(SQLITE_DIR) install || exit "$$?";\
		fi;\
	fi	
	
	#nanomsg-0.5-beta
	@if [ -d $(DEPENDS_APP_DIR)/$(NANOMSG_DIR) ]; then\
		echo "Compile nanomsg-0.5-beta package";\
		if [ "$(CONFIG_NANOMSG)" = "y" ]; then\
			cd $(DEPENDS_APP_DIR)/$(NANOMSG_DIR);\
			if [ -f ./Makefile ];then\
				make distclean;\
			fi;\
			./configure --host=$(RUN_PLATFORM) --target=mipsel-linux --build=x86_64-linux-gnu --prefix=$(GATWAY_DIR) --exec-prefix=$(GATWAY_DIR) --enable-shared --enable-static ;\
			cd -;\
			$(MAKE) $(DEPENDS_APP_DIR)/$(NANOMSG_DIR) &&$(MAKE) $(DEPENDS_APP_DIR)/$(NANOMSG_DIR) install || exit "$$?";\
		fi;\
	fi	
		
	#zlib
	if [ -d $(DEPENDS_APP_DIR)/$(ZLIB_DIR) ]; then\
		echo "Compile zlib package";\
		if [ "$(CONFIG_ZLIB)" = "y" ]; then\
			if [ -f ./libz.so.1.2.8 ];then\
				make distclean;\
			fi;\
			cd $(DEPENDS_APP_DIR)/$(ZLIB_DIR);\
			export CFLAGS="-Os -pipe -mno-branch-likely -mips32r2  -fPIC -D_LARGEFILE64_SOURCE=1 -DHAVE_HIDDEN";\
			./configure --uname=$(RUN_PLATFORM) --prefix=$(GATWAY_DIR) --eprefix=$(GATWAY_DIR);  \
			cd -;\
			$(MAKE) $(DEPENDS_APP_DIR)/$(ZLIB_DIR) && $(MAKE) $(DEPENDS_APP_DIR)/$(ZLIB_DIR) install || exit "$$?";\
		fi;\
	fi	
		
	#libxml2
	if [ -d $(DEPENDS_APP_DIR)/$(LIBXML2_DIR) ]; then\
		echo "Compile Liblzma package";\
		if [ "$(CONFIG_LIBXML2)" = "y" ]; then\
			cd $(DEPENDS_APP_DIR)/$(LIBXML2_DIR);\
			if [ -f ./Makefile ];then\
				make distclean;\
			fi;\
			./configure --host=$(RUN_PLATFORM) --target=mipsel-linux --build=x86_64-linux-gnu --prefix=$(GATWAY_DIR) --exec-prefix=$(GATWAY_DIR)\
						--enable-shared --enable-static --with-c14n --without-catalog --with-debug --without-docbook \
						--with-html --without-ftp --without-http --without-iconv --without-iso8859x --without-legacy \
						--with-output --without-pattern --without-push --without-python --with-reader --without-readline \
						--without-regexps --with-sax1 --with-schemas --with-threads --with-tree --with-valid --with-writer \
						--with-xinclude --with-xpath --with-xptr --with-zlib --without-lzma LIBS="-lz";\
			cd -;\
			$(MAKE) $(DEPENDS_APP_DIR)/$(LIBXML2_DIR) && $(MAKE) $(DEPENDS_APP_DIR)/$(LIBXML2_DIR) install || exit "$$?";\
		fi;\
	fi		
		
	#libdaemon
	if [ -d $(DEPENDS_APP_DIR)/$(LIBDAEMON_DIR) ]; then\
		echo "Compile libdaemon package";\
		if [ "$(CONFIG_LIBDAEMON)" = "y" ]; then\
			cd $(DEPENDS_APP_DIR)/$(LIBDAEMON_DIR);\
			if [ -f ./Makefile ];then\
				make distclean;\
			fi;\
			./configure --host=$(RUN_PLATFORM) --target=mipsel-linux --build=x86_64-linux-gnu --prefix=$(GATWAY_DIR) --exec-prefix=$(GATWAY_DIR)  \
						--enable-shared --enable-static --disable-lynx --disable-examples ac_cv_func_setpgrp_void=yes;\
			cd -;\
			$(MAKE) $(DEPENDS_APP_DIR)/$(LIBDAEMON_DIR) && $(MAKE) $(DEPENDS_APP_DIR)/$(LIBDAEMON_DIR) install || exit "$$?";\
		fi;\
	fi	
	
	#radvd
	if [ -d $(DEPENDS_APP_DIR)/$(RADVD_DIR) ]; then\
		echo "Compile radvd package";\
		if [ "$(CONFIG_RADVD)" = "y" ]; then\
			if [ "$(TOOLCHAIN)" = "/opt/buildroot-gcc342/bin" ]; then\
				cd $(DEPENDS_APP_DIR)/$(RADVD_DIR);\
				if [ -f ./Makefile ];then\
					make distclean;\
				fi;\
				export CPPFLAGS="-I$(shell pwd)/include -I$(shell pwd)/include/libdaemon -I$(SOURCE_DIR)/linux-2.6.36.x/include -I$(SOURCE_DIR)/lib/include";	\
				./configure --host=$(RUN_PLATFORM) 							\
							--target=mipsel-linux --build=x86_64-linux-gnu  \
							--prefix=$(GATWAY_DIR)/program342 				\
							--exec-prefix=$(GATWAY_DIR)/program342 			\
							LIBS="-ldaemon"									\
							--with-configfile=/etc/radvd.conf 				\
							--with-logfile=/var/log/radvd.log 				\
							--with-pidfile=/var/run/radvd.pid ;				\
				cd -;\
				$(MAKE) $(DEPENDS_APP_DIR)/$(RADVD_DIR) && $(MAKE) $(DEPENDS_APP_DIR)/$(RADVD_DIR) install || exit "$$?";	\
			else \
				cd $(DEPENDS_APP_DIR)/$(RADVD_DIR);\
				if [ -f ./Makefile ];then\
					make distclean;\
				fi;\
				export CPPFLAGS="-I$(shell pwd)/include -I$(shell pwd)/include/libdaemon -I$(SOURCE_DIR)/linux-2.6.36.x/include -I$(SOURCE_DIR)/lib/include";	\
				./configure --host=$(RUN_PLATFORM) 							\
							--target=mipsel-linux --build=x86_64-linux-gnu  \
							--prefix=$(GATWAY_DIR) 							\
							--exec-prefix=$(GATWAY_DIR) 					\
							LIBS="-ldaemon"									\
							--with-configfile=/etc/radvd.conf 				\
							--with-logfile=/var/log/radvd.log 				\
							--with-pidfile=/var/run/radvd.pid ;				\
				cd -;\
				$(MAKE) $(DEPENDS_APP_DIR)/$(RADVD_DIR) && $(MAKE) $(DEPENDS_APP_DIR)/$(RADVD_DIR) install || exit "$$?";\
			fi;\
		fi;\
	fi
	
	#ncurses-5.9
	if [ -d $(DEPENDS_APP_DIR)/$(NCURSES_DIR) ]; then\
		echo "Compile ncurses-5.9 package";\
		if [ "$(CONFIG_NCURSES)" = "y" ]; then\
			cd $(DEPENDS_APP_DIR)/$(NCURSES_DIR);\
			if [ -f ./Makefile ];then\
				make distclean;\
			fi;\
			./configure --host=$(RUN_PLATFORM) --target=mipsel-linux --build=x86_64-linux-gnu --prefix=$(GATWAY_DIR) --exec-prefix=$(GATWAY_DIR) \
			--enable-echo --enable-const --enable-overwrite --disable-rpath --without-ada --without-debug --without-tests \
			--without-profile --without-progs --disable-big-core --disable-home-terminfo --with-normal --with-shared; \
			cd -;\
			$(MAKE) $(DEPENDS_APP_DIR)/$(NCURSES_DIR) && $(MAKE) $(DEPENDS_APP_DIR)/$(NCURSES_DIR) install || exit "$$?";\
		fi;\
	fi	

	#libtecla
	if [ -d $(DEPENDS_APP_DIR)/$(LIBTECLA_DIR) ]; then\
		echo "Compile libtecla package";\
		if [ "$(CONFIG_LIBTECLA)" = "y" ]; then\
			cd $(DEPENDS_APP_DIR)/$(LIBTECLA_DIR);\
			make clean;\
			#./configure --host=$(RUN_PLATFORM) --prefix=$(GATWAY_DIR) --exec-prefix=$(GATWAY_DIR);  \
			cd -;\
			$(MAKE) $(DEPENDS_APP_DIR)/$(LIBTECLA_DIR) normal install_lib install_inc || exit "$$?";\
		fi;\
	fi	
	
	#appweb
	if [ -d $(DEPENDS_APP_DIR)/$(APPWEB_DIR) ]; then\
		echo "Compile libtecla package";\
		if [ "$(CONFIG_APPWEB)" = "y" ]; then\
			cd $(DEPENDS_APP_DIR)/$(APPWEB_DIR);\
			if [ -f ./Makefile ];then\
				make clean;\
			fi;\
			./configure --host=$(RUN_PLATFORM) --target=mipsel-linux --build=x86_64-linux-gnu --prefix=$(GATWAY_DIR) --exec-prefix=$(GATWAY_DIR);  \
			cd -;\
			#$(MAKE) $(DEPENDS_APP_DIR)/$(APPWEB_DIR) normal install_lib install_inc || exit "$$?";\
		fi;\
	fi	
	
	
	#mrd6
	if [ -d $(DEPENDS_APP_DIR)/$(MRD6_DIR) ]; then\
		echo "Compile mrd6 package";\
		if [ "$(CONFIG_MRD6)" = "y" ]; then\
			export CPPFLAGS="-I/opt/buildroot-gcc463_gc/usr/mipsel-buildroot-linux-uclibc/sysroot/usr/include"; \
			echo $(CPPFLAGS);\
			$(MAKE) $(DEPENDS_APP_DIR)/$(MRD6_DIR) clean -w ; \
			$(MAKE) $(DEPENDS_APP_DIR)/$(MRD6_DIR)/src PREFIX=$(GATWAY_DIR) \
			CFLAGS="-DNO_INET6_OPTION -fno-builtin -fno-rtti" \
			&& $(MAKE) $(DEPENDS_APP_DIR)/$(MRD6_DIR) install || exit "$$?";\
		fi;\
	fi	
		
	#json-c-0.11
	if [ -d $(DEPENDS_APP_DIR)/$(JSON_DIR) ]; then\
		echo "Compile mrd6 package";\
		if [ "$(CONFIG_JSON)" = "y" ]; then\
			cd $(DEPENDS_APP_DIR)/$(JSON_DIR);\
			if [ -f ./Makefile ];then\
				make clean;\
			fi;\
			./configure --host=$(RUN_PLATFORM) --target=mipsel-linux --build=x86_64-linux-gnu --with-gnu-ld --prefix=$(GATWAY_DIR) --exec-prefix=$(GATWAY_DIR); \
			cd -;\
			$(MAKE) $(DEPENDS_APP_DIR)/$(JSON_DIR) && $(MAKE) $(DEPENDS_APP_DIR)/$(JSON_DIR) install || exit "$$?";\
		fi;\
	fi	
	
	
6LOWPAN:	
	if [ -d $(SRC_APP_DIR)/$(LibJIP_DIR) ]; then\
		if [ "$(CONFIG_6LoWPAN_libJIP)" = "y" ]; then\
			$(MAKE) $(SRC_APP_DIR)/$(LibJIP_DIR)/Build clean;\
			$(MAKE) $(SRC_APP_DIR)/$(LibJIP_DIR)/Build all || exit "$$?";\
			$(CP)   $(SRC_APP_DIR)/$(LibJIP_DIR)/Include/*.h $(GATWAY_DIR)/include;\
			$(CP)   $(SRC_APP_DIR)/$(LibJIP_DIR)/Library/* $(GATWAY_DIR)/lib;\
		fi;\
	fi
	
ZIGBEE:	
	if [ -d $(SRC_APP_DIR)/$(LibJIP_DIR) ]; then\
		if [ "$(CONFIG_zigbee_control_bridge)" = "y" ]; then\
			#$(MAKE) $(SRC_APP_DIR)/$(JIP_DIR)/Build clean;\
			#$(SRC_APP_DIR)/$(JIP_DIR)/Build/build.sh ;\
			cp -a $(SRC_APP_DIR)/$(JIP_DIR)/Build/zigbee-jip-daemon $(GATWAY_DIR)/sbin/ ;\
			#$(MAKE) $(SRC_APP_DIR)/$(JIP_DIR) clean;\
			#$(MAKE) $(SRC_APP_DIR)/$(JIP_DIR) all || exit "$$?";\
		fi;\
	fi
	
PCT_APP:	
	#openssl-1.0.1j
	if [ -d $(DEPENDS_APP_DIR)/$(OPENSSL_DIR) ]; then\
		echo "Compile mrd6 package";\
		if [ "$(CONFIG_OPENSSL)" = "y" ]; then\
			cd $(DEPENDS_APP_DIR)/$(OPENSSL_DIR);\
			if [ -f ./Makefile ];then\
				make clean;\
			fi;\
			./Configure --host=$(RUN_PLATFORM) --target=mipsel-linux --build=x86_64-linux-gnu \
			--prefix=$(GATWAY_DIR); \
			cd -;\
			$(MAKE) $(DEPENDS_APP_DIR)/$(OPENSSL_DIR) && $(MAKE) $(DEPENDS_APP_DIR)/$(OPENSSL_DIR) install || exit "$$?";\
		fi;\
	fi	
	
	#openssh-5.9p1
	if [ -d $(DEPENDS_APP_DIR)/$(OPENSSH_DIR) ]; then\
		echo "Compile mrd6 package";\
		if [ "$(CONFIG_OPENSSH)" = "y" ]; then\
			cd $(DEPENDS_APP_DIR)/$(OPENSSH_DIR);\
			if [ -f ./Makefile ];then\
				make clean;\
			fi;\
			./configure --host=$(RUN_PLATFORM) --target=mipsel-linux --build=x86_64-linux-gnu \
			--prefix=$(GATWAY_DIR)/program342 --exec-prefix=$(GATWAY_DIR)/program342; \
			cd -;\
			$(MAKE) $(DEPENDS_APP_DIR)/$(OPENSSH_DIR) && $(MAKE) $(DEPENDS_APP_DIR)/$(OPENSSH_DIR) install || exit "$$?";\
		fi;\
	fi	
	

roomfs:
	mkdir -p $(ROOMFS_DIR)/etc_ro/init.d
	$(CP) $(GATWAY_DIR)/files/radvd.config $(ROOMFS_DIR)/etc_ro/radvd.conf
	$(CP) $(GATWAY_DIR)/files/radvd.init $(ROOMFS_DIR)/etc_ro/init.d/radvd
	$(CP) $(GATWAY_DIR)/program342/sbin/mipsel-linux-radvd $(ROOMFS_DIR)/sbin/radvd
	
	$(CP) $(GATWAY_DIR)/sbin/mrd $(ROOMFS_DIR)/sbin/mrd6
	$(CP) $(GATWAY_DIR)/bin/mrd6sh $(ROOMFS_DIR)/bin/mrd6sh
	$(CP) $(GATWAY_DIR)/files/mrd6.conf $(ROOMFS_DIR)/etc_ro/mrd6.conf
	$(CP) $(GATWAY_DIR)/lib/mrd6/bgp.so $(ROOMFS_DIR)/lib/bgp.so
	
	$(CP) $(GATWAY_DIR)/sbin/zigbee-jip-daemon $(ROOMFS_DIR)/sbin/zigbee-jip-daemon
	$(CP) $(GATWAY_DIR)/files/zigbee-jip-daemon $(ROOMFS_DIR)/etc_ro/zigbee-jip-daemon
	$(CP) $(GATWAY_DIR)/files/SQLITE.DB $(ROOMFS_DIR)/mnt/SQLITE.DB
	$(CP) $(GATWAY_DIR)/files/jip_cache_definitions.xml $(ROOMFS_DIR)/etc_ro/jip_cache_definitions.xml
clean:
	# rm -rf $(GATWAY_DIR)/lib/*
	# rm -rf $(GATWAY_DIR)/include/*
	# rm -rf $(GATWAY_DIR)/bin/*
	# rm -rf $(GATWAY_DIR)/sbin/*
	# rm -rf $(GATWAY_DIR)/share/*
	# $(MAKE) $(DEPENDS_APP_DIR)/$(LIBDAEMON_DIR) clean;
	# $(MAKE) $(DEPENDS_APP_DIR)/$(MRD6_DIR)/src clean -w;
	# $(MAKE) $(DEPENDS_APP_DIR)/$(ZLIB_DIR) clean;
	# $(MAKE) $(DEPENDS_APP_DIR)/$(SQLITE_DIR) clean;
	# $(MAKE) $(DEPENDS_APP_DIR)/$(RADVD_DIR) clean;
	# $(MAKE) $(DEPENDS_APP_DIR)/$(LIBXML2_DIR) clean;
	# $(MAKE) $(SRC_APP_DIR)/$(LibJIP_DIR)/Build clean;
	# $(MAKE) $(SRC_APP_DIR)/$(JSON_DIR) clean;
	# $(MAKE) $(SRC_APP_DIR)/$(LIBTECLA_DIR) clean;
	# $(MAKE) $(SRC_APP_DIR)/$(NCURSES_DIR) clean;
	
distclean:
	# rm -rf $(GATWAY_DIR)/lib/*
	# rm -rf $(GATWAY_DIR)/include/*
	# rm -rf $(GATWAY_DIR)/bin/*
	# rm -rf $(GATWAY_DIR)/sbin/*
	# rm -rf $(GATWAY_DIR)/share/*
	# rm -rf $(GATWAY_DIR)/man/*
	# cd $(DEPENDS_APP_DIR)/$(LIBDAEMON_DIR);make distclean;
	# cd $(DEPENDS_APP_DIR)/$(MRD6_DIR)/src;make clean -w;
	# cd $(DEPENDS_APP_DIR)/$(ZLIB_DIR);make distclean;
	# cd $(DEPENDS_APP_DIR)/$(SQLITE_DIR);make distclean;
	# cd $(DEPENDS_APP_DIR)/$(RADVD_DIR);make distclean;
	# cd $(DEPENDS_APP_DIR)/$(LIBXML2_DIR);make distclean;
	# cd $(SRC_APP_DIR)/$(LibJIP_DIR)/Build;make clean;	
	# cd $(SRC_APP_DIR)/$(JSON_DIR);make distclean;	
	# cd $(SRC_APP_DIR)/$(LIBTECLA_DIR);make distclean;	
	# cd $(SRC_APP_DIR)/$(NCURSES_DIR);make distclean;	
	