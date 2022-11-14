#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
#include "pico-ss-oled/include/ss_oled.h"
#include "pico/multicore.h"
#include "shift_register_74hc595.h"
#include "CD74HC4067.h"
#include "ShiftRegister74HC595-Pico/sr_common.h"

#define SPI_PORT    spi0 

#define SDA_PIN     4
#define SCL_PIN     5

#define CLK_595     18
#define DATA_595    19
#define LATCH_595   16

#define OLED_WIDTH  128
#define OLED_HEIGHT 64

ShiftRegister74HC595 sr;

typedef enum RGB { R, G, B};

////////////////////////////////////////////////////////////////////////////////////
// Core 1 interrupt Handler ////////////////////////////////////////////////////////
void core1_interrupt_handler() 
{
    while (multicore_fifo_rvalid())
    {
        uint16_t raw = multicore_fifo_pop_blocking();   
    }
    multicore_fifo_clear_irq(); // Clear interrupt
}

////////////////////////////////////////////////////////////////////////////////////
// Core 1 Main Code ////////////////////////////////////////////////////////////////
void core1_entry() 
{
    // Configure Core 1 Interrupt
    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_interrupt_handler);
    irq_set_enabled(SIO_IRQ_PROC1, true);
    // Infinte While Loop to wait for interrupt
    while (true)
    {
        tight_loop_contents();
    }
}


int main()
{
	stdio_init_all();
	multicore_launch_core1(core1_entry);
    bool clk = set_sys_clock_khz(133000, true);
    shift_register_74HC595_init(&sr, SPI_PORT, DATA_595, CLK_595, LATCH_595);

    gpio_set_outover(DATA_595, GPIO_OVERRIDE_INVERT); 


    while (true) 
	{
        _74HC595_set_all_low(&sr);

        for(int y = 0; y < 4; y++)
        {
            for(int x = 0; x < 4; x++)
            {
                for(int i = 0; i < 16; i++)
                {
                    pset_rgb(&sr, x, y, 200, 1200, 1600);
                }
            }
        }
    }
    return 0;
}