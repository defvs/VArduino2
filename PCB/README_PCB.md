#PCB Instructions : How to build your own VArduino !
##BOM (Bill of material)
* Arduino with an ATMega328P (or standalone ATmega)
* BMP085/180 Pressure sensor, I2C compatible (breakout or standalone)
* (optional) A SSD1306 with screen size > 128x32 (this is optimal, more could be working BUT there is a change SRAM isn't enough for bigger screen)
* (optional) A two-pin PASSIVE piezo buzzer/speaker (optionally, an amplifier, like L9110)
* (optional) A bluetooth slave serial module, like the HC-05/HC-06 (breakout or standalone)

##PCB types
There are two types of VArduino PCBs : VArduino2-CS and VArduino2-NS
The first one is the "compact" version : It is way smaller, and running off of lower voltage, but soldering it is really hard.
It uses standalone chips to get more compactness, like using an ATMega328P and a BMP180 out of their breakout boards. Building this requires proper tools & SMD soldering experience !
The second one uses breakout boards, and through-holes soldering for an easy soldering process. Costs a bit more and consumes more power

##3D Printing the case
If you own a 3D Printer, or finds a way to print, I am giving a 3D model for a box for the VArduino2-CS (In progress)
As the BMP lineup series are UV-Sensitive, it is impossible to carry the PCB naked, as it will cause unusable behaviour. Build yourself a box !

##Recommended PCB manufacturers & components stores
I personally am using EasyEDA as my PCB editor, and I was really surprised of the quality of their PCB-baking service. It is 2$ for 10 PCBs (post costs not included) and the PCBs are insanely good quality !
For the components, if you are looking for breakout boards, look forward to buying chinese boards (from Banggood for example, I wasn't disappointed by them).
If you are more into small chips for building the CS version, I couldn't think more of another EasyEDA service, which is LSCS.com ; You can combine both your PCBs and your components
into one package, saving you post costs, BUT the components are original ones, and are cheap ! Check out them (no links here, so no one will think about affiliate links)

##Creating your own PCB / Building VArduino without a PCB
I am personnaly not a fan of building without a PCB, soldering cables is hard and the result isn't that good. But you may try it, and I wish you good luck.
You can create your own PCB on EasyEDA, I will be leaving a link to the BOM and footprints of all the chips & boards I am using (todo)
I really appreciate people sending me pictures of their work, so do not be afraid of sending me some ;) (maybe I'll post them somewhere)

#I wish you good luck on building your own cheap, compact and precise VArduino2!