CPP = g++

Tomato.exe: source/Tomato.cpp
	$(CPP) -Os -I include/ -ffunction-sections -fdata-sections -Wl,--gc-sections -s -Wno-multichar -Wall -o $@ $<