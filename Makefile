all: build

clean:
	rebar clean ; \
	cd priv/judy && (make clean || make distclean) > /dev/null; true

build: judy nif

judy:
	cd priv/judy && \
		./configure --quiet && \
		make --quiet && make install

nif:
	rebar compile
