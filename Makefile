# Project settings for main game card file
OBJNAME :=			game-card
PROJFOLDER :=		game-card
SRC :=				$(wildcard $(PROJFOLDER)/src/*.cpp) \
					$(wildcard common/src/*.cpp)
HEADERS :=			$(wildcard $(PROJFOLDER)/include/*.hpp) \
					$(wildcard common/include/*.hpp)

# Project settings for PC side of rom writer (C# proj)
WR_PC_OBJNAME :=	rom-writer-pc
WR_PC_PROJFOLDER :=	rom-writer
WR_PC_SRC :=		$(wildcard $(WR_PC_PROJFOLDER)/*.cs) \
					$(wildcard $(WR_PC_PROJFOLDER)/pc.csproj)
WR_PC_ARCH :=		linux-x64

# Project settings for game card assembler
AS_OBJNAME :=		gca
AS_PROJFOLDER :=	assembler
AS_SRC :=			$(wildcard $(AS_PROJFOLDER)/*.cs) \
					$(wildcard $(AS_PROJFOLDER)/assembler.csproj)
AS_ARCH :=			linux-x64

# Helper targets
.PHONY : all
all : /tmp/pico-sdk $(OBJNAME).uf2 $(WR_PC_OBJNAME) $(AS_OBJNAME)

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
	rm -rf *.uf2
	rm -rf $(WR_PC_OBJNAME)
	rm -rf $(WR_PC_PROJFOLDER)/obj
	rm -rf $(WR_PC_PROJFOLDER)/bin
	rm -rf $(AS_OBJNAME)
	rm -rf $(AS_PROJFOLDER)/obj
	rm -rf $(AS_PROJFOLDER)/bin
	rm -rf writer-proj
	rm -rf libSystem.IO.Ports.Native.so 

# Main targets
$(OBJNAME).uf2 : /tmp/pico-sdk $(SRC) $(HEADERS) $(PROJFOLDER)/CMakeLists.txt
	mkdir -p $(PROJFOLDER)/build
	cd $(PROJFOLDER)/build; PICO_SDK_PATH=/tmp/pico-sdk cmake ..
	cd $(PROJFOLDER)/build; make
	cp $(PROJFOLDER)/build/$(OBJNAME).uf2 .

$(WR_PC_OBJNAME) : /tmp/pico-sdk $(WR_PC_SRC)
	cd $(WR_PC_PROJFOLDER); \
		dotnet publish \
			-c Release -r $(WR_PC_ARCH) \
			-p:PublishSingleFile=true --self-contained true
	cp \
		$(WR_PC_PROJFOLDER)/bin/Release/net5.0/$(WR_PC_ARCH)/publish/pc \
		$(WR_PC_OBJNAME)

game-writer.uf2 : /tmp/pico-sdk $(WR_PC_OBJNAME) writer-proj/* $(wildcard common/include/*.hpp) $(wildcard common/src/*.cpp)
	mkdir -p writer-proj/build
	cd writer-proj/build; PICO_SDK_PATH=/tmp/pico-sdk cmake ..
	cd writer-proj/build; make
	cp writer-proj/build/game-writer.uf2 .

$(AS_OBJNAME) : $(AS_SRC)
	cd $(AS_PROJFOLDER); \
		dotnet publish \
			-c Release -r $(AS_ARCH) \
			-p:PublishSingleFile=true --self-contained true
	cp \
		$(AS_PROJFOLDER)/bin/Release/net5.0/$(AS_ARCH)/publish/assembler \
		$(AS_OBJNAME)
