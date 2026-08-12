// ===============================================================================
// DEFINES, COLOUR BUFFER, TYPEDEF, AND DATABANK
// ===============================================================================

// --- Accent colours shown on display ---
#define DARK_ACCENT 0x3a09
#define DARKEST_ACCENT 0x2965
#define LIGHT_ACCENT 0x9cd3
#define LIGHTEST_ACCENT 0xb5b6

// --- Bitmap ---
#define TRANSPARENT_KEY 0xF816 
GFXcanvas16 paletteCanvas(paintPalette1__1__width, paintPalette1__1__height);

// Just for reference:
// const int paintPalette1__1__width  = 120;
// const int paintPalette1__1__height = 132;

// --------------------------------------------------------------------------
// PRINT SCREEN HELPER FUNCTIONS: 555 -> 888, DRAWBITMAP, DRAW CENTEREDTEXT
// --------------------------------------------------------------------------

// --- Convert to 888 to analyse brightness in displayColour.h ---

// --- Print palette icon
void drawTransparentBitmapToCanvas(Adafruit_GFX* gfx, int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h) {
    for (int16_t j = 0; j < h; j++) {
        for (int16_t i = 0; i < w; i++) {
            uint16_t color = pgm_read_word(&bitmap[j * w + i]);
            if (color != TRANSPARENT_KEY) { 
                gfx->drawPixel(x + i, y + j, color);
            }
        }
    }
}

// --- Prints centered text according to inputs ---
void drawCenteredText(const char *text, int y, int size, uint16_t textColour, uint16_t bgColour) {

    tft.setTextSize(size);
    tft.setTextColor(textColour, bgColour);

    int16_t x1, y1;
    uint16_t w, h;
    tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    int x = (240 - w)/2;

    tft.fillRect(0, y, 240, size * 8, bgColour); // Prevent overlap in colour code; it usually ignores the outside spaces

    tft.setCursor(x,y);
    tft.print(text);
}

// --- Floods the screen in a circle shape ---
void floodScreen(uint16_t colour) {
    int step = 5;
    unsigned long startTime = millis(); // For testing

    for(int r = 0; r <= 220; r += step) {
        tft.fillCircle(125, 155, r, colour);
        delay(5);
    }

    unsigned long drawTime = millis() - startTime;
    Serial.println(drawTime);
}

// --- Transition between second and third screen ---
void slideUp(uint16_t prevBg) {
    int step = 5;
    tft.fillScreen(prevBg);

    for(int i = 320-step; i > 20; i-=step) {
        tft.fillRect(0, i, 240, step, DARKEST_ACCENT);
        delay(8);
    }
}

// --- Converts float to string with specified width and precision - added for C++14 compatibility; not needed in Arduino IDE---
char* dtostrf(double val, signed char width, unsigned char prec, char* sout) {
    char fmt[20];
    sprintf(fmt, "%%%d.%df", width, prec);
    sprintf(sout, fmt, val);
    return sout;
}

// --------------------------------------------------------------------------
// PRINT SCREEN FUNCTIONS: COLOUR SELECT, GRAM SELECT
// --------------------------------------------------------------------------

// --- Print colour selector screen ---
void printColourSelect(uint16_t leftColour, uint16_t midColour, uint16_t rightColour, const char* colourCode, int tintShade, bool firstDraw) {

    int iconX = 70; // starting x position of palette
    int iconY = 75; // starting y position of palette
    char strBuffer[20];
    int offset = 0; // For adding things onto string

    // Print default text only if it's the first showing
    if(firstDraw) {
        tft.fillScreen(DARKEST_ACCENT);

        // Print screen 1 text
        drawCenteredText("move joystick to navigate", 40, 1, LIGHT_ACCENT, DARKEST_ACCENT);
        drawCenteredText("click to confirm", 275, 1, LIGHTEST_ACCENT, DARKEST_ACCENT);
    }

    // Print the name with the tint/shade modifier
    offset = snprintf(strBuffer, sizeof(strBuffer), "> %s", colourCode);
    if(tintShade !=0) {
        if(tintShade > 0) {
            offset += snprintf(strBuffer + offset, sizeof(strBuffer) - offset, "+");
        }
        offset += snprintf(strBuffer + offset, sizeof(strBuffer) - offset, "%d", tintShade);
    }
    offset += snprintf(strBuffer + offset, sizeof(strBuffer) - offset, " <");
    
    // Send to print new string
    drawCenteredText(strBuffer, 235, 3, LIGHT_ACCENT, DARKEST_ACCENT);
    
    // Draw canvas and rectangles in canvas first
    paletteCanvas.fillScreen(DARKEST_ACCENT);
    paletteCanvas.fillRect(10, 75, 25, 20, leftColour);
    paletteCanvas.fillRect(40, 62, 30, 35, midColour);
    paletteCanvas.fillRect(70, 60, 20, 30, rightColour);
    drawTransparentBitmapToCanvas(&paletteCanvas, 0, 0, paintPalette1__1_, paintPalette1__1__width, paintPalette1__1__height);

    // Send everything to be drawn
    tft.drawRGBBitmap(iconX, iconY, paletteCanvas.getBuffer(), paintPalette1__1__width, paintPalette1__1__height);
}

// --- Print gram selector on second screen ---
void printGramSelect(float currentMass, uint16_t bgColour, float placeVal, bool firstDraw) {

    char strBuffer[40]; // Holds combined floats/chars in one string
    int targetY, startY;

    // --- Identify text colour ---

    // Determine number colour based on the background
    uint8_t r, g, b;
    rgb565to888(bgColour, r, g, b);
    float brightness = sqrt(0.299f * (r * r) + 0.587f * (g * g) + 0.114f * (b * b));

    // 0.0 = black; 255.0 = white
    uint16_t mainTextColour, secondTextColour;
    if(brightness > 130.0f) {mainTextColour = DARKEST_ACCENT; secondTextColour = DARK_ACCENT;} // bright screen; dark text
    else {mainTextColour = LIGHTEST_ACCENT; secondTextColour = LIGHT_ACCENT;} // dark screen; light text

    // --- Print text ---

    // Only print top and bottom text if it hasn't been printed yet
    if(firstDraw) {
      drawCenteredText("up/down:change mass (g)", 30, 1, mainTextColour, bgColour);
      drawCenteredText("click to confirm", 275, 1, mainTextColour, bgColour);
    }

    // Currently selected place value
    snprintf(strBuffer, sizeof(strBuffer), "left/right:change increment (%.2f)", placeVal);
    drawCenteredText(strBuffer, 50, 1, mainTextColour, bgColour);

    // Currently selected mass (always exists)
    snprintf(strBuffer, sizeof(strBuffer), "> %.2f <", currentMass);
    drawCenteredText(strBuffer, 150, 3, mainTextColour, bgColour);

    // Print upper/lower if within bounds (0.00g - 50.00g)
    if(currentMass < 49.995f) { // Print 1 higher number
        float newMass = currentMass + 0.01f;
        dtostrf(newMass, 4, 2, strBuffer);
        drawCenteredText(strBuffer, 110, 2, secondTextColour, bgColour);

        if(currentMass < 49.985f) { // Print 2 higher number
            newMass += 0.01;
            dtostrf(newMass, 4, 2, strBuffer);
            drawCenteredText(strBuffer, 85, 1, secondTextColour, bgColour);
        }
        else tft.fillRect(0, 85, 240, 8, bgColour); 
    }
    else tft.fillRect(0, 85, 240, 41, bgColour); 

    if(currentMass > 0.005f) { // Enter if current mass is safely above 0.00
        float newMass = currentMass - 0.01f;

        // Floating point drift and checking for negatives
        if (newMass < 0.00f && newMass > -0.005f) {
            newMass = 0.00f;
        }
        if (newMass < -0.005f) {
            tft.fillRect(0, 200, 240, 16, bgColour); 
        } 
        
        else {
            dtostrf(newMass, 4, 2, strBuffer);
            drawCenteredText(strBuffer, 200, 2, secondTextColour, bgColour);
        }

        if(currentMass > 0.015f) { 
            newMass -= 0.01f;
            if (newMass < 0.00f && newMass > -0.005f) {
                newMass = 0.00f;
            }
            if (newMass < -0.005f) {
                tft.fillRect(0, 234, 240, 8, bgColour); 
            } 
            else {
                dtostrf(newMass, 4, 2, strBuffer);
                drawCenteredText(strBuffer, 234, 1, secondTextColour, bgColour);
            }
        }
        else tft.fillRect(0, 234, 240, 8, bgColour);
    }
    else {
        // If currentMass is 0.00, completely wipe out the bottom slots
        tft.fillRect(0, 200, 240, 42, bgColour);
    }

}

// --- Print results screen ---
void printResults(const char *colourCode, float targetMass, const char* colours[], float ratios[], float masses[], int numOfCols) {

    char strBuffer[40];

    // Target colour and mass
    snprintf(strBuffer, sizeof(strBuffer), "target colour: %s", colourCode);
    drawCenteredText(strBuffer, 60, 1, LIGHTEST_ACCENT, DARKEST_ACCENT);
    snprintf(strBuffer, sizeof(strBuffer), "target mass: %.2fg", targetMass);
    drawCenteredText(strBuffer, 75, 1, LIGHTEST_ACCENT, DARKEST_ACCENT);

    // Spacers and RESULTS header
    drawCenteredText("----------------------------", 95, 1, LIGHT_ACCENT, DARKEST_ACCENT);
    drawCenteredText("RESULTS", 113, 2, LIGHTEST_ACCENT, DARKEST_ACCENT);
    drawCenteredText("----------------------------", 140, 1, LIGHT_ACCENT, DARKEST_ACCENT);

    // Ratios
    drawCenteredText("> RATIO <", 160, 1, LIGHTEST_ACCENT, DARKEST_ACCENT);
    int offset = 0;
    for(int i = 0; i < numOfCols; i++) {
        const char* suffix = (i == numOfCols - 1) ? "" : "   ";
        offset += snprintf(strBuffer + offset, sizeof(strBuffer) - offset, "%s:%.0fpts%s", colours[i], ratios[i], suffix);
    }
    drawCenteredText(strBuffer, 180, 1, LIGHTEST_ACCENT, DARKEST_ACCENT);

    // Masses
    drawCenteredText("> MASS <", 210, 1, LIGHTEST_ACCENT, DARKEST_ACCENT);
    offset = 0;
    for(int i = 0; i < numOfCols; i++) {
        const char* suffix = (i == numOfCols - 1) ? "" : "   ";
        offset += snprintf(strBuffer + offset, sizeof(strBuffer) - offset, "%s:%.0fpts%s", colours[i], masses[i], suffix);
    }
    drawCenteredText(strBuffer, 230, 1, LIGHTEST_ACCENT, DARKEST_ACCENT);

    // Click to return
    drawCenteredText("click to return", 275, 1, LIGHT_ACCENT, DARKEST_ACCENT);
}