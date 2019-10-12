PROJECT		= sifs

HEADER		= $(PROJECT).h
LIBRARY		= lib$(PROJECT).a

APPLICATIONS	= sifs_mkvolume sifs_dirinfo playground sifs_mkdir sifs_dirinfo sifs_rmdir sifs_writefile sifs_readfile sifs_fileinfo sifs_rmfile

# ----------------------------------------------------------------

CC      = cc
CFLAGS  = -std=c99 -Wall -Werror -pedantic
LIBS	= -L. -lsifs -lm


all:	$(APPLICATIONS)

$(LIBRARY):
	make -C library

%:	%.c $(LIBRARY)
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)


clean:
	rm -f $(LIBRARY) $(APPLICATIONS)
	make -C library clean

remake:
	rm -f $(LIBRARY) $(APPLICATIONS)
	make -C library clean
	make

