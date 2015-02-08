##
## Makefile for amd_10h_tool
##
## Copyright (C) 2014 Alexander Couzens <lynxis@fe80.eu>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

CC = gcc
CFLAGS = -O2 -Wall -W -std=c99
PROGRAM = amdtool
INSTALL = /usr/bin/install
PREFIX  = /usr/local
LDFLAGS = -lpci

all: $(PROGRAM)

$(PROGRAM): amdtool.o cmn.o index.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(PROGRAM)
	$(INSTALL) $(PROGRAM) $(PREFIX)/sbin

clean:
	rm *.o $(PROGRAM)

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -I. -o $@
