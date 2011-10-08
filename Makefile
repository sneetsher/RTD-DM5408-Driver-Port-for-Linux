MODULE_NAME := dm5408
MODULE_VERSION := 0.1
TMP := $(MODULE_NAME)-$(MODULE_VERSION)

doc:
	$(MAKE) -C $@

log:
	@git log --format="* %ci  %an%ncommit: %H%n%n  %s%n%n%b%n" > ChangeLog

clean:
	@rm -r -f ./$(TMP)
	@rm -f *.deb

deb: subdirs
	mkdir ./$(TMP)
	cp -r ./deb_template/* ./$(TMP)/

	make -C mod clean
	mkdir ./$(TMP)/usr/src/$(MODULE_NAME)-$(MODULE_VERSION)
	cp -r ./mod/* ./$(TMP)/usr/src/$(MODULE_NAME)-$(MODULE_VERSION)
	cp ./include/$(MODULE_NAME)_ioctl.h ./$(TMP)/usr/src/$(MODULE_NAME)-$(MODULE_VERSION)

	cp ./mod/$(MODULE_NAME).sh ./$(TMP)/etc/init.d/$(MODULE_NAME)
	cp ./lib/*.so ./$(TMP)/usr/lib/
	cp ./bin/$(MODULE_NAME)_demo ./$(TMP)/usr/bin/

	dpkg-deb -b ./$(TMP)



