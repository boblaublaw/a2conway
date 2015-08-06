#
# Makefile for cc65 samples
#
# This Makefile requires GNU make
#

# Enter the target system here
SYS	= apple2enh

CFLAGS = -Ori --codesize 500
IMGNAME = A2CONWAY.DSK

# test program starts at $6000 to not overwrite graphics pages
LDFLAGS_a2conway_apple2enh=--start-addr 24576

# Determine the path to the executables and libraries. If the samples
# directory is part of a complete source tree, use the stuff from that
# source tree; otherwise, use the "install" directories.
CLIB = --lib $(SYS).lib
CL   = cl65
CC   = cc65
AS   = ca65
LD   = ld65


# --------------------------------------------------------------------------
# System-dependent settings

# --------------------------------------------------------------------------
# Generic rules

%: %.c
%: %.s

.c.o:
	@echo $<
	$(CC) $(CFLAGS) -T -g -t $(SYS) $<
	$(AS) $(<:.c=.s)

.s.o:
	@echo $<
	$(AS) $(AFLAGS) -t $(SYS) $<

.PRECIOUS: %.o

.o:
	$(LD) $(LDFLAGS_$(@F)_$(SYS)) -o $@ -t $(SYS) -m $@.map $^ $(CLIB)

# --------------------------------------------------------------------------
# List of executables. This list could be made target-dependent by checking
# $(SYS).

EXELIST	=	a2conway 

a2conway: a2conway.o glider.o simkins.o gospergun.o naive.o opt1.o


# --------------------------------------------------------------------------
# Rules to make the binaries

.PHONY:	all
all:	$(EXELIST)


# --------------------------------------------------------------------------
# Overlay rules. Overlays need special ld65 configuration files.  Also, the
# overlay file-names are shortenned to fit the Atari's 8.3-character limit.


# --------------------------------------------------------------------------
# Clean-up rules

.PHONY:	clean dsk
clean:
	$(RM) *~ *.map *.o *.s *.lbl ${EXELIST}

dsk:	${EXELIST}
	for x in $^; do AppleCommander.sh -d ${IMGNAME} $$x; done
	for x in $^; do sh -c "AppleCommander.sh -cc65 ${IMGNAME} $$x BIN < $$x"; done
	AppleCommander.sh -l ${IMGNAME} 
