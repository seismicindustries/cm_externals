/* NB: pullups need to be set for the inputs to work */
/*
modified TT-input external to use bcm2835 library instead of the deprecated wiringpi library
renamed to cm_input

usage in pd:
------------
cm_digital_input 13

 ***************
 * cm_digital_input 13 *
 *---***********
   |
   |
   |



CM MK1.5 input pin legend:
-------------------------------
enc1s	gpio0
enc1a 	gpio4
enc1b 	gpio1

enc2s	gpio16
enc2a	gpio13
enc2b	gpio12

enc3s	gpio30
enc3a	gpio29
enc3b	gpio28

enc4s	gpio33
enc4a	gpio32
enc4b	gpio31

trig1	gpio39
trig2	gpio38
trig3	gpio37
trig4	gpio36



compile and linking instructions:
---------------------------------
gcc -std=c99 -O3 -Wall -c cm_digital_input.c -o cm_digtial_input.o
ld --export-dynamic -shared -o cm_digital_input.pd_linux cm_digital_input.o -lc -lm -lbcm2835
sudo mv cm_digital_input.pd_linux /usr/local/lib/pd-externals/


raffi - seismic industries - november 2019
*/


#include "m_pd.h"
#include <stdio.h>
#ifdef __arm__
//	#include <wiringPi.h>
	#include <bcm2835.h>
#endif

t_class *cm_digital_input_class;

typedef struct _cm_digital_input
{
	t_object x_obj;
	t_clock *x_clock;
	t_int clkState;
	t_int pinNum;
	t_outlet *x_out;

} t_cm_digital_input;

void cm_digital_input_tick(t_cm_digital_input *x)
{
	int prevState = x->clkState;
	#ifdef __arm__
		x->clkState = bcm2835_gpio_lev(x->pinNum); // read gpio with bcm library
	#endif
	// pin pulled low since last tick ?
	if(prevState && !x->clkState) outlet_bang(x->x_out);
	clock_delay(x->x_clock, 0x1);
}

void *cm_digital_input_new(t_floatarg _pin)
{
	t_cm_digital_input *x = (t_cm_digital_input *)pd_new(cm_digital_input_class);
	x->x_clock = clock_new(x, (t_method)cm_digital_input_tick);
	// valid pin?
	if (_pin == 0 || _pin == 4 || _pin == 1 || _pin == 16 || _pin == 13 || _pin == 12 || _pin == 30 || _pin == 29 || _pin == 28 || _pin == 33 || _pin == 32 || _pin == 31 || _pin == 39 || _pin ==38 || _pin == 37 || _pin == 36) x->pinNum = _pin;
	else x->pinNum = 0; // default to pin #0
	#ifdef __arm__
		bcm2835_gpio_set_pud(x->pinNum, BCM2835_GPIO_PUD_UP);  // set all pins pullUP resistors active
		bcm2835_gpio_fsel(x->pinNum, BCM2835_GPIO_FSEL_INPT);  // set mode input
	#endif
	x->x_out = outlet_new(&x->x_obj, gensym("bang"));
	cm_digital_input_tick(x);
	return (void *)x;
}

void cm_digital_input_free(t_cm_digital_input *x)
{
	clock_free(x->x_clock);
	outlet_free(x->x_out);
}

void cm_digital_input_setup()
{
	#ifdef __arm__
  if (bcm2835_init() == -1) {
    printf("bmc2835_init Error\n");
    return;
  }
	#endif
	cm_digital_input_class = class_new(gensym("cm_digital_input"),
		(t_newmethod)cm_digital_input_new, (t_method)cm_digital_input_free,
		sizeof(t_cm_digital_input),
		CLASS_NOINLET,
		A_DEFFLOAT,
		0);
}
