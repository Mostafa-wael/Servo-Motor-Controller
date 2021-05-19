/**
 * @author Mostafa Wael
 * @details 
 * reading the required position form DIOs
 * mapping the values to the required ones
 * checking for tne invlaid inputs
 * in this sketch I'm uisng a more optimized servo library made by me 
 */
#include "MWservo.h"
//********************************
#define BCDSize 4
#define BCD0 2
#define BCD1 3
#define BCD2 4
#define BCD3 5
#define SERVO_PIN 6
MWservo MW; // create servo object to control a servo
void setup()
{
    Serial.begin(9600);
    //********************************
    pinMode(BCD0, INPUT);
    pinMode(BCD1, INPUT);
    pinMode(BCD2, INPUT);
    pinMode(BCD3, INPUT);
    //********************************
    MW.init(SERVO_PIN);
}
void loop()
{
    int inputPos = getBCDFromPins(2); // getting the input from the DIOs
    Serial.println(inputPos);
    // using ternary operators instead of if conditions as it results in less assembly code
    int outputPos = inputPos >= 9 ? 180 : inputPos * 20;
    inputPos > 9 ? Serial.println("INVALID INPUT: go to 180") : Serial.print("Motor's position is: "), Serial.println(outputPos);
    // moving the servo to the target position
    MW.moveTo(outputPos);
}
/**
 * @brief converting the input values from the DIOs into a decimal number
 * 
 * @param shift // represents the shift between the bits and the pins. e.g. pin 2 represents bit 0 therfore the shift is 2
 * @return int 
 */
int getBCDFromPins(int shift) 
{
    int inputPos = 0;
    for (int bit = 0; bit < BCDSize; bit++)
    {
        inputPos += digitalRead(bit + shift) * (1 << bit); // (2 ^ bit) * (0 or 1)
    }
    return inputPos;
}

