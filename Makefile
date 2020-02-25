all:
	cd JGE && $(MAKE) all
	cd cswasm && $(MAKE) all

clean:
	cd JGE && $(MAKE) clean
	cd cswasm && $(MAKE) clean

server:
	cd cswasm/bin/apps && python3 -m http.server 8080