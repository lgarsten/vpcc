# Copyright (c) 2014, the Mipster Project Authors.  All rights reserved.
# Please see the AUTHORS file for details.  Use of this source code is governed
# by a BSD license that can be found in the LICENSE file.

include ../cross-common.mk


LIBC_PATH=../skeleton-mipster-libc
OBJECTS += \
    scanner.o \
    parser.o \
    $(LIBC_PATH)/mipster.a
CFLAGS += -nostdlib -I$(LIBC_PATH)
LDFLAGS += -nostdlib
BINARY=a.out

all: libc_cc cross_link

clean: cross_clean
	make -C $(LIBC_PATH) clean

libc_cc:
	make -C $(LIBC_PATH) all

include ../cross-cc-mipster.mk
