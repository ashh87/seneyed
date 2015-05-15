# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall -O2 -I$(top_srcdir)/include
AM_LDFLAGS = -pthread

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = seneyed
seneyed_SOURCES = main.c seneye_daemon.c seneye_hotplug.c
