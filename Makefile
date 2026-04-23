.PHONY: all configure build run test clean rebuild

BUILD_DIR=build
TARGET=April
PREFIX_PATH=$(HOME)/hdd/storage/Logger/Logger/install

all: build

configure:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_PREFIX_PATH=$(PREFIX_PATH)

build: configure
	cmake --build $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/$(TARGET)

test: build
	cd $(BUILD_DIR) && ctest --output-on-failure

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all