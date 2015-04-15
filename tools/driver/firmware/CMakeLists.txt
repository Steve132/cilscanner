set(CMAKE_TOOLCHAIN_FILE arduino-cmake-master/cmake/ArduinoToolchain.cmake) # Arduino Toolchain
cmake_minimum_required(VERSION 2.8)
#====================================================================#
# Setup Project #
#====================================================================#
project(CILScannerFirmware C CXX)
print_board_list()
print_programmer_list()

set(ARDUINO_DEFAULT_BOARD uno) # Default Board ID, when not specified
set(ARDUINO_DEFAULT_PORT /dev/ttyUSB0) # Default Port, when not specified

generate_arduino_firmware(cilscanner_firmware
	SRCS cilscanner.ino
	BOARD uno)

