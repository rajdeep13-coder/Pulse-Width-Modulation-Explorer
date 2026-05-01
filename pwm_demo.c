/*
 * ATmega328P PWM Demo - LED Fade & Servo Control
 * See PWM_DOCUMENTATION.txt for detailed technical reference
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

/* TIMER0: LED Fade (8-bit, OC0A on PD6, 976 Hz PWM) */

void timer0_init(void) {
    DDRD |= (1 << PD6);
    TCCR0A = (1 << COM0A1) | (0 << COM0A0) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (0 << FOC0A) | (0 << FOC0B) | (0 << WGM02) | (0 << CS02) | (1 << CS01) | (1 << CS00);
    OCR0A = 0;
}

void led_fade_demo(void) {
    uint8_t brightness = 0;
    int8_t direction = 1;
    
    while (1) {
        OCR0A = brightness;
        brightness += direction;
        
        if (brightness == 255) {
            direction = -1;
        } else if (brightness == 0) {
            direction = 1;
            // No break – continue forever
        }
        _delay_ms(8);
    }
}

/* TIMER1: Servo Control (16-bit, OC1A on PB1, 50 Hz PWM) */

void timer1_init(void) {
    DDRB |= (1 << PB1);
    TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << WGM11) | (0 << WGM10);
    TCCR1B = (0 << ICNC1) | (0 << ICES1) | (1 << WGM13) | (1 << WGM12) | (1 << CS12) | (0 << CS11) | (0 << CS10);
    ICR1 = 1249;
    OCR1A = 62;
}

void servo_sweep_demo(void) {
    uint16_t pulse_width = 62;
    int8_t direction = 1;
    
    while (1) {
        OCR1A = pulse_width;
        pulse_width += direction;
        
        if (pulse_width >= 125) {
            direction = -1;
        } else if (pulse_width <= 62) {
            direction = 1;
            // No break – continue forever
        }
        _delay_ms(30);
    }
}

/* ============================================================================
 * MAIN PROGRAM
 * ============================================================================
 * Select which demo to run via preprocessor symbol PWM_MODE:
 *   0 = LED fade only
 *   1 = Servo sweep only
 *   2 = Both (sequential)
 */

int main(void) {
    #ifndef PWM_MODE
    #define PWM_MODE 2
    #endif
    
    #if (PWM_MODE == 0) || (PWM_MODE == 2)
    timer0_init();
    led_fade_demo();
    #endif
    
    #if (PWM_MODE == 1) || (PWM_MODE == 2)
    timer1_init();
    servo_sweep_demo();
    #endif
    
    while (1) {
        _delay_ms(1000);
    }
    
    return 0;
}
