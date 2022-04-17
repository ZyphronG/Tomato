CPP = g++

ifeq ($(OS),Windows_NT)
	TOMATO := Tomato.exe
else
	TOMATO := Tomato
endif

$(TOMATO): source/Tomato.cpp
	$(CPP) -Os -s -I include/ -fwhole-program -Wno-multichar -Wall -o $@ $<
	

clean:
	rm -f $(TOMATO)
