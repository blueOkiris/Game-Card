# Project settings
OBJNAME :=			game-card
SRC :=				$(wildcard pico/src/*.cpp)
HEADERS :=			$(wildcard pico/include/*.hpp)

# Helper targets
.PHONY : all
all : /tmp/pico-sdk $(OBJNAME).uf2

.PHONY : install-deps
	@if ! [ "$(shell id -u)" = 0 ];then
		@echo "You are not root, run this target as root please"
		exit 1
	fi
	apt install -y \
		git cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential

/tmp/pico-sdk :
	cd /tmp; git clone -b master https://github.com/raspberrypi/pico-sdk.git
	cd /tmp/pico-sdk; git submodule update --init

.PHONY : clean
clean :
	rm -rf /tmp/pico-sdk
	rm -rf pico/build
	rm -rf $(OBJNAME).uf2

# Main targets
$(OBJNAME).uf2 : $(SRC) $(HEADERS) pico/CMakeLists.txt
	mkdir -p pico/build
	cd pico/build; PICO_SDK_PATH=/tmp/pico-sdk cmake ..
	cd pico/build; make
	cp pico/build/$(OBJNAME).uf2 .
