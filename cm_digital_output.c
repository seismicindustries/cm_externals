/*
modified TT-input external to use bcm2835 library instead of deprecated wiringpi
renamed everything from terminaltedium to cm
renamed again to cm_digital

usage in PD, for example:
-------------------------
cm_digital_output 34


pin legend (CM MK1.5):
----------------------
gpio 34 = CLK1 output
gpio 35 = CLK2 output



compile and linking instructions:
---------------------------------
gcc -std=c99 -O3 -Wall -c cm_digital_output.c -o cm_digital_output.o
ld --export-dynamic -shared -o cm_digital_output.pd_linux cm_digital_output.o -lc -lm -lbcm2835
sudo mv cm_digital_output.pd_linux /usr/local/lib/pd-externals/



raffi - seismic industries - november 2019
*/


#include "m_pd.h"
#include <stdio.h>
#ifdef __arm__
	#include <bcm2835.h>
#endif

t_class *cm_digital_output_class;

typedef struct _cm_digital_output
{
	t_object x_obj;
	t_int clkState;
	t_int pinNum;

} t_cm_digital_output;

void cm_digital_output_gate(t_cm_digital_output *x, t_floatarg _gate)
{
	if (_gate > 0)	x->clkState = 1;
	else		x->clkState = 0;
	#ifdef __arm__
           bcm2835_gpio_write(x->pinNum, x->clkState);
   	#endif
}

void *cm_digital_output_new(t_floatarg _pin)
{
	t_cm_digital_output *x = (t_cm_digital_output *)pd_new(cm_digital_output_class);

	// valid pin?
    // gpio 16 = CLK1 output
    // gpio 12 = CLK2 output
    // gpio 26 = LED button3 output
	if (_pin == 12 || _pin == 16 || _pin == 26) x->pinNum = _pin;
	else x->pinNum = 16; // default to pin #16
	#ifdef __arm__
        bcm2835_gpio_set_pud(x->pinNum, BCM2835_GPIO_PUD_UP);   // set pull up active
        bcm2835_gpio_fsel(x->pinNum, BCM2835_GPIO_FSEL_OUTP);   // set pin as output
    #endif
	x->clkState = 0;
	return (void *)x;
}



void cm_digital_output_setup()
{
	#ifdef __arm__
	  if (bcm2835_init() == -1) {                               // new setup call for bcm2835 initializing
        printf("bmc2835_init Error\n");
        return;
      }
    #endif
	cm_digital_output_class = class_new(gensym("cm_digital_output"),
		(t_newmethod)cm_digital_output_new,
		0, sizeof(t_cm_digital_output),
		CLASS_DEFAULT,
		A_DEFFLOAT,
		0);
	class_addfloat(cm_digital_output_class, (t_method)cm_digital_output_gate);
}
