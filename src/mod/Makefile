obj-m = dm5408.o

MODULE_NAME ?= dm5408
MODULE_VERSION ?= 0.1

kernel_version ?= $(shell uname -r)
PWD ?= $(shell pwd)

all: prepare
	@make -C /lib/modules/$(kernel_version)/build M=$(PWD) modules

clean:
	@rm -f dm5408_ioctl.h
	make -C /lib/modules/$(kernel_version)/build M=$(PWD) clean

prepare:
	@cp -u ../include/dm5408_ioctl.h ./ \
	|| @cp -u /usr/local/include/dm5408/dm5408_ioctl.h ./ \
	|| @cp -u /usr/include/dm5408/dm5408_ioctl.h ./

install: install-src install-dkms install-initd

install-src: clean prepare
	@mkdir -p /usr/src/$(MODULE_NAME)-$(MODULE_VERSION)
	@cp ./* /usr/src/$(MODULE_NAME)-$(MODULE_VERSION)

install-dkms:
	dkms add -m $(MODULE_NAME) -v $(MODULE_VERSION)
	dkms build -m $(MODULE_NAME) -v $(MODULE_VERSION)
	dkms install -m $(MODULE_NAME) -v $(MODULE_VERSION)

install-initd:
	@cp $(MODULE_NAME).sh /etc/init.d/$(MODULE_NAME)
	@chmod 755 /etc/init.d/$(MODULE_NAME)
	update-rc.d $(MODULE_NAME) defaults
	/etc/init.d/$(MODULE_NAME) restart

uninstall: uninstall-src uninstall-dkms uninstall-initd

uninstall-src:
	@rm -f -r /usr/src/$(MODULE_NAME)-*

uninstall-dkms:
	dkms uninstall -m $(MODULE_NAME) -v $(MODULE_VERSION)
	dkms remove -m $(MODULE_NAME) -v $(MODULE_VERSION) --all

uninstall-initd:
	update-rc.d -f $(MODULE_NAME) remove
	@rm -f -r /etc/init.d/$(MODULE_NAME)
