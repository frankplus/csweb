all:
	cd JGE && $(MAKE) all
	cd cspsp && $(MAKE) all

clean:
	cd JGE && $(MAKE) clean
	cd cspsp && $(MAKE) clean

server:
	cd cspsp/bin/apps && python3 -m http.server 8080