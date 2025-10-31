default: compile

compile:
	pio run

upload:
	pio run -t upload

clean:
	rm -rf .pio