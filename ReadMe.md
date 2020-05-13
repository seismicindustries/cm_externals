externals for mcp3208 ADC + GPIO / COMPUTEMODULE 
===========================================================
externals are or will be converted to use the bcm2835 library, since wiringpi is unfortunately deprecated by now.

## ADC: cm_adc_input - NOT YET CONVERTED - still using the kernel device but should be moved to bcm2835 as well
- wm8731 version (6 channels: outlets 1 - 6)
<img src="https://c1.staticflickr.com/5/4286/35410513881_823463e5e9_o.png" width="50%">

- message < open > opens the SPI device. reads ADC when banged. 
- message < open_adc > opens the SPI device. reads ADC when banged. 

- **NB:** plain `[open(` is for use with the wm8731 version (in which case, we only need to read 6 ADC channels). use `[open adc(` for use with the pcm5102a version (in which case all 8 channels need to be read):
<img src="https://c1.staticflickr.com/5/4232/35410513941_bdf67d3bc3_b.jpg" width="50%">

- the ADC object has **two additional methods**, `[smooth(` and `[deadband(`. if the ADC is jittery, you can use those to smooth over some of that. for instance, `[smooth 4(` will average over four input samples,  `[smooth 8(` over eight, etc (available values are 1x, 2x, 4x, 8x, 16x); `[deadband(` takes values from 0-5. default is: `smooth` = 1x, `deadband` = 0. 


 
## gate outputs: cm_output
inlet: sending < 1 > turns the gate on, sending < 0 > off; the creation arguments gives the pin number, where 
- GPIO_num = 12, 16 (Gate OUT 1 & 2)
- GPIO_num = 26 (LED near Button3)
```
   |   
   |   
   |   
 *---************
 * cm_output 12 *
 ****************
```


## gate/button/nav-button inputs: cm_input
where:
- GPIO_num = 22, 6, 5, 13 (up, down, left, right)
- GPIO_num = 4, 17, 35 (trig 1-3)
- GPIO_num = 23, 24, 25 (btn 1-3) 
outputs bang.
```
 ***************
 * cm_input 17 *
 *---***********
   |
   |
   |
```


## switch inputs (alternative): cm_switch
the external is applicaple to the same inputs as the cm_input one.

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


**compile with:**

`gcc -std=c99 -O3 -Wall -c [name_of_external].c -o [name_of_external].o`
`ld --export-dynamic -shared -o [name_of_external].pd_linux [name_of_external].o -lc -lm -lwiringPi`

then move things into externals folder, eg: 

`sudo mv [name_of_external].pd_linux /usr/lib/pd/extra/`
`sudo mv [name_of_external].pd_linux /usr/local/lib/pd-externals/`
