/* NB: pullups need to be set for the inputs to work */
/*
modified TT-input external to use bcm2835 library instead of the deprecated wiringpi library
renamed to cm_input

usage in pd:
------------
cm_input 23


cm input pin legend:
-------------------------------
btn1    gpio23  pin77
btn2    gpio25  pin83
btn3    gpio24  pin81
-
up      gpio22  pin75 -> needs pull UP to be set manually (default pullDown active)
down    gpio6   pin21
left    gpio5   pin17 
right   gpio13  pin47 -> needs pull UP to be set manually (default pullDown active)
-
trig1   gpio4   pin15
trig2   gpio17  pin59
trig3   gpio35  pin54 -> can also be assigned to pin 14(original) but has been moved, since gpio14 has uartTX as alternate function
trig4   gpio27  pin89


compile and linking instructions:
---------------------------------
gcc -std=c99 -O3 -Wall -c cm_input.c -o cm_input.o
ld --export-dynamic -shared -o cm_input.pd_linux cm_input.o -lc -lm -lbcm2835
sudo mv cm_input.pd_linux /usr/local/lib/pd-externals/


raffi - seismic industries - november 2019
*/


#include "m_pd.h"
#include <stdio.h>
#ifdef __arm__
//	#include <wiringPi.h>
	#include <bcm2835.h>
#endif

t_class *cm_input_class;

typedef struct _cm_input
{
	t_object x_obj;
	t_clock *x_clock;
	t_int clkState;
	t_int pinNum;
	t_outlet *x_out;

} t_cm_input;

void cm_input_tick(t_cm_input *x)
{
	int prevState = x->clkState;
	#ifdef __arm__
		x->clkState = bcm2835_gpio_lev(x->pinNum); // read gpio with bcm library
	#endif
	// pin pulled low since last tick ?
	if(prevState && !x->clkState) outlet_bang(x->x_out);
	clock_delay(x->x_clock, 0x1); 
}

void *cm_input_new(t_floatarg _pin)
{
	t_cm_input *x = (t_cm_input *)pd_new(cm_input_class);
	x->x_clock = clock_new(x, (t_method)cm_input_tick);
	// valid pin?
	if (_pin == 4 || _pin == 5 || _pin == 6 || _pin == 13 || _pin == 17 || _pin == 2 || _pin == 3 || _pin == 14 || _pin == 27 || _pin == 22 || _pin == 23 || _pin == 24 || _pin == 25 || _pin == 27 || _pin == 35) x->pinNum = _pin;
	else x->pinNum = 4; // default to pin #4	
	#ifdef __arm__
      bcm2835_gpio_set_pud(x->pinNum, BCM2835_GPIO_PUD_UP);  // set all pins pullUP resistors active
	  bcm2835_gpio_fsel(x->pinNum, BCM2835_GPIO_FSEL_INPT);  // set mode input
	#endif
	x->x_out = outlet_new(&x->x_obj, gensym("bang"));
	cm_input_tick(x);
	return (void *)x;
}

void cm_input_free(t_cm_input *x)
{
	clock_free(x->x_clock);
	outlet_free(x->x_out);  
}

void cm_input_setup()
{
	#ifdef __arm__
  if (bcm2835_init() == -1) {
    printf("bmc2835_init Error\n");
    return;
  }
	#endif	
	cm_input_class = class_new(gensym("cm_input"),
		(t_newmethod)cm_input_new, (t_method)cm_input_free,
		sizeof(t_cm_input), 
		CLASS_NOINLET, 
		A_DEFFLOAT,
		0);
}
