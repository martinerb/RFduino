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

eclipse1:
	make -f makefile.md clean
	make -f makefile.md all
	make -f makefile.md afiles
	make -f makefile.md main
	make -f makefile.md link


eclipse2:
	make -f makefile.md cleansource
	make -f makefile.md all
	make -f makefile.md afiles
	make -f makefile.md main
	make -f makefile.md link
	
ble:
	make -f makefile.md cleansource
	make -f makefile.md all
	make -f makefile.md afiles
	make -f makefile.md main
	make -f makefile.md link
	make -f makefile.md load1
	
blevirt:
	make -f makefile.md cleansource
	make -f makefile.md all
	make -f makefile.md afiles
	make -f makefile.md main
	make -f makefile.md link
	make -f makefile.md load0


without-core:
	make -f makefile.md main
	
	
	