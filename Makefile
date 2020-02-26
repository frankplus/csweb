all:
	cd JGE && $(MAKE) all
	cd cswasm && $(MAKE) all

clean:
	cd JGE && $(MAKE) clean
	cd cswasm && $(MAKE) clean

debug:
	cd JGE && $(MAKE) debug
	cd cswasm && $(MAKE) debug
	
release:
	cd JGE && $(MAKE) release
	cd cswasm && $(MAKE) release

server:
	cd cswasm/bin/apps && python3 -m http.server 8080

.PHONY: all clean debug release server