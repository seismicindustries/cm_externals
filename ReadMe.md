Externals for the ComputeModule MK1.5 
=====================================
using the MCP3208 as ADC chip and the GPIO's from the raspberryPI Computemodule to interface.
Externals are, or will be converted to use the bcm2835 library, since wiringpi is unfortunately deprecated by now.

## ADC: cm_adc_input - NOT YET CONVERTED - still uses the kernel device but should be moved to bcm2835 as well
```
|-----------/
| open_adc |
|-----------\
 \
   \                |-------------/
     \              | deadband 4 |
       \            |-------------\
         \          |
           \        |   |-----------/     
             \      |   | smooth 8 |
               \    |   |-----------\
                 \  |  /
                   \|/   
                  *---*****************************
                  * cm_adc_input                  *
                  *---*---*---*---*---*---*---*---*
```
- message `[open_adc(` opens the SPI device. reads ADC when banged. 

- the ADC object has **two additional methods**, `[smooth(` and `[deadband(`. if the ADC is jittery, you can use those to smooth over some of that. for instance, `[smooth 4(` will average over four input samples,  `[smooth 8(` over eight, etc (available values are 1x, 2x, 4x, 8x, 16x); `[deadband(` takes values from 0-5. default is: `smooth` = 1x, `deadband` = 0. 



 
## cm_digital_output - gate outputs
inlet: sending < 1 > turns the gate on, sending < 0 > off; the creation arguments gives the pin number, where 

CM MK1.5 output pin legend (CM MK1.5):
- gpio 34 = CLK1 output
- gpio 35 = CLK2 output

```
   |   
   |   
   |   
   |   
 *---************
 * cm_output 12 *
 ****************
```



## cm_digital_input - encoder pins and trigger inputs

CM MK1.5 input pin legend:
- enc1s   gpio0
- enc1a   gpio4
- enc1b   gpio1

- enc2s   gpio16
- enc2a   gpio13
- enc2b   gpio12

- enc3s   gpio30
- enc3a   gpio29
- enc3b   gpio28

- enc4s   gpio33
- enc4a   gpio32
- enc4b   gpio31

- trig1   gpio39
- trig2   gpio38
- trig3   gpio37
- trig4   gpio36

```
 ***************
 * cm_input 39 *
 *---***********
   |
   |
   |
   |
```



## cm_switch - switch inputs (alternative)
the external is applicaple to the same inputs as the cm_digital_input one.

- left outlet: puts out the time the switch is held down for (once it's released, in milliseconds).
- right outlet: push = < 1 > / release = < 0 >.
```
 ****************
 * cm_switch 23 *
 *---********---*
   |          |
   |          |
   |          |
```





====================================================================================


**compile instructions**

`gcc -std=c99 -O3 -Wall -c [name_of_external].c -o [name_of_external].o`
`ld --export-dynamic -shared -o [name_of_external].pd_linux [name_of_external].o -lc -lm -lwiringPi`

then move things into externals folder, eg: 

`sudo mv [name_of_external].pd_linux /usr/lib/pd/extra/`
`sudo mv [name_of_external].pd_linux /usr/local/lib/pd-externals/`
