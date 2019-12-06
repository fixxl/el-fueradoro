@echo off

echo.

avr-gcc --version

for /f %%X in ('date /t ^| gawk -F"." "{gsub(/ /, \"\", $0); print $3$2$1;}"') do SET dtstrng=%%X
echo Datestring is %dtstrng%
for /f %%X in ('time /t ^| gawk -F":" "{gsub(/ /, \"\", $0); print $1$2;}"') do SET tmstrng=%%X
echo Timestring is %tmstrng%

REM -------------------------------------------------------------------
REM BUILD CHAIN OF FILENAMES FOR LINKER

setlocal enabledelayedexpansion
SET foo=
for %%X in (..\Firmware_Sources\*.c) do SET foo=!foo! .\%%~nX.o

SET foo3=
for %%X in (..\Firmware_Zuendbox_v3\*.c) do SET foo3=!foo3! .\%%~nX.o
REM -------------------------------------------------------------------

SET maxId=50
SET channels=16

:repeat
set var=%1
set para=%2
if "%var:~0,1%"=="-" (
    if "%var%"=="-ch" (
        set channels=%para%
    )
    if "%var%"=="-id" (
        shift
        set maxId=%para%
    )
    shift
    goto repeat
)

if %maxId% GTR 250 set maxId=250
if %maxId% LSS 1 set maxId=1

echo.
echo Maximum number of IDs: %maxId%
echo Number of channels: %channels%
echo.

echo Compiling sources for ATMega328p and RFM69...

for %%X in (..\Firmware_Sources\*.c) do avr-gcc -Wall -Wextra -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -DCOMPILEDATE=%dtstrng% -DCOMPILETIME=%tmstrng% -DMAX_ID=%maxId% -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Sources/%%~nX.c"

avr-gcc -Wl,-Map,Pyro_atmega328p_RFM69.map -flto -Os -mrelax -mmcu=atmega328p -o "Pyro_atmega328p_RFM69.elf" %foo%

avr-size --format=avr --mcu=atmega328p Pyro_atmega328p_RFM69.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_atmega328p_RFM69.elf "Pyro_atmega328p_RFM69.hex"

copy "Pyro_atmega328p_RFM69.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q "%%a"
echo Done
echo.


REM -------------------------------------------------------------------


echo Compiling sources for ATMega328p and RFM69 (High Power)...

for %%X in (..\Firmware_Sources\*.c) do avr-gcc -Wall -Wextra -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -DCOMPILEDATE=%dtstrng% -DCOMPILETIME=%tmstrng% -DMAX_ID=%maxId% -DP_OUT_DBM=13 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Sources/%%~nX.c"

avr-gcc -Wl,-Map,Pyro_atmega328p_RFM69_HP.map -flto -Os -mrelax -mmcu=atmega328p -o "Pyro_atmega328p_RFM69_HP.elf" %foo%

avr-size --format=avr --mcu=atmega328p Pyro_atmega328p_RFM69_HP.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_atmega328p_RFM69_HP.elf "Pyro_atmega328p_RFM69_HP.hex"

copy "Pyro_atmega328p_RFM69_HP.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q "%%a"
echo Done
echo.


REM -------------------------------------------------------------------


echo Compiling v3-sources for ATMega328p and RFM69...

for %%X in (..\Firmware_Zuendbox_v3\*.c) do avr-gcc -Wall -Wextra -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -DCOMPILEDATE=%dtstrng% -DCOMPILETIME=%tmstrng% -DMAX_ID=%maxId% -DSR_CHANNELS=%channels% -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Zuendbox_v3/%%~nX.c"

avr-gcc -Wl,-Map,Pyro_v3_atmega328p_RFM69.map -flto -Os -mrelax -mmcu=atmega328p -o "Pyro_v3_atmega328p_RFM69.elf" %foo3%

avr-size --format=avr --mcu=atmega328p Pyro_v3_atmega328p_RFM69.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_v3_atmega328p_RFM69.elf "Pyro_v3_atmega328p_RFM69.hex"

copy "Pyro_v3_atmega328p_RFM69.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q "%%a"
echo Done
echo.

REM -------------------------------------------------------------------

echo Compiling v3-sources for ATMega328p and RFM69 (High Power, 16 Channels)...

for %%X in (..\Firmware_Zuendbox_v3\*.c) do avr-gcc -Wall -Wextra -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -DCOMPILEDATE=%dtstrng% -DCOMPILETIME=%tmstrng% -DMAX_ID=%maxId% -DSR_CHANNELS=16UL -DP_OUT_DBM=13 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Zuendbox_v3/%%~nX.c"

avr-gcc -Wl,-Map,Pyro_v3_atmega328p_RFM69_HP.map -flto -Os -mrelax -mmcu=atmega328p -o "Pyro_v3_atmega328p_RFM69_HP.elf" %foo3%

avr-size --format=avr --mcu=atmega328p Pyro_v3_atmega328p_RFM69_HP.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_v3_atmega328p_RFM69_HP.elf "Pyro_v3_atmega328p_RFM69_HP.hex"

copy "Pyro_v3_atmega328p_RFM69_HP.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q "%%a"
echo Done
echo.

REM -------------------------------------------------------------------

echo Compiling v3-sources for ATMega328p and RFM69 (High Power, 32 Channels)...

for %%X in (..\Firmware_Zuendbox_v3\*.c) do avr-gcc -Wall -Wextra -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -DCOMPILEDATE=%dtstrng% -DCOMPILETIME=%tmstrng% -DMAX_ID=%maxId% -DSR_CHANNELS=32UL -DP_OUT_DBM=13 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Zuendbox_v3/%%~nX.c"

avr-gcc -Wl,-Map,Pyro_v3_atmega328p_RFM69_HP_32.map -flto -Os -mrelax -mmcu=atmega328p -o "Pyro_v3_atmega328p_RFM69_HP_32.elf" %foo3%

avr-size --format=avr --mcu=atmega328p Pyro_v3_atmega328p_RFM69_HP_32.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_v3_atmega328p_RFM69_HP_32.elf "Pyro_v3_atmega328p_RFM69_HP_32.hex"

copy "Pyro_v3_atmega328p_RFM69_HP_32.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q "%%a"
echo Done
echo.


timeout /T 10  > nul