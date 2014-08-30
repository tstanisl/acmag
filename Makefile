.PHONY: all clean distclean 
all:
	make -C src all

clean:
	make -C src clean

distclean: clean

