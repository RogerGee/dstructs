################################################################################
# Makefile for dstructs library ################################################
################################################################################
.PHONY: package install uninstall debug clean

# compiler options
ifeq ($(MAKECMDGOALS),debug)
COMPILE = gcc -Wall -g -c -o
else
COMPILE = gcc -Wall -c -o
endif
ARCHIVE = ar cr

# header files
DSTRUCTS_H = dstructs.h
TREEMAP_H = treemap.h $(DSTRUCTS_H)
DYNARRAY_H = dynarray.h $(DSTRUCTS_H)
LIST_H = list.h
STACK_H = stack.h $(DYNARRAY_H)
QUEUE_H = stack.h $(DYNARRAY_H)
HASHMAP_H = hashmap.h $(DSTRUCTS_H)

# output files
LIBRARY = libdstructs.a
OBJECTS = treemap.o dynarray.o list.o queue.o stack.o hashmap.o
ifeq ($(TEMPDIR),)
# default to /tmp if TEMPDIR environment variable doesn't exist; I
# use TEMPDIR for my own purposes (TMP and TMPDIR are standards)
TEMPDIR = /tmp
endif
ifeq ($(MAKECMDGOALS),debug)
OBJDIR = dobj/
else
OBJDIR = obj/
endif
OBJECTS := $(addprefix $(OBJDIR),$(OBJECTS))

# rules
all: libdstructs.a

debug: libdstructs-debug.a

package:
# check to make sure the environment is correct
	@if [ ! -f $(LIBRARY) ]; then echo "Run 'make' to build project first"; exit 1; fi
	@if [ ! -d $(TEMPDIR) ]; then echo "Make sure $(TEMPDIR) exists as a directory"; exit 1; fi
	@if [ "$(ARCH)" = "" ]; then echo "Run 'make package ARCH=architecture VER=version'"; exit 1; fi
	@if [ "$(VER)" = "" ]; then echo "Run 'make package ARCH=architecture VER=version'"; exit 1; fi
# define areas within the package directory
	PACKAGEDIR = $(TEMPDIR)/dstructs-deb
	DEBDIR = $(PACKAGEDIR)/DEBIAN
	LIBDIR = $(PACKAGEDIR)/usr/lib
	INCDIR = $(PACKAGEDIR)/usr/include
	CONTROL_FILE = $(DEBDIR)/control
# make directories for package
	@mkdir -p $(PACKAGEDIR)
	@mkdir -p $(DEBDIR)
	@mkdir -p $(LIBDIR)
	@mkdir -p $(INCDIR)
# make control file
	@echo "Package: dstructs" > $(CONTROL_FILE)
	@echo "Version: $(VER)" >> $(CONTROL_FILE)
	@echo "Maintainer: Roger Gee <rpg11a@acu.edu>" >> $(CONTROL_FILE)
	@echo "Description: Development files for C-library data structures" >> $(CONTROL_FILE)
	@echo "Architecture: $(ARCH)" >> $(CONTROL_FILE)
	@echo "Depends: libc6" >> $(CONTROL_FILE)
# copy package files
	@cp -p $(LIBRARY) $(LIBDIR)
	@cp -p treemap.h hashmap.h dynarray.h queue.h stack.h list.h dstructs.h $(INCDIR)
# build package; let dpkg-deb name the package based on the control file contents
	@dpkg-deb --build $(PACKAGEDIR) .

install:
	@if [ ! -f $(LIBRARY) ]; then echo "Run 'make' to build project first"; exit 1; fi
# copy files to local installation directories
	@cp --verbose $(LIBRARY) /usr/local/lib
	@mkdir /usr/local/include/dstructs
	@cp --verbose treemap.h hashmap.h dynarray.h queue.h stack.h list.h dstructs.h /usr/local/include/dstructs

uninstall:
	@rm --verbose -f /usr/local/lib/$(LIBRARY)
	@rm --verbose -rf /usr/local/include/dstructs

clean:
# remove files in currect directory from 'all' or 'debug' operation
	@rm --verbose -rf obj/
	@rm --verbose -rf dobj/
	@rm -f --verbose $(LIBRARY)

libdstructs.a: $(OBJDIR) $(OBJECTS)
	$(ARCHIVE) libdstructs.a $(OBJECTS)
libdstructs-debug.a: $(OBJDIR) $(OBJECTS)
	$(ARCHIVE) libdstructs-debug.a $(OBJECTS)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)treemap.o: treemap.c $(TREEMAP_H) $(DYNARRAY_H)
	$(COMPILE)$(OBJDIR)treemap.o treemap.c

$(OBJDIR)dynarray.o: dynarray.c $(DYNARRAY_H)
	$(COMPILE)$(OBJDIR)dynarray.o dynarray.c

$(OBJDIR)stack.o: stack.c $(STACK_H)
	$(COMPILE)$(OBJDIR)stack.o stack.c

$(OBJDIR)queue.o: queue.c $(QUEUE_H)
	$(COMPILE)$(OBJDIR)queue.o queue.c

$(OBJDIR)list.o: list.c $(LIST_H)
	$(COMPILE)$(OBJDIR)list.o list.c

$(OBJDIR)hashmap.o: hashmap.c $(HASHMAP_H)
	$(COMPILE)$(OBJDIR)hashmap.o hashmap.c
