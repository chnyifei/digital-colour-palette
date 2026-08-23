// ===============================================================================
// #INCLUDE ALL OTHER TABS AND SETUP
// ===============================================================================

#include "config.h"
#include "joystick.h"
#include "clayCalc.h"
#include "displayColour.h"
#include "displayUI.h"

void setup() {
  delay(250);
  pinMode(SW_PIN, INPUT_PULLUP);
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(DARKEST_ACCENT);
}

// ===============================================================================
// MAIN LOOP
// ===============================================================================

void loop() {
  bool firstDraw = (currentScreen != previousScreen);
  joystickPos input;

  switch(currentScreen) {
    
    // --- First screen: select colour ---
    case SELECT_COLOUR: {

      input = selColourJoy();
      // --- Draw screen immediately as it's switched to ---
      if(firstDraw) {
        leftIndex = (currentHueIndex == 0) ? 23 : currentHueIndex - 1;
        rightIndex = (currentHueIndex == 23) ? 0 : currentHueIndex + 1;

        leftColour = blendDisplayColour(dHues[leftIndex], currentTintShade);
        selectedColour = blendDisplayColour(dHues[currentHueIndex], currentTintShade);
        rightColour = blendDisplayColour(dHues[rightIndex], currentTintShade);

        printColourSelect(leftColour, selectedColour, rightColour, dHues[currentHueIndex].name, currentTintShade, firstDraw);

        previousScreen = currentScreen;
      }

      // --- Respond to joystick inputs by incrementing tracking vars --
      if(input == LEFT) {
        if(currentHueIndex > 0) currentHueIndex--;
        else currentHueIndex = 23;
        needsRedraw = true;
      }

      else if(input == RIGHT) {
        if(currentHueIndex < 23) currentHueIndex++;
        else currentHueIndex = 0;
        needsRedraw = true;
      }

      else if(input == UP) {
        if(currentTintShade < 3) currentTintShade++;
        else currentTintShade = -3;
        needsRedraw = true;
      }

      else if(input == DOWN) {
        if(currentTintShade > -3) currentTintShade--;
        else currentTintShade = 3;
        needsRedraw = true;
      }

      if(input == CLICK) {
        selectedColour = blendDisplayColour(dHues[currentHueIndex], currentTintShade);
        floodScreen(selectedColour);
        currentScreen = SELECT_MASS;
        return;
      }

      // --- If no joystick input was detected, do not call colour functinos ---
      if(needsRedraw == false) break;

      // --- Blend colours to show on display and update on screen ---
      leftIndex = (currentHueIndex == 0) ? 23 : currentHueIndex - 1;
      rightIndex = (currentHueIndex == 23) ? 0 : currentHueIndex + 1;

      leftColour = blendDisplayColour(dHues[leftIndex], currentTintShade);
      selectedColour = blendDisplayColour(dHues[currentHueIndex], currentTintShade);
      rightColour = blendDisplayColour(dHues[rightIndex], currentTintShade);

      printColourSelect(leftColour, selectedColour, rightColour, dHues[currentHueIndex].name, currentTintShade, firstDraw);
      needsRedraw = false;
    break;
    }

    // --- Second screen: select mass ---
    case SELECT_MASS: {
      input = selMassJoy();
      float placeValue[4] = {0.01f, 0.10f, 1.0f, 10.0f};
      static int placeValIndex = 0;

      // --- Print screen once immediately after switch ---
      if(firstDraw) {
        printGramSelect(currentMass, selectedColour, placeValue[placeValIndex], firstDraw);
        previousScreen = currentScreen;
      }

      // --- Change place value based on left/right joystick inputs ---
      if(input == RIGHT) {
        placeValIndex = (placeValIndex == 0) ? 3 : placeValIndex - 1; 
        needsRedraw = true;
      }
      
      if(input == LEFT) {
        placeValIndex = (placeValIndex == 3) ? 0 : placeValIndex + 1; 
        needsRedraw = true;
      }

      // --- Change current mass based on up/down joystick inputs ---
      else if(input == UP && currentMass <= (float)50.0 - placeValue[placeValIndex]) {
        currentMass += placeValue[placeValIndex];
        needsRedraw = true;
      }

      else if(input == DOWN && currentMass >= placeValue[placeValIndex]) {
        currentMass -= placeValue[placeValIndex];
        needsRedraw = true;
      }

      if(input == CLICK) {
        slideUp(selectedColour);
        currentScreen = RESULTS;
        return;
      }

      // Only call print function if up/down were detected
      if(needsRedraw == false) break;

      printGramSelect(currentMass, selectedColour, placeValue[placeValIndex], firstDraw);
      needsRedraw = false;
    break;
    }
    
    // --- Third screen: results ---
    case RESULTS: {

      input = readJoystick();
      // --- Only print results screen once using firstDraw ---
      if(firstDraw) {
        // --- Based on current colour and current mass, calculate physical ratios and increment through arrays to find values ---
        mixResult result = calculateMix(currentHueIndex, currentTintShade, currentMass);

        const char* colours[3];
        float ratios[3], masses[3];
        int numOfCols = 0;
        int j = 0; // For tracking the array index of colours, ratios, and masses

        // Increment through for info to send to printResults
        for(int i = 0; i < 7; i++) {
          if(result.grams[i] == 0) continue;
          
          colours[j] = clayNames[i];
          ratios[j] = result.ratio[i];
          masses[j] = result.grams[i];

          numOfCols++;
          j++;
        }

        printResults(HUES[currentHueIndex].name, currentMass, colours, ratios, masses, numOfCols, currentTintShade);

        previousScreen = currentScreen;
      }
      
      if(input == CLICK) {
        currentScreen = SELECT_COLOUR;
        return;
      }
    break;
    }
  }
}