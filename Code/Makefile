all:
	make -f makefile.md all
	make -f makefile.md afiles
	make -f makefile.md main
	make -f makefile.md link
	
clean:
	make -f makefile.md clean

load:
	make -f makefile.md load
	picocom /dev/ttyUSB1

eclipse:
	make -f makefile.md cleansource
	make -f makefile.md all
	make -f makefile.md afiles
	make -f makefile.md main
	make -f makefile.md link
	make -f makefile.md load

without-core:
	make -f makefile.md main