// ===============================================================================
// INCLUDE LIBRARIES
// ===============================================================================

// --- Display libraries ---
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "paintPaletteIcon.h"
#include <SPI.h>

// --- Joystick and calculator libraries ---
#include <stdio.h> // Will probably need?
#include <string.h>
#include <math.h>

// ===============================================================================
// #DEFINE KEYS AND PINS
// ===============================================================================

// --- Physical connections and tft --- (subject to change)
constexpr uint8_t TFT_CS = 17;
constexpr uint8_t TFT_DC = 16;
constexpr uint8_t TFT_RST = 15;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

constexpr uint8_t VRX_PIN = 26;
constexpr uint8_t VRY_PIN = 27;
constexpr uint8_t SW_PIN = 22;

// RESET GP15
// DC GP 16
// SDI/MOSI GP19
// SCK GP18
// VCC/LED 3V3
// GND GND

// ===============================================================================
// UNIVERSAL NAMES
// ===============================================================================

// --- Base clay hue names ---
const char* clayNames[7] = {"Y", "R", "M", "B", "G", "W", "K"};

// --- Shade and tint step number codes ---
int shadeSteps[7] = {-3, -2, -1, 0, 1, 2, 3};

// ===============================================================================
// STATE TRACKING
// ===============================================================================

enum ScreenState {SELECT_COLOUR, SELECT_MASS, RESULTS};
ScreenState currentScreen = SELECT_COLOUR;
ScreenState previousScreen = RESULTS;

int currentHueIndex = 0;      // index into HUES[]/dHues[]
int currentTintShade = 0;     // -3 to +3
float currentMass = 5.00f;    // starting default, adjustable on screen 3
int leftIndex;
int rightIndex;

bool needsRedraw = false;

uint16_t leftColour;
uint16_t selectedColour;
uint16_t rightColour;