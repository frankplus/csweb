all:
	cd JGE && $(MAKE) all
	cd cspsp && $(MAKE) all

clean:
	cd JGE && $(MAKE) clean
	cd cspsp && $(MAKE) clean

debug:
	cd JGE && $(MAKE) debug
	cd cspsp && $(MAKE) debug
	
release:
	cd JGE && $(MAKE) release
	cd cspsp && $(MAKE) release

server:
	cd cspsp/bin/apps && python3 -m http.server 8080
proxy:
	cd cspsp/bin/apps && node ./proxy.js

.PHONY: all clean debug release server