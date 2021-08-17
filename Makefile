## Settings

# Generic settings
LIB_FILES :=			$(wildcard libraries/GameCardControls/*.hpp) \
						$(wildcard libraries/GameCardControls/*.cpp) \
						$(wildcard libraries/GameCardDisplay/*.hpp) \
						$(wildcard libraries/GameCardDisplay/*.cpp) \

I2C_LIB_URL :=			https://github.com/adafruit/TinyWireM
I2C_LIB_NAME :=			TinyWireM
I2C_LIB_FILES :=		$(wildcard $(I2C_LIB_NAME)/*.h) \
						$(wildcard $(I2C_LIB_NAME)/*.cpp)

ARD_CLI_WIN_URL :=		https://github.com/arduino/arduino-cli/releases/download/0.18.3/arduino-cli_0.18.3_Windows_64bit.zip
ARD_CLI_LIN_URL	:=		https://github.com/arduino/arduino-cli/releases/download/0.18.3/arduino-cli_0.18.3_Linux_64bit.tar.gz
ARD_CLI_FLD_NAME :=		arduino-cli
ARD_CLI_FNAME :=		$(ARD_CLI_FLD_NAME)/arduino-cli
ARD_CLI_CONF_FILE :=	arduino-cli.yaml
ARD_CLI_PROG :=			arduinoasisp

TINY_BOARD_FAMILY :=	attiny:avr
TINY_BOARD_FQBN :=		attiny:avr:ATtinyX4

DEF_TINY_BOARDS :=		arduino-cli/data/packages/attiny/hardware/avr/1.0.2/boards.txt
REPL_TINY_BOARDS :=		boards-w-new-defaults.txt

ifeq ($(OS), Windows_NT)
	BUILD_PATH :=		C:\build
else
	BUILD_PATH :=		/tmp/build
endif

## Helper Targets

.PHONY : all
all : upload

.PHONY : burn-bootloader
burn-bootloader : $(ARD_CLI_FNAME)
	$(ARD_CLI_FNAME) burn-bootloader \
		--config-file=$(ARD_CLI_CONF_FILE) --fqbn $(TINY_BOARD_FQBN) \
		-P $(ARD_CLI_PROG) -p $(PORT)

libraries/$(I2C_LIB_NAME) :
	git clone $(I2C_LIB_URL) $@

$(ARD_CLI_FNAME) :
	curl -L $(ARD_CLI_WIN_URL) -o arduino-cli-win.zip
	curl -L $(ARD_CLI_LIN_URL) -o arduino-cli-lin.tar.gz
	-mkdir arduino-cli
	-tar -xf arduino-cli-win.zip -C arduino-cli
	tar -xf arduino-cli-lin.tar.gz -C arduino-cli
	
	$(ARD_CLI_FNAME) --config-file=$(ARD_CLI_CONF_FILE) core update-index
	$(ARD_CLI_FNAME) --config-file=$(ARD_CLI_CONF_FILE) core install arduino:avr
	$(ARD_CLI_FNAME) --config-file=$(ARD_CLI_CONF_FILE) core install $(TINY_BOARD_FAMILY)
	
	-rm -rf $(DEF_TINY_BOARDS)
	-cp $(REPL_TINY_BOARDS) $(DEF_TINY_BOARDS)

	-del $(subst /,\,$(DEF_TINY_BOARDS))
	-cmd /K "copy $(REPL_TINY_BOARDS) $(subst /,\,$(DEF_TINY_BOARDS)) & exit"

.PHONY : clean
clean :
	-rm -rf arduino-cli-win.zip
	-rm -rf arduino-cli-lin.tar.gz
	-rm -rf arduino-cli
	-rm -rf $(BUILD_PATH_LIN)
	-rm -rf libraries/$(I2C_LIB_NAME)
	-rm -rf Ping.ino.hex
	-rm -rf Snek.ino.hex
	-del arduino-cli-win.zip
	-del arduino-cli-lin.tar.gz
	-rmdir /q /s arduino-cli
	-rmdir /q /s $(BUILD_PATH_WIN)
	-rmdir /q /s libraries\$(I2C_LIB_NAME)
	-del Ping.ino.hex
	-del Snek.ino.hex

## Main Targets

# Build Game
$(GAME).ino.hex : $(ARD_CLI_FNAME) $(LIB_FILES) libraries/$(I2C_LIB_NAME) $(wildcard libraries/$(I2C_LIB_NAME)/*.h) $(wildcard libraries/$(I2C_LIB_NAME)/*.cpp) $(wilcard games/$(GAME)/*)
	-mkdir $(BUILD_PATH)
	$(ARD_CLI_FNAME) compile \
		--config-file=$(ARD_CLI_CONF_FILE) \
		--fqbn $(TINY_BOARD_FQBN) \
		--build-path $(BUILD_PATH) \
		--libraries ./libraries,. \
		games/$(GAME)
	-cp $(BUILD_PATH)/$@ .
	-cmd /K "copy $(BUILD_PATH)\$@ . & exit"

# Build Test
$(TEST).ino.hex : $(ARD_CLI_FNAME) $(LIB_FILES) libraries/$(I2C_LIB_NAME) $(wildcard libraries/$(I2C_LIB_NAME)/*.h) $(wildcard libraries/$(I2C_LIB_NAME)/*.cpp) $(wilcard tests/$(TEST)/*)
	-mkdir $(BUILD_PATH)
	$(ARD_CLI_FNAME) compile \
		--config-file=$(ARD_CLI_CONF_FILE) \
		--fqbn $(TINY_BOARD_FQBN) \
		--build-path $(BUILD_PATH) \
		--libraries ./libraries,. \
		tests/$(TEST)
	-cp $(BUILD_PATH)/$@ .
	-cmd /K "copy $(BUILD_PATH)\$@ . & exit"

# Upload a game
# Takes paramaters PORT and GAME
.PHONY : upload
upload : $(GAME).ino.hex
	$(ARD_CLI_FNAME) upload \
		--fqbn $(TINY_BOARD_FQBN) \
		-P $(ARD_CLI_PROG) -p $(PORT) -i $<
		
# Upload a test file
# Takes paramaters PORT and TEST
.PHONY : upload-test
upload-test : $(TEST).ino.hex
	$(ARD_CLI_FNAME) upload \
		--fqbn $(TINY_BOARD_FQBN) \
		-P $(ARD_CLI_PROG) -p $(PORT) -i $<