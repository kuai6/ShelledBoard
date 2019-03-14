How to build and debug project?

1. Install latest gcc-arm-none-eabi
2. create file /etc/ld.so.conf.d with following content
"/opt/gcc-arm-none-eabi-8-2018-q4-major/bin"
and run command
"sudo ldconfig"
3. Install latest openocd into the /opt/openocd directory
4. Setup build tool in Netbeans as following:
    Base directory:     /opt/gcc-arm-none-eabi-8-2018-q4-major/bin
    C compiler:         /opt/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-gcc
    C++ compiler:       /opt/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-g++
    Assembler:          /opt/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-as
    Make command:       /usr/bin/make
    Debugger command:   /opt/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-gdb
    Cmake command:      /usr/bin/cmake
5. Create project and build
6. Deploy .elf file to microcontroller:
    openocd -c "tcl_port disabled" -s /opt/openocd/0.10.0-11-20190118-1134/scripts -f board/st_nucleo_f103rb.cfg -c "program path_to_my_binary.elf" -c "init;reset init;"
or (only debug)
    openocd -c "tcl_port disabled" -s /opt/openocd/0.10.0-11-20190118-1134/scripts -f board/st_nucleo_f103rb.cfg
