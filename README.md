# as3935_esp8266_iot
Another a bit more complex project written in c with espressif sdk for my diploma thesis, so it is all non working and work in progress...


Uroš Golob <golob.uros@gmail.com> wrote this file.
 
Work was done with intention to protect electronic devices and so lower e-waste.
It might work or it may not, just don't sue me if your house burns down or if it kills you...

This software and hardware is described in my diploma thesis(work in progress). 
   
Use it for personal use as you wish, just keep this notice on it if you do! 
And return changes back free of charge by same restrictions and notice! 

For comercial use you can contact me and we can make a deal...

Development took alot of time and money so remember me with some spare 
bitcoins: 1MaGphnuiMjywjQPKHKF239wzcef4KpNxX if you wish:)

This software uses espressif sdk software, spi library from David Ogilvy(MetalPhreak) and libesphttpd & wifi settins form Jeroen Domburg(sprite_fm). So use and rescpect their respective licencies for their work. 
 
how to:
-get yourself working esp-open-sdk + newer sdk from espressif and link it to esp-open-sdk/sdk 
-aditionaly get working sprite_fm libesphttpd and cats/led example so you can check and update any changes... 
-find spi driver from MetalPhreak and check that example
I shuld add everithing as git submodules, but as of now i don't know how...

make sure to export your esp-open-sdk/xtensa-lx106-elf/bin path to system path... additionaly export esp-open-sdk/sdk as SDK_BASE, then ESPTOOL... in ubuntu it is best to do it in .profile  

from as3935_esp8266_iot folder run

	make clean all

put esp into boot mode (i have reset and boot(rst and gpio0 respectivly to gnd) buttons soldered into my board) so press reset and boot then lift reset first then boot and run

	sudo make burn 

for debbuging

	make tty

-setup geany for above commnds it will save you lots of time

todo: 
- recheck registers there migh be another terrible mistake
- interrupt won't fire and i don't know why...  
maybe becouse it is somewhat ftloating - check for gpio settings

later maybe... 
when that essential things work make some use of flash for storing settings
maybe add client side for sendig lightning events with time  and distance to database in external server
