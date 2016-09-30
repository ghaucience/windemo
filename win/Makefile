include ./make/arch.mk

TARGET  := main.exe
targets := $(TARGET)
midobjs := ./*.obj ./*.res

include ./make/rules.mk

run: $(TARGET)
	@./main.exe
	
$(eval $(call LinkApp,$(TARGET),main.obj Serial.obj Proto.obj res.res))
