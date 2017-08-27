

all: PLA Pocket Dstump


PLA: HW1/PLA.c
	gcc $^ -o $<.exe

Pocket: HW1/Pocket.c
	gcc $^ -o $<.exe

Dstump: HW2/Dstump.c
	gcc $^ -o $<.exe

clean:
	rm -f *.exe