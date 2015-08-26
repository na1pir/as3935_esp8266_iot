# Thunderstorm sensor protected iot switch
*Another a bit more complex project written in c with espressif sdk for my diploma thesis, so it is all non working and work in progress...*

Copyright (c) 2015 Uroš Golob <golob.uros@gmail.com> 
 
Work was done with intention to protect electronic devices and so lower e-waste.
It might work or it may not, just don't sue me if your house burns down or if it kills you...

This software and hardware is described in my diploma thesis(work in progress). 
   
Use it for *personal* use as you wish, just keep this notice on it if you do! 
And return changes back free of charge by same restrictions and notice! 

This software uses espressif sdk software, spi library from David Ogilvy(MetalPhreak) and libesphttpd & wifi settins form Jeroen Domburg(sprite_fm). So use and rescpect their respective licencies for their work. 

Development took alot of time and money so remember me with some spare 
bitcoins: 1MaGphnuiMjywjQPKHKF239wzcef4KpNxX if you wish:)

For comercial use you can contact me and we can make some separate licencing MIT deal...

## What it is?
Well this thing uses som crazy trikery from ams... and lovely soc from espressif... 
OK as3935 it not so simple chip that uses antena tunned to 500khz to detect lightning discarges.
It mesures amplitude and uses some crazy statistics to determine distance from face of the thunderstorm.
Well I added a bit of trikery of my own to libesphttpd and used spi driver to talk to as3935 with esp8266.
When you mix everything together add coule of pasives transistor, optocoupler and relay you got a switch that disangages relay when we detect nearby thunderstorm. 

This is my baby and so work in progress and will probably newer be perfect.

Well you can order boards from [DirtyPCB's] (http://dirtypcbs.com/view.php?share=10417&accesskey=882ad84b962361d538d1656b4bbab783)

## bom: 
 * esp-201 or esp12 or esp12e
 * as3935
 * ma5532
 * ams1117 3.3
 * optotransistor
 * npn
 * couple of R, C & D 

## how to:
1. get yourself working esp-open-sdk + newer sdk from espressif and link it to esp-open-sdk/sdk 
2. aditionaly get working sprite_fm libesphttpd and cats/led example so you can check and update any changes... 
3. find spi driver from MetalPhreak and check that example

 * I shuld add everithing as git submodules, but as of now i don't know how...

 * make sure to export your esp-open-sdk/xtensa-lx106-elf/bin path to system path... additionaly export esp-open-sdk/sdk as SDK_BASE, then ESPTOOL... in ubuntu it is best to do it in .profile  

4. from as3935_esp8266_iot folder run

	**make clean all**

5. put esp into boot mode (i have reset and boot(rst and gpio0 respectivly to gnd) buttons soldered into my board) so press reset and boot then lift reset first then boot and run

	**make burn** 
or 
	**sudo make burn** 
if your dialout permision thingy stufs aren't setup properly

6. for debbuging

	**make tty**

7. setup geany for above commnds it will save you lots of time

*todo: fix all issues*

## user interface looks like this: 
![settings web page](https://github.com/na1pir/as3935_esp8266_iot/blob/master/webpage.png)
