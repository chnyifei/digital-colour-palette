// ===============================================================================
// DISPLAY COLOUR DATABANK
// ===============================================================================

// --- Structure for display mixed hue ratios ---
typedef struct {
  const char* name;
  float ratio[5];
} displayHues;

// --- Base hues on display ---
uint16_t basehues565[10] = {
  0xFEA3,  // CLAY_YELLOW
  0xF800,  // CLAY_RED
  0xF010,  // CLAY_MAGENTA
  0x64B7,  // CLAY_BLUE
  0x05EF,  // CLAY_GREEN
  0xFFF7,  // CLAY_WHITE
  0x01E4,  // CLAY_BLACK
  0xA5D4,  // DESAT -1
  0x7C6F,  // DESAT -2
  0x4B4A   // DESAT -3
};

// --- Display mixed hue ratios ---
const displayHues dHues[24] = {
  {"Y",    {1,   0, 0, 0, 0}},
  {"YI",   {80,  2, 0, 0, 0}},
  {"YII",  {20,  1, 0, 0, 0}},
  {"YIII", {6,   1, 0, 0, 0}},
  {"YIV",  {6,   4, 0, 0, 0}},
  {"R",    {0,   1, 0, 0, 0}},
  {"RI",   {0,   3, 2, 0, 0}},
  {"RII",  {0,   2, 4, 0, 0}},
  {"M",    {0,   0, 1, 0, 0}},
  {"MI",   {0,   0, 5, 4, 0}},
  {"MII",  {0,   0, 3, 7, 0}},
  {"MIII", {0,   0, 2, 8, 0}},
  {"MIV",  {0,   0, 1, 15, 0}},
  {"MV",   {0,   0, 1, 22, 0}},
  {"B",    {0,   0, 0, 1, 0}},
  {"BI",   {0,   0, 0, 9, 2}},
  {"BII",  {0,   0, 0, 8, 5}},
  {"BIII", {0,   0, 0, 2, 3}},
  {"G",    {0,   0, 0, 0, 1}},
  {"GI",   {3,   0, 0, 0, 2}},
  {"GII",  {8,   0, 0, 0, 1}},
  {"GIII", {15,  0, 0, 0, 1}},
  {"GIV",  {50,  0, 0, 0, 1}},
  {"GV",   {105, 0, 0, 0, 1}}
};

// --- Pigment strengths (tentative on black and white) ---
float pigmentStrength[5] = {0.30f, 0.75f, 1.25f, 0.50f, 1.50f};

// ===============================================================================
// HELPER FUNCTION(S)
// ===============================================================================

// --- 565 to 888 ---
void rgb565to888(uint16_t colour565, uint8_t &r, uint8_t &g, uint8_t &b) {
    r = (colour565 >> 11) & 0x1F;
    g = (colour565 >> 5)  & 0x3F;
    b = (colour565 >> 0)  & 0x1F;
    r = (r << 3) | (r >> 2);
    g = (g << 2) | (g >> 4);
    b = (b << 3) | (b >> 2);
}

// --- 888 to 565 ---
uint16_t rgb888to565(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t r5 = (r >> 3) & 0x1F;
    uint16_t g6 = (g >> 2) & 0x3F;
    uint16_t b5 = (b >> 3) & 0x1F;
    return (r5 << 11) | (g6 << 5) | b5;
}

// ===============================================================================
// PRIMARY FUNCTION(S)
// ===============================================================================

// --- Blend hues, then blend black and white if necessary ---
uint16_t blendDisplayColour(displayHues ratios, int tintShade) {
  
  float rAcc = 0, gAcc = 0, bAcc = 0, totalParts = 0;
  
  // Go through the hues, break into 888 RGB, add up
  for(int i = 0; i < 5; i++) {
      
    if(ratios.ratio[i] <= 0) continue;
    uint8_t r, g, b;
    
    // Send the base clay hue to unpack to 888; replace values in rgb vars
    rgb565to888(basehues565[i], r, g, b);
    rAcc += (r * ratios.ratio[i] * pigmentStrength[i]);
    gAcc += (g * ratios.ratio[i] * pigmentStrength[i]);
    bAcc += (b * ratios.ratio[i] * pigmentStrength[i]);
    totalParts += ratios.ratio[i] * pigmentStrength[i];
  }
  
  float hueTotalParts = totalParts; // capture BEFORE black/white touch totalParts

  // Check for error -- shouldn't happen here
  if (totalParts == 0) {
    return 0xB0CF; // error colour
  }

  // Check for black/white in the target colour and calculate accordingly
  if (tintShade != 0) {

    float tintShadeStr;
    uint8_t r, g, b;

    if (tintShade < 0) {

      float desatStr;

      if (tintShade == -3) { tintShadeStr = 0.65; desatStr = 1.3; rgb565to888(basehues565[9], r, g, b); }
      if (tintShade == -2) { tintShadeStr = 0.5; desatStr = 0.9; rgb565to888(basehues565[8], r, g, b); }
      if (tintShade == -1) { tintShadeStr = 0.25; desatStr = 0.4; rgb565to888(basehues565[7], r, g, b); }

      // Desat - already scaled from last round, unchanged
      rAcc += (r * desatStr * hueTotalParts);
      gAcc += (g * desatStr * hueTotalParts);
      bAcc += (b * desatStr * hueTotalParts);
      totalParts += desatStr * hueTotalParts;

      // Black - NOW scaled the same way desat already was
      rgb565to888(basehues565[6], r, g, b);
      rAcc += (r * tintShadeStr * hueTotalParts);
      gAcc += (g * tintShadeStr * hueTotalParts);
      bAcc += (b * tintShadeStr * hueTotalParts);
      totalParts += tintShadeStr * hueTotalParts;
    }

    else {
      if (tintShade == 1) {tintShadeStr = 0.5;}
      if (tintShade == 2) {tintShadeStr = 1.3;}
      if (tintShade == 3) {tintShadeStr = 3.2;}

      // White - NOW scaled the same way
      rgb565to888(basehues565[5], r, g, b);
      rAcc += (r * tintShadeStr * hueTotalParts);
      gAcc += (g * tintShadeStr * hueTotalParts);
      bAcc += (b * tintShadeStr * hueTotalParts);
      totalParts += tintShadeStr * hueTotalParts;
    }
  }
  
  // Average values
  float rResult = roundf(rAcc / totalParts);
  float gResult = roundf(gAcc / totalParts);
  float bResult = roundf(bAcc / totalParts);
  
  // Cap at 255 and 0
  rResult = (rResult < 0 ? 0 : rResult > 255 ? 255 : rResult);
  gResult = (gResult < 0 ? 0 : gResult > 255 ? 255 : gResult);
  bResult = (bResult < 0 ? 0 : bResult > 255 ? 255 : bResult);
  
  // Pack to 565 and return
  uint8_t rFinal = (uint8_t)rResult;
  uint8_t gFinal = (uint8_t)gResult;
  uint8_t bFinal = (uint8_t)bResult;
  return rgb888to565(rFinal, gFinal, bFinal);
}