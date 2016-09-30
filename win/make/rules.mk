subdirs		+=
targets		+=
midobjs		+=

all : $(subdirs) $(targets)

%$(OEXT): %$(CPPEXT)
	$(CPP) $(COMPILE) $< $(CPPFLAGS) $(CCOUT)"$@"

%$(OEXT): %$(CEXT)
	$(CC) $(COMPILE) $< $(CFLAGS) $(CCOUT)"$@"

%$(RESEXT): %$(RCEXT)
	$(RCC) $(RCCFLAGS) $(RCOUT) $@ $<

$(subdirs):
	make -C $@

clean:
	$(foreach dir,$(subdirs),make -C $(dir) clean;)
	$(RM) $(RMFLAGS) $(targets)
	$(RM) $(RMFLAGS) $(midobjs)

define LinkApp
$1: $2
	$(LD) $(LDFLAGS) $2 $(LDOUT)"$1"
endef

