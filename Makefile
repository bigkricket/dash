APP ?= transmitter

all:
	cd $(APP) && idf.py build

flash:
	cd $(APP) && idf.py -p /dev/ttyUSB0 flash monitor
