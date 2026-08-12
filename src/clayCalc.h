// ===============================================================================
// NAMING, TYPEDEF, DATABASE
// ===============================================================================

// --- Clay ID's (base clay colours mapped to array slots) ---
#define CLAY_YELLOW  0
#define CLAY_RED     1
#define CLAY_MAGENTA 2
#define CLAY_BLUE    3
#define CLAY_GREEN   4
#define CLAY_WHITE   5
#define CLAY_BLACK   6

// --- Structure for hue descriptions ---
typedef struct {
  const char* name;     
  int colour_a;          
  int parts_a;          
  int colour_b;          
  int parts_b;          
} hueFormula;

// --- Structure to hold calculation results ---
typedef struct {
  float ratio[7];
  float grams[7]; 
} mixResult;

// --- Base clay hue ratios ---
const hueFormula HUES[24] = {
  // --- Yellow to red ---
  { "Y",    CLAY_YELLOW,   1,  0,             0 }, 
  { "YI",   CLAY_YELLOW,  80,  CLAY_RED,      1 }, 
  { "YII",  CLAY_YELLOW,  20,  CLAY_RED,      1 }, 
  { "YIII", CLAY_YELLOW,   4,  CLAY_RED,      1 }, 
  { "YIV",  CLAY_YELLOW,   2,  CLAY_RED,      3 },

  // --- Red to magenta ---
  { "R",    CLAY_RED,      1,  0,             0 }, 
  { "RI",   CLAY_RED,      3,  CLAY_MAGENTA,  2 }, 
  { "RII",  CLAY_RED,      1,  CLAY_MAGENTA,  4 }, 

  // --- Magenta to blue
  { "M",    CLAY_MAGENTA,  1,  0,             0 }, 
  { "MI",   CLAY_MAGENTA, 20,  CLAY_BLUE,     1 }, 
  { "MII",  CLAY_MAGENTA,  4,  CLAY_BLUE,     1 }, 
  { "MIII", CLAY_MAGENTA,  3,  CLAY_BLUE,     2 }, 
  { "MIV",  CLAY_MAGENTA,  3,  CLAY_BLUE,     7 }, 
  { "MV",   CLAY_MAGENTA,  1,  CLAY_BLUE,     9 }, 

  // --- Blue to green ---
  { "B",    CLAY_BLUE,     1,  0,             0 }, 
  { "BI",   CLAY_BLUE,     9,  CLAY_GREEN,    1 }, 
  { "BII",  CLAY_BLUE,     7,  CLAY_GREEN,    3 }, 
  { "BIII", CLAY_BLUE,     2,  CLAY_GREEN,    3 }, 

  // --- Green to yellow ---
  { "G",    CLAY_GREEN,    1,  0,             0 }, 
  { "GI",   CLAY_GREEN,    9,  CLAY_YELLOW,   1 }, 
  { "GII",  CLAY_GREEN,    2,  CLAY_YELLOW,   3 }, 
  { "GIII", CLAY_GREEN,    1,  CLAY_YELLOW,   9 }, 
  { "GIV",  CLAY_GREEN,    1,  CLAY_YELLOW,  40 }, 
  { "GV",   CLAY_GREEN,    1,  CLAY_YELLOW, 120 } 
};

// ===============================================================================
// HELPER FUNCTION(S)
// ===============================================================================

// --- Euclid's algorithm ---
int findGCD(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// ===============================================================================
// PRIMARY FUNCTION(S)
// ===============================================================================

mixResult calculateMix(int hueIndex, int tintShade, float totalMass) { // Anything else?
  mixResult output = {0};
  hueFormula hue = HUES[hueIndex];
  float totalColourParts = (float)(hue.parts_a + hue.parts_b);
  float hueAmount = totalMass;

  // --- Compute the grams and un-reduced ratios and store in output ---
  if (tintShade == -3) { // 1K + 5 parts colour mix
    float blackFraction = 1.0f / 6.0f;
    output.grams[CLAY_BLACK] = totalMass * blackFraction;
    hueAmount = totalMass - output.grams[CLAY_BLACK];
    
    output.ratio[CLAY_BLACK] = totalColourParts * 1.0f;
    output.ratio[hue.colour_a] = hue.parts_a * 5.0f;
    output.ratio[hue.colour_b] = hue.parts_b * 5.0f;
    }

  else if (tintShade == -2) { // 1K + 20 parts colour mix
    float blackFraction = 1.0f / 21.0f;
    output.grams[CLAY_BLACK] = totalMass * blackFraction;
    hueAmount = totalMass - output.grams[CLAY_BLACK];
    
    output.ratio[CLAY_BLACK] = totalColourParts * 1.0f;
    output.ratio[hue.colour_a] = hue.parts_a * 20.0f;
    output.ratio[hue.colour_b] = hue.parts_b * 20.0f;
    }

  else if (tintShade == -1) { // 1K + 80 parts colour mix
    float blackFraction = 1.0f / 81.0f;
    output.grams[CLAY_BLACK] = totalMass * blackFraction;
    hueAmount = totalMass - output.grams[CLAY_BLACK];
    
    output.ratio[CLAY_BLACK] = totalColourParts * 1.0f;
    output.ratio[hue.colour_a] = hue.parts_a * 80.0f;
    output.ratio[hue.colour_b] = hue.parts_b * 80.0f;
    }

  else if (tintShade == 1) { // 1W + 1 part colour mix
    float whiteFraction = 1.0f / 2.0f;
    output.grams[CLAY_WHITE] = totalMass * whiteFraction;
    hueAmount = totalMass - output.grams[CLAY_WHITE];
    
    output.ratio[CLAY_WHITE] = totalColourParts * 1.0f;
    output.ratio[hue.colour_a] = hue.parts_a * 1.0f;
    output.ratio[hue.colour_b] = hue.parts_b * 1.0f;
  }

  else if (tintShade == 2) { // 4W + 1 part colour mix
    float whiteFraction = 4.0f / 5.0f;
    output.grams[CLAY_WHITE] = totalMass * whiteFraction;
    hueAmount = totalMass - output.grams[CLAY_WHITE];
    
    output.ratio[CLAY_WHITE] = totalColourParts * 4.0f;
    output.ratio[hue.colour_a] = hue.parts_a * 1.0f;
    output.ratio[hue.colour_b] = hue.parts_b * 1.0f;
  }

  else if (tintShade == 3) { // 20W + 1 part colour mix
    float whiteFraction = 20.0f / 21.0f;
    output.grams[CLAY_WHITE] = totalMass * whiteFraction;
    hueAmount = totalMass - output.grams[CLAY_WHITE];
    
    output.ratio[CLAY_WHITE] = totalColourParts * 20.0f;
    output.ratio[hue.colour_a] = hue.parts_a * 1.0f;
    output.ratio[hue.colour_b] = hue.parts_b * 1.0f;
  }

  else { // tintShade == 0
    output.ratio[hue.colour_a] = hue.parts_a * 1.0f;
    output.ratio[hue.colour_b] = hue.parts_b * 1.0f;
    }

  if (totalColourParts > 0.0f) {
    output.grams[hue.colour_a] = hueAmount * ((float)hue.parts_a / totalColourParts);
    if (hue.parts_b > 0) {
      output.grams[hue.colour_b] = hueAmount * ((float)hue.parts_b / totalColourParts);
    }
  }
  
  // --- GCD Reduction ---
  int roundedRatios[7] = {0};
  int overallGCD = 0;

  // Round to int and store
  for (int i = 0; i < 7; i++) {
    roundedRatios[i] = (int)roundf(output.ratio[i]);
  }

  // Find non-zero int
  for (int i = 0; i < 7; i++) {
    if (roundedRatios[i] > 0) {
      overallGCD = roundedRatios[i];
      break;
    }
  }

  // Send to GCD function
  if (overallGCD > 1) {
    for (int i = 0; i < 7; i++) {
      if (roundedRatios[i] > 0) {
          overallGCD = findGCD(overallGCD, roundedRatios[i]);
      }
    }
  }

  // Divide all numbers if there exists common divisor greater than 1
  if (overallGCD > 1) {
    for (int i = 0; i < 7; i++) {
      output.ratio[i] = (float)roundedRatios[i] / overallGCD;
    }
  }

  return(output);
}