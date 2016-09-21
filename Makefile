CFLAGS += -L./
LIBS := -lpthread
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))
DEPS := $(patsubst %.o,%.d,$(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.c,$(MISSING_DEPS)))


ifeq ($(debug),n)
CFLAGS += -s -O2 -Wall -MD
else
CFLAGS += -g -O0 -Wall -MD -DDEBUG
endif

TARGET := flow_monitor

all:$(TARGET)

ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS) :
	@$(RM) $(patsubst %.d,%.o,$@)
endif
-include $(DEPS)

$(TARGET):$(OBJS) 
	$(CC) $(CFLAGS) -o $(TARGET) $^ $(LIBS)
#	strip $(TARGET)

$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.d *.s *.o $(TARGET) *.o
	
