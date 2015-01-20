@echo off


echo Compiling sources for ATMega328p and RFM69...

avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"crcchk.d" -MT"crcchk.d" -c -o "crcchk.o" "../Firmware_Sources/crcchk.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"adc.d" -MT"adc.d" -c -o "adc.o" "../Firmware_Sources/adc.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"1wire.d" -MT"1wire.d" -c -o "1wire.o" "../Firmware_Sources/1wire.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"addresses.d" -MT"addresses.d" -c -o "addresses.o" "../Firmware_Sources/addresses.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"dht.d" -MT"dht.d" -c -o "dht.o" "../Firmware_Sources/dht.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"eeprom.d" -MT"eeprom.d" -c -o "eeprom.o" "../Firmware_Sources/eeprom.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"leds.d" -MT"leds.d" -c -o "leds.o" "../Firmware_Sources/leds.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"lcd.d" -MT"lcd.d" -c -o "lcd.o" "../Firmware_Sources/lcd.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"pyro.d" -MT"pyro.d" -c -o "pyro.o" "../Firmware_Sources/pyro.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"rfm12.d" -MT"rfm12.d" -c -o "rfm12.o" "../Firmware_Sources/rfm12.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"rfm69.d" -MT"rfm69.d" -c -o "rfm69.o" "../Firmware_Sources/rfm69.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"shiftregister.d" -MT"shiftregister.d" -c -o "shiftregister.o" "../Firmware_Sources/shiftregister.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"terminal.d" -MT"terminal.d" -c -o "terminal.o" "../Firmware_Sources/terminal.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"timer.d" -MT"timer.d" -c -o "timer.o" "../Firmware_Sources/timer.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"uart.d" -MT"uart.d" -c -o "uart.o" "../Firmware_Sources/uart.c"

avr-gcc -Wl,-Map,Pyro_atmega328p_RFM69.map -flto -Os -mmcu=atmega328p -o "Pyro_atmega328p_RFM69.elf" ./1wire.o ./adc.o ./addresses.o ./crcchk.o ./dht.o ./eeprom.o ./lcd.o ./leds.o ./pyro.o ./rfm12.o ./rfm69.o ./shiftregister.o ./terminal.o ./timer.o ./uart.o

avr-size --format=avr --mcu=atmega328p Pyro_atmega328p_RFM69.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_atmega328p_RFM69.elf "Pyro_atmega328p_RFM69.hex"

copy "Pyro_atmega328p_RFM69.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==build_hexfiles del /q %%a
echo Done
echo.
echo Compiling sources for ATMega328p and RFM12...


avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"crcchk.d" -MT"crcchk.d" -c -o "crcchk.o" "../Firmware_Sources/crcchk.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"adc.d" -MT"adc.d" -c -o "adc.o" "../Firmware_Sources/adc.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"1wire.d" -MT"1wire.d" -c -o "1wire.o" "../Firmware_Sources/1wire.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"addresses.d" -MT"addresses.d" -c -o "addresses.o" "../Firmware_Sources/addresses.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"dht.d" -MT"dht.d" -c -o "dht.o" "../Firmware_Sources/dht.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"eeprom.d" -MT"eeprom.d" -c -o "eeprom.o" "../Firmware_Sources/eeprom.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"leds.d" -MT"leds.d" -c -o "leds.o" "../Firmware_Sources/leds.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"lcd.d" -MT"lcd.d" -c -o "lcd.o" "../Firmware_Sources/lcd.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"pyro.d" -MT"pyro.d" -c -o "pyro.o" "../Firmware_Sources/pyro.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"rfm12.d" -MT"rfm12.d" -c -o "rfm12.o" "../Firmware_Sources/rfm12.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"RFM12.d" -MT"rfm69.d" -c -o "rfm69.o" "../Firmware_Sources/rfm69.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"shiftregister.d" -MT"shiftregister.d" -c -o "shiftregister.o" "../Firmware_Sources/shiftregister.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"terminal.d" -MT"terminal.d" -c -o "terminal.o" "../Firmware_Sources/terminal.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"timer.d" -MT"timer.d" -c -o "timer.o" "../Firmware_Sources/timer.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"uart.d" -MT"uart.d" -c -o "uart.o" "../Firmware_Sources/uart.c"

avr-gcc -Wl,-Map,Pyro_atmega328p_RFM12.map -flto -Os -mmcu=atmega328p -o "Pyro_atmega328p_RFM12.elf" ./1wire.o ./adc.o ./addresses.o ./crcchk.o ./dht.o ./eeprom.o ./lcd.o ./leds.o ./pyro.o ./rfm12.o ./rfm69.o ./shiftregister.o ./terminal.o ./timer.o ./uart.o

avr-size --format=avr --mcu=atmega328p Pyro_atmega328p_RFM12.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_atmega328p_RFM12.elf "Pyro_atmega328p_RFM12.hex"

copy "Pyro_atmega328p_RFM12.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==build_hexfiles del /q %%a
echo Done
echo.

echo Compiling sources for ATMega168p and RFM69...

avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"crcchk.d" -MT"crcchk.d" -c -o "crcchk.o" "../Firmware_Sources/crcchk.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"adc.d" -MT"adc.d" -c -o "adc.o" "../Firmware_Sources/adc.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"1wire.d" -MT"1wire.d" -c -o "1wire.o" "../Firmware_Sources/1wire.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"addresses.d" -MT"addresses.d" -c -o "addresses.o" "../Firmware_Sources/addresses.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"dht.d" -MT"dht.d" -c -o "dht.o" "../Firmware_Sources/dht.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"eeprom.d" -MT"eeprom.d" -c -o "eeprom.o" "../Firmware_Sources/eeprom.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"leds.d" -MT"leds.d" -c -o "leds.o" "../Firmware_Sources/leds.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"lcd.d" -MT"lcd.d" -c -o "lcd.o" "../Firmware_Sources/lcd.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"pyro.d" -MT"pyro.d" -c -o "pyro.o" "../Firmware_Sources/pyro.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"rfm12.d" -MT"rfm12.d" -c -o "rfm12.o" "../Firmware_Sources/rfm12.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"rfm69.d" -MT"rfm69.d" -c -o "rfm69.o" "../Firmware_Sources/rfm69.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"shiftregister.d" -MT"shiftregister.d" -c -o "shiftregister.o" "../Firmware_Sources/shiftregister.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"terminal.d" -MT"terminal.d" -c -o "terminal.o" "../Firmware_Sources/terminal.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"timer.d" -MT"timer.d" -c -o "timer.o" "../Firmware_Sources/timer.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"uart.d" -MT"uart.d" -c -o "uart.o" "../Firmware_Sources/uart.c"

avr-gcc -Wl,-Map,Pyro_atmega168p_RFM69.map -flto -Os -mmcu=atmega168p -o "Pyro_atmega168p_RFM69.elf" ./1wire.o ./adc.o ./addresses.o ./crcchk.o ./dht.o ./eeprom.o ./lcd.o ./leds.o ./pyro.o ./rfm12.o ./rfm69.o ./shiftregister.o ./terminal.o ./timer.o ./uart.o

avr-size --format=avr --mcu=atmega168p Pyro_atmega168p_RFM69.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_atmega168p_RFM69.elf "Pyro_atmega168p_RFM69.hex"

copy "Pyro_atmega168p_RFM69.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==build_hexfiles del /q %%a
echo Done
echo.

echo Compiling sources for ATMega168p and RFM12...


avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"crcchk.d" -MT"crcchk.d" -c -o "crcchk.o" "../Firmware_Sources/crcchk.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"adc.d" -MT"adc.d" -c -o "adc.o" "../Firmware_Sources/adc.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"1wire.d" -MT"1wire.d" -c -o "1wire.o" "../Firmware_Sources/1wire.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"addresses.d" -MT"addresses.d" -c -o "addresses.o" "../Firmware_Sources/addresses.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"dht.d" -MT"dht.d" -c -o "dht.o" "../Firmware_Sources/dht.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"eeprom.d" -MT"eeprom.d" -c -o "eeprom.o" "../Firmware_Sources/eeprom.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"leds.d" -MT"leds.d" -c -o "leds.o" "../Firmware_Sources/leds.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"lcd.d" -MT"lcd.d" -c -o "lcd.o" "../Firmware_Sources/lcd.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"pyro.d" -MT"pyro.d" -c -o "pyro.o" "../Firmware_Sources/pyro.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"rfm12.d" -MT"rfm12.d" -c -o "rfm12.o" "../Firmware_Sources/rfm12.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"rfm69.d" -MT"rfm69.d" -c -o "rfm69.o" "../Firmware_Sources/rfm69.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"shiftregister.d" -MT"shiftregister.d" -c -o "shiftregister.o" "../Firmware_Sources/shiftregister.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"terminal.d" -MT"terminal.d" -c -o "terminal.o" "../Firmware_Sources/terminal.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"timer.d" -MT"timer.d" -c -o "timer.o" "../Firmware_Sources/timer.c"
avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"uart.d" -MT"uart.d" -c -o "uart.o" "../Firmware_Sources/uart.c"

avr-gcc -Wl,-Map,Pyro_atmega168p_RFM12.map -flto -Os -mmcu=atmega168p -o "Pyro_atmega168p_RFM12.elf" ./1wire.o ./adc.o ./addresses.o ./crcchk.o ./dht.o ./eeprom.o ./lcd.o ./leds.o ./pyro.o ./rfm12.o ./rfm69.o ./shiftregister.o ./terminal.o ./timer.o ./uart.o

avr-size --format=avr --mcu=atmega168p Pyro_atmega168p_RFM12.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_atmega168p_RFM12.elf "Pyro_atmega168p_RFM12.hex"

copy "Pyro_atmega168p_RFM12.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==build_hexfiles del /q %%a
echo Done
echo.
pause

