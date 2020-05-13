/*
 *  left outlet: print time
 *  right outlet: print 1 when pushed, 0 when released
 *
 */
/*
modded to use bcm2835 library
raffi - seismic industries - november 2019
*/

#include "m_pd.h"
#include <stdio.h>
#ifdef __arm__
	#include <bcm2835.h>
#endif

t_class *cm_switch_class;

typedef struct _cm_switch
{
	t_object x_obj;
	t_clock *x_clock;
	t_int clkState;
	t_int switchState;
	t_int ticks;
	t_int pinNum;
	t_outlet *x_out1;
	t_outlet *x_out2;

} t_cm_switch;

void cm_switch_tick(t_cm_switch *x)
{
	int prevState = x->clkState;
	#ifdef __arm__
      x->clkState = bcm2835_gpio_lev(x->pinNum); // read gpio with bcm library
	#endif
	// pin pulled low since last tick ?
	if(prevState && !x->clkState) {
		x->switchState = 0x1;
		outlet_float(x->x_out2, 0x1);
	}
	// released ? 
	if (!prevState && x->clkState) {
		outlet_float(x->x_out1, x->ticks);
		outlet_float(x->x_out2, 0x0);
		x->switchState = 0x0;
		x->ticks = 0x0;
	}
	// delay 1 msec
	clock_delay(x->x_clock, 0x1);
	// if button is held, count++
	if (x->switchState == 0x1) {
		x->ticks++;
	}
}

void *cm_switch_new(t_floatarg _pin)
{
	t_cm_switch *x = (t_cm_switch *)pd_new(cm_switch_class);
	x->x_clock = clock_new(x, (t_method)cm_switch_tick);
	// valid pin? extended to all possible input pins - also allows to detect PW on trig inputs, and longpresses on the menu buttons
    if (_pin == 36 || _pin == 37 || _pin == 38 || _pin == 39 ) x->pinNum = _pin;    // pins adapted to CM MK1.5 - raffi 2020.05.12
	else x->pinNum = 36; // default to pin 36
	// init 
	x->clkState = 1;
	x->switchState = 0;
	x->ticks = 0;
	#ifdef __arm__
      bcm2835_gpio_set_pud(x->pinNum, BCM2835_GPIO_PUD_UP);  // set all pins pullUP resistors active
      bcm2835_gpio_fsel(x->pinNum, BCM2835_GPIO_FSEL_INPT);  // set mode input

	#endif
	x->x_out1 = outlet_new(&x->x_obj, gensym("float"));
	x->x_out2 = outlet_new(&x->x_obj, gensym("float"));
	cm_switch_tick(x);
	return (void *)x;
}

void cm_switch_free(t_cm_switch *x)
{
	clock_free(x->x_clock);
	outlet_free(x->x_out1);
	outlet_free(x->x_out2); 
}

void cm_switch_setup()
{
	#ifdef __arm__
    if (bcm2835_init() == -1) {
      printf("bmc2835_init Error\n");
      return;
    }
	#endif	
	cm_switch_class = class_new(gensym("cm_switch"),
		(t_newmethod)cm_switch_new, (t_method)cm_switch_free,
		sizeof(t_cm_switch), 
		CLASS_NOINLET, 
		A_DEFFLOAT,
		0);
}
