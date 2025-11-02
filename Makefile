.PHONY: all clean

all:
	pio run -t upload
	pio device monitor --baud 115200

clean:
	rm -rf .pio