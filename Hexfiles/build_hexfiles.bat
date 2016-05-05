@echo off

echo.

avr-gcc --version

REM -------------------------------------------------------------------
REM BUILD CHAIN OF FILENAMES FOR LINKER

setlocal enabledelayedexpansion
SET foo=
for %%X in (..\Firmware_Sources\*.c) do SET foo=!foo! .\%%~nX.o
REM -------------------------------------------------------------------


echo Compiling sources for ATMega328p and RFM69...

for %%X in (..\Firmware_Sources\*.c) do avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Sources/%%~nX.c"

avr-gcc -Wl,--relax,-Map,Pyro_atmega328p_RFM69.map -flto -Os -mmcu=atmega328p -o "Pyro_atmega328p_RFM69.elf" %foo%

avr-size --format=avr --mcu=atmega328p Pyro_atmega328p_RFM69.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_atmega328p_RFM69.elf "Pyro_atmega328p_RFM69.hex"

copy "Pyro_atmega328p_RFM69.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q %%a
echo Done
echo.


REM -------------------------------------------------------------------


echo Compiling sources for ATMega328p and RFM69 (High Power)...

for %%X in (..\Firmware_Sources\*.c) do avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -DP_OUT_DBM=13 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Sources/%%~nX.c"

avr-gcc -Wl,--relax,-Map,Pyro_atmega328p_RFM69_HP.map -flto -Os -mmcu=atmega328p -o "Pyro_atmega328p_RFM69_HP.elf" %foo%

avr-size --format=avr --mcu=atmega328p Pyro_atmega328p_RFM69_HP.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_atmega328p_RFM69_HP.elf "Pyro_atmega328p_RFM69_HP.hex"

copy "Pyro_atmega328p_RFM69_HP.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q %%a
echo Done
echo.


REM -------------------------------------------------------------------


echo Compiling sources for ATMega328p and RFM12...


for %%X in (..\Firmware_Sources\*.c) do avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=12 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Sources/%%~nX.c"

avr-gcc -Wl,--relax,-Map,Pyro_atmega328p_RFM12.map -flto -Os -mmcu=atmega328p -o "Pyro_atmega328p_RFM12.elf" %foo%

avr-size --format=avr --mcu=atmega328p Pyro_atmega328p_RFM12.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_atmega328p_RFM12.elf "Pyro_atmega328p_RFM12.hex"

copy "Pyro_atmega328p_RFM12.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q %%a
echo Done
echo.


REM -------------------------------------------------------------------


echo Compiling sources for ATMega168p and RFM69...

for %%X in (..\Firmware_Sources\*.c) do avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Sources/%%~nX.c"

avr-gcc -Wl,--relax,-Map,Pyro_atmega168p_RFM69.map -flto -Os -mmcu=atmega168p -o "Pyro_atmega168p_RFM69.elf" %foo%

avr-size --format=avr --mcu=atmega168p Pyro_atmega168p_RFM69.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_atmega168p_RFM69.elf "Pyro_atmega168p_RFM69.hex"

copy "Pyro_atmega168p_RFM69.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q %%a
echo Done
echo.


REM -------------------------------------------------------------------


echo Compiling sources for ATMega168p and RFM12...


for %%X in (..\Firmware_Sources\*.c) do avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=12 -mmcu=atmega168p -DMCU=atmega168p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Sources/%%~nX.c"

avr-gcc -Wl,--relax,-Map,Pyro_atmega168p_RFM12.map -flto -Os -mmcu=atmega168p -o "Pyro_atmega168p_RFM12.elf" %foo%

avr-size --format=avr --mcu=atmega168p Pyro_atmega168p_RFM12.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_atmega168p_RFM12.elf "Pyro_atmega168p_RFM12.hex"

copy "Pyro_atmega168p_RFM12.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q %%a
echo Done
echo.

timeout /T 10  > nul