# Install debian dependencies
sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0

# Make sure you have python3 
python --version

# Install ESP-IDF SDK
mkdir -p ~/esp
cd ~/esp
git clone -b v5.4.1 --recursive https://github.com/espressif/esp-idf.git

# Setup ESP-ID tools for esp32
cd ~/esp/esp-idf
./install.sh esp32
