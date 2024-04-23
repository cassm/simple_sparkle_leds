#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include "palette_jellyfish.h"

#define PIN D1

const int repeat_num = 1;

#define NUM_LEDS 215

#define BRIGHTNESS 255

const int millis_per_palette = 30000;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

uint8_t gamma_table[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

//uint32_t last_sparkle[NUM_LEDS] = {0};
uint16_t sparkle_colour[NUM_LEDS][3] = {{0}};
long unsigned int nextSparkEvent = 0;

float frames_per_led = float(palette_len)/float(NUM_LEDS/3);
float sparkle_brightness[NUM_LEDS] = {0};
float min_brightness = 0.65;
float decay_factor = 0.9;

float lpf = 0.85;

uint8_t actual_colours[NUM_LEDS][3] = {{0}};

bool randomIntervalTimer (int eventInterval, long unsigned int &nextEventMillis) {
  long int currentMillis = millis();

  if (currentMillis > nextEventMillis) {
    nextEventMillis = currentMillis + (rand() % eventInterval);
    return true;
  }

  return false;
}

void setup()
{
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  //Serial.begin(9600);
  randomSeed(613838339);
}

int upscale_factor = 16;

float sin_freq_1 = - 1000;

float sin_factor = 6.28 / NUM_LEDS/4;

void loop()
{
    if (randomIntervalTimer(100, nextSparkEvent))
    {
      int num_sparks = rand() % 7;
      for (int i = 0; i < num_sparks; i++) {
        int pixel_index = rand() % (NUM_LEDS);
        sparkle_brightness[pixel_index] = 1.5;
      }
    }

    int time_offset = millis() / 30;
    for (int i = 0; i < NUM_LEDS; i++)
    {
      uint8_t pixel_val[3] = {0};
      int space_offset = float(i) * frames_per_led; 
      int palette_offset = sparkle_brightness[i] * frames_per_led * 20;
      int colour_index = (space_offset + time_offset + palette_offset) % (palette_len-1);

      float sine_brightness_factor = (sin(millis() / sin_freq_1 + i*sin_factor) * 0.175) + 1;

      //float total_brightness = min(1.0f, max(min_brightness, sparkle_brightness[i] * sine_brightness_factor));
      float total_brightness = min(1.0f, max(min_brightness, sine_brightness_factor));


      

      for (int j = 0; j < 3; j++)
      {
          pixel_val[j] = gamma_table[(int)(min(255, int(palette[colour_index][j] * max(min_brightness, sparkle_brightness[i]) * sine_brightness_factor)))];
          actual_colours[i][j] = actual_colours[i][j] * lpf + pixel_val[j] * (1.0 - lpf);
      }

      

      strip.setPixelColor(i, strip.Color(actual_colours[i][1], actual_colours[i][0], actual_colours[i][2]));
      if (sparkle_brightness[i] > min_brightness) {
         sparkle_brightness[i] = sparkle_brightness[i] - 0.02;
        
      }
    }
    strip.show();
    delay(1000/60);
}
