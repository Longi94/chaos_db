.PHONY: all clean debug release

all: release

clean:
	rm -rf build

debug:
	mkdir -p build/debug && \
	cd build/debug && \
	cmake -DCMAKE_BUILD_TYPE=Debug ../.. && \
	cmake --build . --target injector

release:
	mkdir -p build/release && \
	cd build/release && \
	cmake -DCMAKE_BUILD_TYPE=Release ../.. && \
	cmake --build . --target injector
