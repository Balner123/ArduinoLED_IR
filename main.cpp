#include <Arduino.h>
#include <IRremote.hpp>

// Definice použití AVR timeru 1 pro IR přijímač
#define IR_USE_AVR_TIMER1

// Definice pinů
#define IR_RECEIVE_PIN 12  // Pin pro IR přijímač
#define MINUS 8            // GND pin pro LED diody

// Pole s názvy barev
String nazvy[] = {"RED", "GREEN", "BLUE"};

// Počet LED diod
int count = 3;

// Piny připojené k LED diodám
int leds[] = {9, 10, 5};

// Pole s IR kódy odpovídajícími číslům 0-9
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
    3041591040
};

// Řetězec pro uchování vstupního čísla z dálkového ovládání
String stringNum;

// Funkce pro nalezení pozice IR kódu v poli 'pole' a vrácení jako řetězec
String position(unsigned long code)
{
    for (int i = 0; i < 10; i++)
    {
        if (code == pole[i])
        {
            return String(i);
        }
    }
    return "";
}

void setup()
{
    // Inicializace IR přijímače
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    // Inicializace sériové komunikace pro ladění
    Serial.begin(9600);

    // Nastavení LED pinů jako výstupy a jejich vypnutí
    for (int i = 0; i < count; i++)
    {
        pinMode(leds[i], OUTPUT);
        digitalWrite(leds[i], LOW);
    }

    // Nastavení pinu MINUS jako výstup a jeho vypnutí
    pinMode(MINUS, OUTPUT);
    digitalWrite(MINUS, LOW);

    // Nastavení IR_RECEIVE_PIN jako vstup (není nutné, provádí to IrReceiver.begin)
    pinMode(IR_RECEIVE_PIN, INPUT);
}

// Funkce pro vypnutí všech LED diod
void off_leds()
{
    for (int i = 0; i < count; i++)
    {
        digitalWrite(leds[i], LOW);
    }
}

// Proměnné pro sledování aktuálního stavu a indexu LED
int d = 0;
int der = 0;

void loop()
{
    // Vytiskne aktuální název barvy, pokud začíná přijímat novou hodnotu
    if (d == 0)
    {
        Serial.print(nazvy[der]);
        Serial.print(": ");
        d++;
    }

    // Kontrola, zda byl přijat IR kód
    if (IrReceiver.decode())
    {
        // Získání surových dat IR
        unsigned long irData = IrReceiver.decodedIRData.decodedRawData;

        if (irData > 0)
        {
            // Resetování proměnné stavu
            d = 0;
            // Definování IR kódu pro tlačítko 'enter'
            unsigned long enter = 4127850240;

            // Kontrola, zda je přijatý IR kód kód 'enter'
            if (irData == enter)
            {
                if (der < 3)
                {
                    // Převod řetězce čísla na celé číslo
                    int intNum = stringNum.toInt();
                    // Kontrola, zda je číslo v platném rozsahu pro PWM (0-255)
                    if (intNum < 256)
                    {
                        // Nastavení jasu aktuální LED
                        analogWrite(leds[der], intNum);
                        // Vyčištění vstupního řetězce a přechod k další LED
                        stringNum = "";
                        der++;
                    }
                    else
                    {
                        // Vytištění chybové zprávy, pokud je číslo mimo rozsah
                        Serial.println("Minmorozsah\n");
                        // Vyčištění vstupního řetězce
                        stringNum = "";
                    }
                }
                else
                {
                    // Pokud byly nastaveny všechny LED, vypnutí všech LED
                    off_leds();
                    // Resetování indexu LED
                    der = 0;
                }
            }
            else
            {
                // Přidání přijatého čísla do vstupního řetězce a jeho vytištění
                stringNum += position(irData);
                Serial.println(stringNum);
            }
        }

        // Obnovení IR přijímače pro příjem dalšího kódu
        IrReceiver.resume();
    }
}
