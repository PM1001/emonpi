MAINFILE = PCD8544
APPNAME = PCD8544.so
TESTAPPNAME = lcdtest
OUT_PATH = ./
OBJ_DIR = obj
MKDIR_P = mkdir -p

APP_SRC_PATH = ./
APP_INC_PATH = ./
BCM2835_SRC_PATH = ./bcm2835-1.36/src

SYNC = rsync -av --progress 
SYNC_DST = /home/pi/emonpi/lcd
SYNC_SRC = /mnt/remote_usb/lcd/

#$(PREFIX)/powerpc-eabi/include
#DATLIB_PATHS = -L $(PREFIX)/powerpc-eabi/lib    # -lgcc
CC = g++ -c
LD = g++ 
LDFLAGS = -shared 


#g++ -o lcdtest  bcm2835.o p_4094driverBCM.o PCD8544.o lcdtest.o
#Application defines
CPPFLAGS  = -std=c++11 -fPIC
CFLAGS  = -fPIC
ifeq ($(DEBUG),1)
  CPPFLAGS+= -g -fstack-protector-all
else
  CPPFLAGS+= -O3 -DNDEBUG
endif
ifeq ($(OS),Windows_NT)
else
  CPPFLAGS+= -DPOSIX
endif


AP_SRC  = $(APP_SRC_PATH)/lcdinterface.c
APP_SRC = $(APP_SRC_PATH)/p_4094driverBCM.cpp
APP_SRC += $(APP_SRC_PATH)/PCD8544.cpp
APP_SRC += $(APP_SRC_PATH)/pcdWrapper.cpp
APP_SRC += $(APP_SRC_PATH)/fontLiberationSans14x14.cpp
TEST_SRC = $(APP_SRC_PATH)/lcdtest.c
BCM_SRC  = $(BCM2835_SRC_PATH)/bcm2835.c
APP_INCLUDE_PATH  = -I$(APP_INC_PATH)
APP_INCLUDE_PATH += -I$(BCM2835_SRC_PATH)

APP_OBJ    = $(patsubst $(APP_SRC_PATH)/%.cpp,$(OBJ_DIR)/%.o,$(APP_SRC))
AP_OBJ     = $(patsubst $(APP_SRC_PATH)/%.c,$(OBJ_DIR)/%.o,$(AP_SRC))
TEST_OBJ   = $(patsubst $(APP_SRC_PATH)/%.c,$(OBJ_DIR)/%.o,$(TEST_SRC))
BCM_OBJ    = $(patsubst $(BCM_SRC_PATH)/%.c,$(OBJ_DIR)/%.o,$(BCM_SRC))

$(APP_OBJ): CPPFLAGS+= $(APP_FLAGS) $(APP_INCLUDE_PATH) 
$(TEST_OBJ):CPPFLAGS+= $(APP_FLAGS) $(APP_INCLUDE_PATH) 
$(BCM_OBJ): CPPLAGS+= $(APP_FLAGS) $(APP_INCLUDE_PATH) 
$(AP_OBJ): CPPFLAGS+= $(APP_FLAGS) $(APP_INCLUDE_PATH) 	

.PHONY: directories

all: directories lib lcd

${SYNC}: 
		${SYNC} ${SYNC_SRC} ${SYNC_DST}

${OBJ_DIR}:
		${MKDIR_P} ${OBJ_DIR}

$(AP_OBJ):$(OBJ_DIR)/%.o: $(APP_SRC_PATH)/%.c
	@echo 'Building file: $<'
	$(CC) $(CPPFLAGS) $< -o $@
	@echo 'Finished building: $<'
$(APP_OBJ):$(OBJ_DIR)/%.o: $(APP_SRC_PATH)/%.cpp
	@echo 'Building file: $<'
	$(CC) $(CPPFLAGS) $< -o $@
	@echo 'Finished building: $<'
$(TEST_OBJ):$(OBJ_DIR)/%.o: $(APP_SRC_PATH)/%.c
	@echo 'Building file: $<'
	$(CC) $(CPPFLAGS) $< -o $@
	@echo 'Finished building: $<'
$(BCM_OBJ):$(OBJ_DIR)/%.o: $(BCM2835_SRC_PATH)/%.c
	@echo 'Building file: $<'
	$(CC) $(CPPFLAGS) $< -o $@
	@echo 'Finished building: $<'

clean:
	rm -f $(OUT_PATH)/$(APPNAME)
	rm -f $(OBJ_DIR)/*.o
#operational application 
lib: $(AP_OBJ)  $(BCM_OBJ) $(APP_OBJ)
	@echo 'Linking'
	$(LD) $(LDFLAGS) -Wl,-soname,$(APPNAME) -o $(OUT_PATH)/$(APPNAME) $(AP_OBJ) $(APP_OBJ) $(BCM_OBJ) 
	@echo 'All Done'

#operational application 
lcd: $(BCM_OBJ) $(APP_OBJ)  $(TEST_OBJ) 
	@echo 'Linking'
	$(LD) $(LDFLAGS) -o $(OUT_PATH)/$(TESTAPPNAME) $(APP_OBJ) $(TEST_OBJ)  $(BCM_OBJ) 
	@echo 'All Done'

directories: ${OUT_DIR}

remote:rsync