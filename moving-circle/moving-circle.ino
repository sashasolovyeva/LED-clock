/*
 * This example shows how to display bitmaps that are exported from GIMP and
 * compiled into the sketch and stored in the Teensy's Flash memory
 * See more details here:
 * http://docs.pixelmatix.com/SmartMatrix/library.html#smartmatrix-library-how-to-use-the-library-drawing-raw-bitmaps
 *
 * This example uses only the SmartMatrix Background layer
 */

#define USE_ADAFRUIT_GFX_LAYERS

// uncomment one line to select your MatrixHardware configuration - configuration header needs to be included before <SmartMatrix.h>
#include <MatrixHardware_Teensy3_ShieldV4.h>        // SmartLED Shield for Teensy 3 (V4)
#include <SmartMatrix.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Fonts/FreeSerifBoldItalic12pt7b.h>

#include "gimpbitmap.h"

// pixelmatix.c is a bitmap exported from GIMP without modification
#include "modified-boobs.c"


#define COLOR_DEPTH 24                  // Choose the color depth used for storing pixels in the layers: 24 or 48 (24 is good for most sketches - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24)
const uint16_t kMatrixWidth = 32;       // Set to the width of your display, must be a multiple of 8
const uint16_t kMatrixHeight = 32;      // Set to the height of your display
const uint8_t kRefreshDepth = 36;       // Tradeoff of color quality vs refresh rate, max brightness, and RAM usage.  36 is typically good, drop down to 24 if you need to.  On Teensy, multiples of 3, up to 48: 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48.  On ESP32: 24, 36, 48
const uint8_t kDmaBufferRows = 4;       // known working: 2-4, use 2 to save RAM, more to keep from dropping frames and automatically lowering refresh rate.  (This isn't used on ESP32, leave as default)
const uint8_t kPanelType = SM_PANELTYPE_HUB75_32ROW_MOD16SCAN;   // Choose the configuration that matches your panels.  See more details in MatrixCommonHub75.h and the docs: https://github.com/pixelmatix/SmartMatrix/wiki
const uint32_t kMatrixOptions = (SM_HUB75_OPTIONS_NONE);        // see docs for options: https://github.com/pixelmatix/SmartMatrix/wiki
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);
const uint8_t kIndexedLayerOptions = (SM_INDEXED_OPTIONS_NONE);
const uint8_t kScrollingLayerOptions = (SM_SCROLLING_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(indexedLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);
// SMARTMATRIX_ALLOCATE_SCROLLING_LAYER(scrollingLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kScrollingLayerOptions);
SMARTMATRIX_ALLOCATE_GFX_MONO_LAYER(scrollingLayer, kMatrixWidth, kMatrixHeight, 6*1024, 1, COLOR_DEPTH, kScrollingLayerOptions);

// int led = -1;
int x = kMatrixWidth / 2;
int y = kMatrixHeight / 2;

// int textSize = 1;

int r = 5;

int xDir = 1;
int yDir = 1;

void drawBitmap(int16_t x, int16_t y, const gimp32x32bitmap* bitmap) {
  for(unsigned int i=0; i < bitmap->height; i++) {
    for(unsigned int j=0; j < bitmap->width; j++) {
      SM_RGB pixel = { bitmap->pixel_data[(i*bitmap->width + j)*3 + 0],
                      bitmap->pixel_data[(i*bitmap->width + j)*3 + 1],
                      bitmap->pixel_data[(i*bitmap->width + j)*3 + 2] };
      if(COLOR_DEPTH == 48) {
          pixel.red = pixel.red << 8;
          pixel.green = pixel.green << 8;
          pixel.blue = pixel.blue << 8;
      }

      backgroundLayer.drawPixel(x + j, y + i, pixel);
    }
  }
}

void setup() {
  matrix.addLayer(&backgroundLayer);
  matrix.addLayer(&scrollingLayer);
  matrix.addLayer(&indexedLayer); 
  matrix.begin();

  matrix.setBrightness(30);

  scrollingLayer.setMode(wrapForward);
  scrollingLayer.setColor({0xff, 0xff, 0xff});
  scrollingLayer.setSpeed(30);
  scrollingLayer.setOffsetFromTop(6);
  scrollingLayer.setFont(&FreeSerifBoldItalic12pt7b);
  scrollingLayer.start("Send Nudes", -1);


  // if(led >= 0)  pinMode(led, OUTPUT);
}

void loop() {

  // backgroundLayer.setCursor(kMatrixWidth/2/textSize, kMatrixHeight/2/textSize);
  // backgroundLayer.setTextSize(textSize);
  // backgroundLayer.fillScreen({0,0,0});
  int pic1, pic2;
  pic1 = (kMatrixWidth / 2) - 16;
  pic2 = (kMatrixHeight / 2) - 16;
  // to use drawBitmap, must cast the pointer to pixelmatixlogo as (const gimp32x32bitmap*)
  drawBitmap(pic1,pic2,(const gimp32x32bitmap*)&boobs);

  // backgroundLayer.print("hello");
  // backgroundLayer.write(0x2A);
  // backgroundLayer.write(0x03);

  backgroundLayer.fillCircle(x, y, r, {0xff,0xff,0xff});
  backgroundLayer.swapBuffers();
  // backgroundLayer.swapBuffers(false);

  // delay(100);
  // if(textSize < 5){
  //   textSize++;
  // } else {
  //   textSize = 1;
  // }
  

  x+=1*xDir;
  y+=2*yDir;

  delay(100);

  if(x + r >= kMatrixWidth || x - r < 0) {
    xDir *= -1;
  }

  if(y + r >= kMatrixHeight || y - r < 0) {
    yDir *= -1;
  }
  
}
