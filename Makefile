CPP = g++

ifeq ($(OS),Windows_NT)
	TOMATO := Tomato.exe
else
	TOMATO := Tomato
endif

$(TOMATO): source/Tomato.cpp
	$(CPP) -Os -I include/ -fno-rtti -fno-exceptions -ffunction-sections -fdata-sections -Wl,--gc-sections -s -Wno-multichar -Wall -o $@ $<

clean:
	rm -f $(TOMATO)
