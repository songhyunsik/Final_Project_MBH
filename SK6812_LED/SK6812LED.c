// gcc -o LED SK6812LED.c -lws2811 -lm

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <ws2811.h>

#define LED_COUNT 30
#define LED_PIN 18
#define LED_FREQ_HZ 800000
#define LED_DMA 10
#define LED_BRIGHTNESS 255
#define LED_CHANNEL 0

ws2811_t ledstring = {
    .freq = LED_FREQ_HZ,
    .dmanum = LED_DMA,
    .channel = {
        [LED_CHANNEL] = {
            .gpionum = LED_PIN,
            .count = LED_COUNT,
            .invert = 0,
            .brightness = LED_BRIGHTNESS,
            .strip_type = WS2811_STRIP_GRB,
        },
    },
};

void colorWipe(ws2811_t *ledstring, uint32_t color, int wait_ms) {
    for (int i = 0; i < ledstring->channel[LED_CHANNEL].count; i++) {
        ledstring->channel[LED_CHANNEL].leds[i] = color;
        ws2811_render(ledstring);
        usleep(wait_ms * 1000);
    }
}

void signalHandler(int signum) {
    colorWipe(&ledstring, 0x000000, 10);
    ws2811_fini(&ledstring);
    printf("Received signal %d, exiting...\n", signum);
    exit(0);
}

int main() {
    if (ws2811_init(&ledstring) != WS2811_SUCCESS) {
        fprintf(stderr, "ws2811_init failed\n");
        return 1;
    }

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    while (1) {
        colorWipe(&ledstring, 0xFF0000, 50); // Red
        sleep(1);
        colorWipe(&ledstring, 0x00FF00, 50); // Green
        sleep(1);
        colorWipe(&ledstring, 0x0000FF, 50); // Blue
        sleep(1);
    }

    ws2811_fini(&ledstring);
    return 0;
}