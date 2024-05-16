#include <Arduino.h>
#include <IRremote.hpp>
#define IR_USE_AVR_TIMER1
#define IR_RECEIVE_PIN 12
#define MINUS 8

String nazvy[] = {"RED","GREEN","BLUE"};

int count = 3;

int leds[] = {9, 10,5};

unsigned long pole[] = {
    3910598400,         
    4077715200,
    3877175040,
    2707357440,
    4144561920,
    3810328320,
    2774204160,
    3175284480,
    2907897600,
    3041591040};

String stringNum;

String position(unsigned long code)
{
    for (int i = 0; i < 10; i++)
    {
        if (code == pole[i])
        {
            return String(i);
        }
    }
}

void setup()
{
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
    Serial.begin(9600);

    for (int i = 0; i < count; i++)
    {
        pinMode(leds[i], OUTPUT);
        digitalWrite(leds[i],LOW);
    }
    pinMode(MINUS, OUTPUT);
    digitalWrite(MINUS, LOW);
    pinMode(IR_RECEIVE_PIN, INPUT);


}

void off_leds()
{
    for (int i = 0; i < count; i++)
    {
        digitalWrite(leds[i], LOW);
    }
}


int d = 0;
int der = 0;
void loop()
{
    if(d==0){
        Serial.print(nazvy[der]);
        Serial.print(": ");
        d++;
    }
    

    if (IrReceiver.decode())
    {
        unsigned long irData = IrReceiver.decodedIRData.decodedRawData; // Print "old" raw data
        if (irData > 0)
        {
            d=0;
            unsigned long enter = 4127850240;
           
            if (irData == enter)
            {
                if (der < 3)
                {
                    int intNum = stringNum.toInt();
                    if (intNum < 256)
                    {

                        analogWrite(leds[der], intNum);

                        stringNum = "";
                        der++;
                    }
                    else
                    {
                        Serial.println("Minmorozsah\n");
                        stringNum = "";
                    }
                }
                else
                {
                    off_leds(); 
                    
                    der = 0;
                }
            }
            else
            {
                stringNum += position(irData);
                Serial.println(stringNum);
            }
        }

        IrReceiver.resume();
    }
}
