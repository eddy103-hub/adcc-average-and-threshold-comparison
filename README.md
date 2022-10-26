# adcc-average-and-threshold-comparison
* The ADCC peripheral is used to read a value from an analogue pin connected to a potentiometer. 
* The ADCC average and threshold comparison is used.
# Software Used
 * [MPLAB® X IDE v6.00](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide) or newer
 * [MPLAB® XC8 v2.36](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers/downloads-documentation#XC8) or newer
 * Microchip PIC18F-Q Series Device Support (1.12.193) or newer (https://packs.download.microchip.com/)
 * [Tera Term](https://www.heise.de/download/product/tera-term-51776) or any terminal emulator
 
 # Hardware Used
* [PIC18F57Q43 Curiosity Nano Evaluation Kit](https://www.microchip.com/en-us/development-tool/DM164150)
* [Curiosity Nano Base for Click boards](https://www.microchip.com/en-us/development-tool/AC164162)
* Potentiometer
### Pin Connection Table

| Description | Direction| Microcontroller Pin|
| ------------- | ------------- |-------------         
| Analog Input  | IN | RA0
# Demo
After the application is flashed to the PIC18F57Q43 Curiosity Nano,An average of 8 ADC readings is compared to 2 V as a setpoint with a threshold of +-1 V ( an out-of-threshold error) in case the threshold condition is met, the threshold interrupt flag ADTIF is set.


![image](https://user-images.githubusercontent.com/66494140/198106908-47449c7c-3bdf-4e5f-9995-a538fd8ffb6d.png)
