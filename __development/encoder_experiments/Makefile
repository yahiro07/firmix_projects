build:
	pio run

flash:
	pio run --target upload

write_uf2:
	cp .pio/build/generic/firmware.uf2 /media/ore/RPI-RP2/firmware.uf2

# debug: flash monitor

debug: build write_uf2
