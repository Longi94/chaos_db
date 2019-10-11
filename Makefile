.PHONY: all clean debug release

all: release

clean:
	rm -rf build

debug:
	mkdir -p build/debug && \
	cd build/debug && \
	cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Debug ../.. && \
	cmake --build . -- --no-print-directory

release:
	mkdir -p build/release && \
	cd build/release && \
	cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release ../.. && \
	cmake --build . -- --no-print-directory
