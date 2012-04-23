all: build

clean:
	rebar clean ; \
	cd priv/judy && (make clean || make distclean)

build: judy nif

judy:
	cd priv/judy && \
		./configure --quiet && \
		make --quiet && make install 1>/dev/null

nif:
	rebar compile
