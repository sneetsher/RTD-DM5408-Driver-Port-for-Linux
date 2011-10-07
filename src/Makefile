SRCDIR := src
SUBDIRS := mod lib bin


.PHONY: all $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

bin: lib

doc:
	$(MAKE) -C $@

clean:
	$(foreach dir,$(SUBDIRS),$(MAKE) -C $(dir) clean;)

install:
	$(foreach dir,$(SUBDIRS),$(MAKE) -C $(dir) install;)

uninstall:
	$(foreach dir,$(SUBDIRS),$(MAKE) -C $(dir) uninstall;)
