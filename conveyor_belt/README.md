Conveyor Belt Control
=====================


## Requirements

- gcc
- make
- cmake
- modbus
- protobuf
- zeromq
- boost

## Debian Based Systems

    sudo apt-get install build-essential cmake libmodbus-dev libprotobuf-dev \
	libzmq-dev libboost-system-dev libprotoc-dev protobuf-compiler

## Arch Linux

    sudo pacman -S cmake protobuf boost zeromq
   
And from the AUR: libmodbus

    sudo aura -A libmodbus

## Build & Install

    mkdir ./build
    cd ./build 
    cmake ..
    make
    
