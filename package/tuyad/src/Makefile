CFLAGS += -std=gnu99
LDFLAGS += -llink_core -lutils_modules -lplatform_port -lmiddleware_implementation -lblobmsg_json -lubox -lubus -luci -lcjson

SRCS = main.c cpu.c file_reader.c network_interface.c system.c user_input.c tuya.c helpers.c config.c
OBJS = $(SRCS:.c=.o)
TARGET = tuyad

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)