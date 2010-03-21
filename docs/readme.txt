[00] contents

  [01] features
  [02] installation
  [03] prepare gba cart
  [04] usage
  [05] compile instructions
  [06] credits
  [07] final word


[01] features

 - supports md myth and n64 myth
 - supports .smd and .bin(.md .gen) roms
 - supports .n64 .z64 .v64 and probably whatever n64 roms are there
 - correctly detects type of rom and save type for most roms(some sms roms probably
   left out, report them if you find them).
 - above means you can safely have 16 saves at a time(32 also possible,
   just not now), instead of just 4, like with official programmer
 - includes save management facility
 - roms are written correctly to the flash every time, no "verify failed" messages
   (ok, it doesn't actually verify anything, but it works without it, trust me :) )
 - runs on winxp, vista, win7, and linux. work on 32bit and 64bit machines(there
   might be some problems with 64bit, for windows you'll probably have to compile
   native x64 application from source).
 - according to this http://www.neoflash.com/forum/index.php/topic,5914.msg44602.html#msg44602
   also works on intel mac.
 - actually you can run command line version pretty much everywhere if libusb
   is ported there. gui client supports windows, linux and macos.

known problems:
 - progress indicators do not work properly - ignore them for now,
   it doesn't affect functionality in any way
 - bad blocks ignored for n64 roms - if you have any bad blocks,
   probably you will not be able to complete any n64 game
   this is a hardware limitation, so it won't be fixed soon I believe.
   as a workaround you can write 2 identical roms - when one fails,
   switch to another, or find a cart without bad blocks.
   due to above, you don't have to format the cart if you only
   plan on using n64 myth with it.
 - there is no properties dialog in this release for md roms and
   gba cart - will be added soon.


[02] installation

windows:

download qt dlls http://neo-myth-programmer.googlecode.com/files/neoMythProgrammer%20dlls.7z
download latest binaries http://code.google.com/p/neo-myth-programmer/downloads/list
unpack these two archives into the same directory(command line version does not use qt).
to update, you only need to redownload binaries.
for n64 support, download this http://neo-myth-programmer.googlecode.com/files/plugInfoDB_01.7z
and unpack in the same directory as binaries.

download driver pack http://neo-myth-programmer.googlecode.com/files/neoMythProgrammer%20drivers.7z
now you have 3 options:

libusb filter driver(recommended):
1) get filter driver from here http://libusb-win32.sourceforge.net/#downloads
   be sure to read installation instructions http://libusb-win32.sourceforge.net/#installation
   on vista and win7, you need to set compatibility mode with xp on installer
   executable before driver installation.
this will only work if you have some specific driver for the device already installed,
if you can use official client, that's the case. if not, see below.

libusb driver:
1) unplug cart
2) in device manager, check view->show hidden devices
3) uninstall all slimloader4 drivers
4) right click on NeoMyth_libusb.inf file and choose "install"
5) plug cart, when driver selection pops up, choose libusb driver

neo win2kxp driver:
1) unplug cart
2) in device manager, check view->show hidden devices
3) uninstall all slimloader4 drivers
4) right-click on XGSlim.inf file and choose "install"
5) plug in device, when driver selection dialog pops up, choose "manual method" and point
   it to unpacked SlimLoaderV4 driver, when/if selection with several drivers appears choose
   "NEO2 FlashTeam USB SlimLoader IV" driver(do not make mistake here, as the name of the
   other driver(which wouldn't work here) is similar).

if you want to run this and official client under windows, either use libusb filter driver,
or neo win2kxp driver, for neo win2kxp driver additionally do this:
1) goto "Neo2Usb.dll for official client" directory and run Neo2Usb.bat file.

linux:

download latest binaries http://code.google.com/p/neo-myth-programmer/downloads/list
for n64 support, download this http://neo-myth-programmer.googlecode.com/files/plugInfoDB_01.7z
unpack to ~/.neoMythProgrammer/ directory


1) login to terminal as root
2) install libusb:
   apt-get install libusb
   this is for debian-based distros, replace "apt-get install" with whatever your distro uses.
3) create new group
   groupadd neomyth
4) add yourself to this group
   gpasswd -a <username> neomyth
5) create file /etc/udev/rules.d/neomyth.rules with following content

----------------------------------------------------------------------------
#neomyth device
SUBSYSTEM=="usb", ATTRS{idVendor}=="ffab", ATTRS{idProduct}=="dd03", MODE="0664", GROUP="neomyth"
----------------------------------------------------------------------------

   SUBSYSTEM=="usb" might be SUBSYSTEM=="usb_device", seems to be udev version dependent.

6) logoff-logon for group membership to come into effect, replug cart to reset permissions.

alternatively, you can run app as root every time instead of steps 3-6(this might be useful
if you can't figure out why it doesn't work).


[03] prepare gba cart

you must format the cart before you use it first time with this app(and you only need to
format the cart once, there is no need to reformat after the first time).

md myth:
replace menu with ChillyWilly dx menu http://www.neoflash.com/forum/index.php/topic,6021.0.html
warning: official neo menu (blue and white screen) is not supported. you will get mangled
menu items with this menu.

[04] usage

gui:

you must press burn button to save any changes you made to the cart.
if you don't do this, no changes are saved.

md myth:
for regional protection use gg codes from this page:
http://codehut.gshi.org/RegionalBypassGG.txt
gamefaqs is also a good place to find gg codes.

command line:

run
neocmd --help to see all available options

raw io explained below(I think the rest should be self-explanatory):
neocmd --iotarget menu --read menu.bin
 - read whole menu area to file menu.bin
neocmd --iotarget menu --size 64KB --read menu.bin
 - read first 64KB of menu area to file menu.bin
neocmd --iotarget menu --offset 64KB --size 64KB --read menu.bin
 - read second 64KB of menu area to file menu.bin

default values for offset=0, and size=max(area_size).
available targets: menu, flash, sram, n64menu, n64sram
write operation is the same as read, except that the size is minimum of specified size and filesize
sram reading is unreliable, don't use it as-is to backup saves(save backup function in the app
compensates for this by reading several times and comparing results).


[05] compile instructions

windows:
you need mingw, msys, qt and libusb
mingw: http://www.mingw.org/wiki/MinGW
msys: http://www.mingw.org/wiki/msys
qt: http://qt.nokia.com/products/platform/qt-for-windows
libusb: https://sourceforge.net/projects/libusb-win32/files/ get libusb-win32-device-bin-0.1.12.2.tar.gz

check out source from http://code.google.com/p/neo-myth-programmer/source/checkout
you can use TortoiseSvn to do that http://tortoisesvn.tigris.org/

console app:
install mingw and msys, if you plan to build gui app, install mingw version bundled with qt,
otherwise you will have to recompile qt libraries.
make sure that both mingw/bin and msys/bin directories are in the path variable.
browse to build/console subdirectory, run make

gui app:
install qt, rebuild qt libraries from source if you're using separate mingw installation
make sure that qmake is in the path variable.
browse to build/gui subdirectory, run qmake && make

note: above is for 32bit, for native x64 application you must use vs2008x64 qt pack.

warning: qt build process is broken - this is a problem of qmake, so I'll probably leave it as is.
if you change any headers, source files dependent on those headers will not be recompiled,
therefore you must execute make clean to force recompilation.


linux:
install required packages
apt-get install libusb-dev subversion libqt4-dev qt4-dev-tools
checkout source
svn checkout http://neo-myth-programmer.googlecode.com/svn/trunk/  neo-myth-programmer-read-only
browse to build/console subdirectory, run make
browse to build/gui subdirectory, run qmake && make

warning: qt build process is broken - this is a problem of qmake, so I'll probably leave it as is.
if you change any headers, source files dependent on those headers will not be recompiled,
therefore you must execute make clean to force recompilation.

[06] credits

the following people contributed to the project in one way or another:
Sektor - tests and bug reports
Conle - tests(lots of them), bug reports, n64 plugin
ChillyWilly - hardware info, rom detection stuff and md menu format ripped from his menu
sanni - tests(lots of them), bug reports, n64 support is literary ripped from his batch file :)
elefas - confirmed working on intel macs
Dr.neo - a bit of info here and there
SivenYu - a bit of info here and there
Hatta - bug reports

the following sources were used:
slime e-loader http://www.neoflash.com/forum/index.php/topic,4328.msg29397.html - original idea, also used his makefile
http://gxdev.wordpress.com/category/genesis-plus/ - eeprom/sram detection
http://codehut.gshi.org/RegionalBypassGG.txt - ssf2 master code + regional bypass codes

if I forgot someone, don't hold it against me, just drop me a line an I'll add you to the list.


[07] final word

I have been told multiple times that my instructions suck, the fact that english
is not my native language doesn't help here either. If something is not clear from
above instructions, ask questions in neo forums. If you can write better readme, be my quest :)
send bug reports, suggestions and feature requests to madfkingmonkey@gmail.com or post in neo forums.
