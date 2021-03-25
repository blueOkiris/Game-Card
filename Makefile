# Project settings
OBJNAME :=			game-card
PROJFOLDER :=		game-card
SRC :=				$(wildcard $(PROJFOLDER)/src/*.cpp) \
					$(wildcard common/src/*.cpp)
HEADERS :=			$(wildcard $(PROJFOLDER)/include/*.hpp) \
					$(wildcard common/include/*.hpp)

WR_OBJNAME :=	rom-writer
WR_PROJFOLDER :=	rom-writer/pico
WR_SRC :=		$(wildcard $(WR_PROJFOLDER)/src/*.cpp) \
					$(wildcard common/src/*.cpp)
WR_HEADERS :=	$(wildcard common/include/*.hpp)

# Helper targets
.PHONY : all
all : /tmp/pico-sdk $(OBJNAME).uf2 $(WR_OBJNAME).uf2

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
	rm -rf $(PROJFOLDER)/build
	rm -rf $(WR_PROJFOLDER)/build
	rm -rf *.uf2

# Main targets
$(OBJNAME).uf2 : /tmp/pico-sdk $(SRC) $(HEADERS) $(PROJFOLDER)/CMakeLists.txt
	mkdir -p $(PROJFOLDER)/build
	cd $(PROJFOLDER)/build; PICO_SDK_PATH=/tmp/pico-sdk cmake ..
	cd $(PROJFOLDER)/build; make
	cp $(PROJFOLDER)/build/$(OBJNAME).uf2 .

$(WR_OBJNAME).uf2 : /tmp/pico-sdk $(WR_SRC) $(WR_HEADERS) $(WR_PROJFOLDER)/CMakeLists.txt
	mkdir -p $(WR_PROJFOLDER)/build
	cd $(WR_PROJFOLDER)/build; PICO_SDK_PATH=/tmp/pico-sdk cmake ..
	cd $(WR_PROJFOLDER)/build; make
	cp $(WR_PROJFOLDER)/build/$(WR_OBJNAME).uf2 .
