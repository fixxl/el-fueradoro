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

SET fooM=
for %%X in (..\Firmware_Minimodul\*.c) do SET fooM=!fooM! .\%%~nX.o
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

REM -------------------------------------------------------------------


echo Compiling v1/v2-sources for ATMega328p and RFM69CW (13 dBm, 16 Channels)...

for %%X in (..\Firmware_Sources\*.c) do avr-gcc -Wall -Wextra -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -DCOMPILEDATE=%dtstrng% -DCOMPILETIME=%tmstrng% -DMAX_ID=%maxId% -DP_OUT_DBM=13 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Sources/%%~nX.c"

avr-gcc -Wl,-Map,Pyro_atmega328p_RFM69CW_16ch.map -flto -Os -mrelax -mmcu=atmega328p -o "Pyro_atmega328p_RFM69CW_16ch.elf" %foo%

avr-size --format=avr --mcu=atmega328p Pyro_atmega328p_RFM69CW_16ch.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_atmega328p_RFM69CW_16ch.elf "Pyro_atmega328p_RFM69CW_16ch.hex"

copy "Pyro_atmega328p_RFM69CW_16ch.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q "%%a"
echo Done
echo.


REM -------------------------------------------------------------------


echo Compiling v1/v2-sources for ATMega328p and RFM69HCW (20 dBm, 16 Channels)...

for %%X in (..\Firmware_Sources\*.c) do avr-gcc -Wall -Wextra -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -DCOMPILEDATE=%dtstrng% -DCOMPILETIME=%tmstrng% -DMAX_ID=%maxId% -DP_OUT_DBM=20 -DHPVERSION=1 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Sources/%%~nX.c"

avr-gcc -Wl,-Map,Pyro_atmega328p_RFM69HCW_16ch.map -flto -Os -mrelax -mmcu=atmega328p -o "Pyro_atmega328p_RFM69HCW_16ch.elf" %foo%

avr-size --format=avr --mcu=atmega328p Pyro_atmega328p_RFM69HCW_16ch.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_atmega328p_RFM69HCW_16ch.elf "Pyro_atmega328p_RFM69HCW_16ch.hex"

copy "Pyro_atmega328p_RFM69HCW_16ch.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q "%%a"
echo Done
echo.

REM -------------------------------------------------------------------

echo Compiling v3-sources for ATMega328p and RFM69CW (13 dBm, 16 Channels)...

for %%X in (..\Firmware_Zuendbox_v3\*.c) do avr-gcc -Wall -Wextra -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -DCOMPILEDATE=%dtstrng% -DCOMPILETIME=%tmstrng% -DMAX_ID=%maxId% -DFIRE_CHANNELS=16UL -DP_OUT_DBM=13 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Zuendbox_v3/%%~nX.c"

avr-gcc -Wl,-Map,Pyro_v3_atmega328p_RFM69CW_16ch.map -flto -Os -mrelax -mmcu=atmega328p -o "Pyro_v3_atmega328p_RFM69CW_16ch.elf" %foo3%

avr-size --format=avr --mcu=atmega328p Pyro_v3_atmega328p_RFM69CW_16ch.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_v3_atmega328p_RFM69CW_16ch.elf "Pyro_v3_atmega328p_RFM69CW_16ch.hex"

copy "Pyro_v3_atmega328p_RFM69CW_16ch.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q "%%a"
echo Done
echo.

REM -------------------------------------------------------------------

echo Compiling v3-sources for ATMega328p and RFM69CW (13 dBm, 32 Channels)...

for %%X in (..\Firmware_Zuendbox_v3\*.c) do avr-gcc -Wall -Wextra -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -DCOMPILEDATE=%dtstrng% -DCOMPILETIME=%tmstrng% -DMAX_ID=%maxId% -DFIRE_CHANNELS=32UL -DP_OUT_DBM=13 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Zuendbox_v3/%%~nX.c"

avr-gcc -Wl,-Map,Pyro_v3_atmega328p_RFM69CW_32ch.map -flto -Os -mrelax -mmcu=atmega328p -o "Pyro_v3_atmega328p_RFM69CW_32ch.elf" %foo3%

avr-size --format=avr --mcu=atmega328p Pyro_v3_atmega328p_RFM69CW_32ch.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_v3_atmega328p_RFM69CW_32ch.elf "Pyro_v3_atmega328p_RFM69CW_32ch.hex"

copy "Pyro_v3_atmega328p_RFM69CW_32ch.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q "%%a"
echo Done
echo.

REM -------------------------------------------------------------------

echo Compiling v3-sources for ATMega328p and RFM69 (20dBm, 16 Channels)...

for %%X in (..\Firmware_Zuendbox_v3\*.c) do avr-gcc -Wall -Wextra -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -DCOMPILEDATE=%dtstrng% -DCOMPILETIME=%tmstrng% -DMAX_ID=%maxId% -DFIRE_CHANNELS=16UL -DP_OUT_DBM=20 -DHPVERSION=1 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Zuendbox_v3/%%~nX.c"

avr-gcc -Wl,-Map,Pyro_v3_atmega328p_RFM69HCW_16ch.map -flto -Os -mrelax -mmcu=atmega328p -o "Pyro_v3_atmega328p_RFM69HCW_16ch.elf" %foo3%

avr-size --format=avr --mcu=atmega328p Pyro_v3_atmega328p_RFM69HCW_16ch.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_v3_atmega328p_RFM69HCW_16ch.elf "Pyro_v3_atmega328p_RFM69HCW_16ch.hex"

copy "Pyro_v3_atmega328p_RFM69HCW_16ch.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q "%%a"
echo Done
echo.

REM -------------------------------------------------------------------

echo Compiling v3-sources for ATMega328p and RFM69 (20dBm, 32 Channels)...

for %%X in (..\Firmware_Zuendbox_v3\*.c) do avr-gcc -Wall -Wextra -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -DCOMPILEDATE=%dtstrng% -DCOMPILETIME=%tmstrng% -DMAX_ID=%maxId% -DFIRE_CHANNELS=32UL -DP_OUT_DBM=20 -DHPVERSION=1 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=9830400UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Zuendbox_v3/%%~nX.c"

avr-gcc -Wl,-Map,Pyro_v3_atmega328p_RFM69HCW_32ch.map -flto -Os -mrelax -mmcu=atmega328p -o "Pyro_v3_atmega328p_RFM69HCW_32ch.elf" %foo3%

avr-size --format=avr --mcu=atmega328p Pyro_v3_atmega328p_RFM69HCW_32ch.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_v3_atmega328p_RFM69HCW_32ch.elf "Pyro_v3_atmega328p_RFM69HCW_32ch.hex"

copy "Pyro_v3_atmega328p_RFM69HCW_32ch.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q "%%a"
echo Done
echo.

REM -------------------------------------------------------------------

echo Compiling Minimodul-sources for ATMega328p and RFM69 (13dBm, 4 Channels)...

for %%X in (..\Firmware_Minimodul\*.c) do avr-gcc -Wall -Wextra -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -DCOMPILEDATE=%dtstrng% -DCOMPILETIME=%tmstrng% -DMAX_ID=%maxId% -DFIRE_CHANNELS=4UL -DP_OUT_DBM=13 -DHPVERSION=0 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=11059258UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Minimodul/%%~nX.c"

avr-gcc -Wl,-Map,Pyro_Mini_atmega328p_RFM69CW_4ch.map -flto -Os -mrelax -mmcu=atmega328p -o "Pyro_Mini_atmega328p_RFM69CW_4ch.elf" %fooM%

avr-size --format=avr --mcu=atmega328p Pyro_Mini_atmega328p_RFM69CW_4ch.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_Mini_atmega328p_RFM69CW_4ch.elf "Pyro_Mini_atmega328p_RFM69CW_4ch.hex"

copy "Pyro_Mini_atmega328p_RFM69CW_4ch.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q "%%a"
echo Done
echo.

REM -------------------------------------------------------------------

echo Compiling Minimodul-sources for ATMega328p and RFM69 (20dBm, 4 Channels)...

for %%X in (..\Firmware_Minimodul\*.c) do avr-gcc -Wall -Wextra -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -flto -mcall-prologues -fno-tree-loop-optimize -fno-caller-saves -DRFM=69 -DCOMPILEDATE=%dtstrng% -DCOMPILETIME=%tmstrng% -DMAX_ID=%maxId% -DFIRE_CHANNELS=4UL -DP_OUT_DBM=20 -DHPVERSION=1 -mmcu=atmega328p -DMCU=atmega328p -DF_CPU=11059258UL -MMD -MP -MF"%%~nX.d" -MT"%%~nX.d" -c -o "%%~nX.o" "../Firmware_Minimodul/%%~nX.c"

avr-gcc -Wl,-Map,Pyro_Mini_atmega328p_RFM69HCW_4ch.map -flto -Os -mrelax -mmcu=atmega328p -o "Pyro_Mini_atmega328p_RFM69HCW_4ch.elf" %fooM%

avr-size --format=avr --mcu=atmega328p Pyro_Mini_atmega328p_RFM69HCW_4ch.elf

avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Pyro_Mini_atmega328p_RFM69HCW_4ch.elf "Pyro_Mini_atmega328p_RFM69HCW_4ch.hex"

copy "Pyro_Mini_atmega328p_RFM69HCW_4ch.hex" .\Updater > NUL
for %%a in (*) do if /I not %%~na==%~n0 del /q "%%a"
echo Done
echo.


timeout /T 10  > nul