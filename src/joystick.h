// ===============================================================================
// TYPEDEF
// ===============================================================================

// --- Possible joystick positions ---
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE,
    CLICK,
} joystickPos;

// ===============================================================================
// PRIMARY FUNCTION(S)
// ===============================================================================

// --- Joystick logic for first screen ---
joystickPos readJoystick() {
    int xPos = analogRead(VRX_PIN);
    int yPos = analogRead(VRY_PIN);
    int buttonState = digitalRead(SW_PIN);

    static joystickPos lastJoyState = NONE; //where centre is no input
    static unsigned long lastButPress = 0;
    static unsigned long currentButPress;

    //check for a button push
    if(buttonState == LOW && lastJoyState != CLICK) {
        
        currentButPress = millis();
        
        //debounce click
        if(currentButPress - lastButPress > 50) {
            
            //pass current press time to last time press; pass to lastJoyState; tell loop that there was a click
            lastButPress = currentButPress;
            lastJoyState = CLICK;
            return(CLICK);
        }
    }

    //if button isn't held, set state to none
    if (buttonState == HIGH) lastJoyState = NONE;

    //down position
    if(xPos >= 200 && xPos <= 800 && yPos >= 800) return(DOWN);
    
    //up position
    else if(xPos >= 200 && xPos <= 800 && yPos <= 200) return(UP);
    
    //left position
    else if(yPos >= 200 && yPos <= 800 && xPos <= 200) return(LEFT);
    
    //right position
    else if(yPos >= 200 && yPos <= 800 && xPos >= 800) return(RIGHT);
    
    //clear corners
    else return(NONE);
}

joystickPos selColourJoy() {
    joystickPos pos = readJoystick();
    static joystickPos lastPos = NONE;
    unsigned long countdown = 200;
    static unsigned long nextMoveTime;
    static unsigned long holdStart;
    
    // Return all none and click immediately
    if(pos == NONE) {
        lastPos = pos;
        return(pos);
    }

    else if(pos == CLICK) return(pos);

    else if (pos != lastPos) {
        holdStart = millis();
        nextMoveTime = millis();
        lastPos = pos;
        return(pos);
    }

    else if(millis() < nextMoveTime || millis() - holdStart < 500) return(NONE);

    // If holding, repeat outputs every 200ms
    else {
        nextMoveTime = millis() + countdown;
        lastPos = pos;
        return(pos);
    }
}

joystickPos selMassJoy() {
    joystickPos pos = readJoystick();
    static joystickPos lastPos = NONE;
    unsigned long countdown;
    static unsigned long nextMoveTime;
    static unsigned long holdStart;

        // Return all none and click immediately
    if(pos == NONE) {
        lastPos = pos;
        return(pos);
    }

    else if(pos == CLICK) return(pos);

    else if (pos != lastPos) {
        holdStart = millis();
        nextMoveTime = millis();
        lastPos = pos;
        return(pos);
    }

    else if(millis() < nextMoveTime || millis() - holdStart < 500) return(NONE);

    // If holding, repeat outputs every 200ms
    else {
        countdown = (millis() - holdStart > 1100) ? 10 : 150;
        nextMoveTime = millis() + countdown;
        lastPos = pos;
        return(pos);
    }
}