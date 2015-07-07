# as3935_esp8266_iot
Another a bit more complex project written in c with espressif sdk for my diploma thesis, so it is all non working and work in progress...

Uroš Golob <golob.uros@gmail.com> wrote this file.
 
Work was done with intention to protect electronic devices and so lower e-waste.
It might work or it may not, just don't sue me if your house burns down or if it kills you...

This software and hardware is described in my diploma thesis (work in progress). 

Use it for personal use as you will, just keep this notice on it if you do! 
And return changes back free of charge by same restrictions and notice! 
 
For comercial use you can contact me and we can make a deal...
 
Development took alot of time and money so remember me with some spare 
bitcoins: 1MaGphnuiMjywjQPKHKF239wzcef4KpNxX if you wish:)

This software uses espressif sdk software, spi library from David Ogilvy(MetalPhreak) and 
libesphttpd & wifi settins form Jeroen Domburg(sprite_fm), use and rescpect their respective licencies for their work. 

how to:
get yourself working esp-open-sdk + newer sdk from expressif linked to sdk/ 
aditionaly get working libesphttpd and cats/led example so you can check for changes and 
find spi driver from MetalPhreak and check that example

make clean all
sudo make burn
make tty

todo: 
-spi is strange in 90% of time so it doesn't work, ... get that fixed...
-interrupt won't fire and i dont know why...

later maybe... 
when that essential things work make some use of flash for storing settings
maybe add client side for sendig lightning events with time  and distance to database in external server
