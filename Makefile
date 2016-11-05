SOURCE  = $ BOINCData.cpp\
            CPUData.cpp\
            NetworkData.cpp\
            LEDControl.cpp

HEADERS = $ BOINC_specs.h BOINCData.h\
            CPUData.h\
            NetworkData.h\
            LEDControl.h

CPP     = g++

CFLAGS += -fdiagnostics-color -Werror -Wall -Wextra -std=c++14 -pedantic -O2

LDFLAGS = -lwiringPi

status-leds: $(SOURCE) $(HEADERS) main.cpp
	$(CPP) $(CFLAGS) $(LDFLAGS) $(SOURCE) -o $@ main.cpp
	sudo setcap cap_net_raw,cap_dac_read_search+ep $@
	./export_gpio.sh

.PHONY: wp
wp:
	./build_wp

.PHONY: clean
clean:
	rm -f status-leds *~
	./clean_wp
