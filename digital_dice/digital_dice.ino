


/*

    IDEA:
    This sketch resembles a digital die.
    It consists of two buttons and a seven segment display.
    One button is used for the die selection; clicking switches to the next die in the selection.
    By pressing the other one you roll the currently selected die.
    The die - i.e., its maximum number - is resembled by the first two digits of the seven segment display whereas the other two display the current die roll.

    NOTES:
    - This sketch utilizes the 'SevSeg' library.
    - The twelve pins of the seven segment display occupy all digital pins of the Arduino board. Accordingly the button input has to be interpreted as a analog signal.
    - TODO: button debouncing

*/





// imports

#include <SevSeg.h>





// definitions

SevSeg sevseg; //    instantiate a seven segment controller object

const int buttonPinDice = A0;    //
const int buttonPinRoll = A1;
const int dice[] = {2, 4, 6, 8, 10, 12, 20};
const int ndice = sizeof(dice)/sizeof(int);

int buttonDiceSensorval;
int buttonRollSensorval;
int diceIndex = 0;
int buttonDiceVal = dice[diceIndex];
int buttonRollVal = random(1, buttonDiceVal+1);
String digit1 = " ";    // first character of the seven segment display
String digit2 = " ";    // second character of the seven segment display
String digit3 = " ";    // third character of the seven segment display
String digit4 = " ";    // fourth character of the seven segment display
String displaystring = "";    // five-character string that will be displayed on the seven segment display
char displayarray[5];    // array buffer for the five-character string above (character array required by 'sevseg.setChars()')
unsigned long starttime = 0;
int wait = 100;
bool debounced = true;
int debounced_ctr_max = 3;
int debounced_ctr = 1;    // for whatever reason the debouncing mechanism does not work properly, if this value is initially 0... Does anyone have an idea???


void roll_Die (){
    buttonRollVal = random(1, buttonDiceVal+1);
}





void setup()
{
  Serial.begin(9600);
  byte numDigits = 4;   
  byte digitPins[] = {2, 3, 4, 5}; //Digits: 1,2,3,4 <--put one resistor (ex: 220 Ohms, or 330 Ohms, etc, on each digit pin)
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13}; //Segments: A,B,C,D,E,F,G,Period

  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);
  sevseg.setBrightness(30); //Note: 100 brightness simply corresponds to a delay of 2000us after lighting each segment. A brightness of 0 
                            //is a delay of 1us; it doesn't really affect brightness as much as it affects update rate (frequency).
                            //Therefore, for a 4-digit 7-segment + pd, COMMON_ANODE display, the max update rate for a "brightness" of 100 is 1/(2000us*8) = 62.5Hz.
                            //I am choosing a "brightness" of 10 because it increases the max update rate to approx. 1/(200us*8) = 625Hz.
                            //This is preferable, as it decreases aliasing when recording the display with a video camera....I think.
}





void loop() {

    // multiplexing the four digits of the seven segment display: i.e., flashing 
    // only if the current time 'millis()' exceeds the most recently logged value 'starttime' by 'wait' (i.e., every 'wait' milliseconds) the displayed values change
    if (millis() > (starttime +wait)){
        starttime = millis();
        buttonDiceSensorval = analogRead(buttonPinDice);
        buttonRollSensorval = analogRead(buttonPinRoll);

        // debouncing
        if ((buttonDiceSensorval < 512) && (debounced == false)){
            debounced_ctr = debounced_ctr +1;
        }
        if (debounced_ctr == debounced_ctr_max){
            debounced = true;
            debounced_ctr = 0;
        }

        // dice selection
        if ((buttonDiceSensorval > 512) && (debounced == true)){
            diceIndex++;
            diceIndex = diceIndex % ndice;
            buttonDiceVal = dice[diceIndex];
            roll_Die();
            debounced = false;
        }

        // rolling the selected dice
        if ((buttonRollSensorval > 512) && (debounced == true)){
            roll_Die();
            debounced = false;
        }
        
        // determining the die display
        if (buttonDiceVal < 10){
            digit1 = " ";
            digit2 = String(buttonDiceVal).charAt(String(buttonDiceVal).length()-1);
        } else {
            digit1 = String(buttonDiceVal).charAt(String(buttonDiceVal).length()-2);
            digit2 = String(buttonDiceVal).charAt(String(buttonDiceVal).length()-1);
        }

        // determining the die roll display
        if (buttonRollVal < 10){
            digit3 = " ";
            digit4 = String(buttonRollVal).charAt(String(buttonRollVal).length()-1);
        } else {
            digit3 = String(buttonRollVal).charAt(String(buttonRollVal).length()-2);
            digit4 = String(buttonRollVal).charAt(String(buttonRollVal).length()-1);
        }

        // printing the 'displaystring', i.e., die and die roll
        displaystring = digit1 +digit2 +"." +digit3 +digit4;
        displaystring.toCharArray(displayarray, 6);
        sevseg.setChars(displayarray);

        // debugging
        //Serial.println("###############################");
        //Serial.println(debounced_ctr);
        //Serial.println(buttonDiceSensorval);
        //Serial.println(debounced);

    }

    // multiplexing the display segments
    sevseg.refreshDisplay();

}
