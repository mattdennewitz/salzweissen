BUILD_TYPE := Debug
BUILD_DIR := build/$(shell echo $(BUILD_TYPE) | tr A-Z a-z)

PLUGIN_AU_CODE := aumu
PLUGIN_MFR_CODE := Whim

.PHONY: all release clean test validate install uninstall check-tools

all: build-plugin

check-tools:
	@xcode-select -p >/dev/null 2>&1 || { xcode-select --install; exit 1; }
	@command -v cmake >/dev/null 2>&1 || brew install cmake
	@command -v ninja >/dev/null 2>&1 || brew install ninja

lib/JUCE/CMakeLists.txt:
	git submodule update --init --recursive

$(BUILD_DIR)/build.ninja: CMakeLists.txt lib/JUCE/CMakeLists.txt
	cmake -B $(BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build-plugin: $(BUILD_DIR)/build.ninja
	cmake --build $(BUILD_DIR)

release:
	$(MAKE) BUILD_TYPE=Release build-plugin

test: build-plugin
	cd $(BUILD_DIR) && ctest --output-on-failure

clean:
	rm -rf build

validate: build-plugin
	killall -9 AudioComponentRegistrar 2>/dev/null; true
	sleep 1
	auval -v aumu Mngr Whim

uninstall:
	rm -rf "$(HOME)/Library/Audio/Plug-Ins/VST3/Mangrove.vst3"
	rm -rf "$(HOME)/Library/Audio/Plug-Ins/Components/Mangrove.component"
	killall -9 AudioComponentRegistrar 2>/dev/null; true
