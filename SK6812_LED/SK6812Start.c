// gcc SK6812Start.c -o SK6812Start -lws2811 -lm

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "ws2811.h"

// LED configuration
#define LED_COUNT 144        // Number of LEDs
#define LED_PIN 18           // GPIO pin connected to the pixels (must support PWM)
#define LED_FREQ_HZ 800000   // LED signal frequency in hertz (usually 800khz)
#define LED_DMA 10           // DMA channel to use for generating signal (try 10)
#define LED_BRIGHTNESS 255   // Set to 0 for darkest and 255 for brightest
#define LED_CHANNEL 0

// Create a ws2811_t structure to hold the LED strip state
ws2811_t ledstring = {
    .freq = LED_FREQ_HZ,
    .dmanum = LED_DMA,
    .channel = {
        [0] = {
            .gpionum = LED_PIN,
            .count = LED_COUNT,
            .invert = 0,
            .brightness = LED_BRIGHTNESS,
        },
    },
};

void setBrightness(uint32_t color, int brightness) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    for (int i = 0; i < LED_COUNT; i++) {
        ledstring.channel[0].leds[i] = ((r * brightness / 255) << 16) |
                                       ((g * brightness / 255) << 8) |
                                       (b * brightness / 255);
    }
    ws2811_render(&ledstring);
}

void fadeInOut(uint32_t color, int wait) {
    for (int brightness = 0; brightness <= 255; brightness += 5) {
        setBrightness(color, brightness);
        usleep(wait * 1000);
    }
    for (int brightness = 255; brightness >= 0; brightness -= 5) {
        setBrightness(color, brightness);
        usleep(wait * 1000);
    }
}

void signalHandler(int signum) {
    // Clear the LED strip on exit
    for (int i = 0; i < LED_COUNT; i++) {
        ledstring.channel[0].leds[i] = 0;
    }
    ws2811_render(&ledstring);
    ws2811_fini(&ledstring);
    exit(signum);
}

int main() 
{
    // Initialize the LED strip
    if (ws2811_init(&ledstring)) {
        fprintf(stderr, "ws2811_init failed\n");
        return -1;
    }

    // Set up signal handling for clean exit
    signal(SIGINT, signalHandler);

    // Main loop
    while (1) {
        fadeInOut(0xFFFFFF, 10); // Fade in and out with white color
    }

    // Clean up and exit (this won't be reached in this example)
    ws2811_fini(&ledstring);
    return 0;
}