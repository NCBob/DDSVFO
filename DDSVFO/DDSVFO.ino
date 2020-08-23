//#include "TUNING.h"
//AD9850 DDS test
#include "AD9850.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//#include <EEPROM.h>
AD9850 ad9850(8, 9, 10, 11);
//LiquidCrystal_I2C lcd(0x27,20,4);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
#include "IRremote.h"
/*-----( Declare Constants )-----*/
int receiver = 31; // pin 1 of IR receiver to Arduino digital pin 11

/*-----( Declare objects )-----*/
IRrecv irrecv(receiver);           // create instance of 'irrecv'
decode_results results;            // create instance of 'decode_results'
/*-----( Declare Variables )-----*/

#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 3;


char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};
byte rowPins[ROWS] = {41, 43, 45, 47};
// connect to the row pinouts of the keypad
byte colPins[COLS] = {49, 51, 53 };
// connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
long Frequency = 0;
int jump = 1;
long num = 0;
long numA = 0;
long NEWFrequency = 0;
int setfreq;
int_fast32_t rx = 7400000;
//STARTING FREQ
int_fast32_t rx2 = 1;
// variable to hold the updated frequency
int_fast32_t increment = 10;
// starting VFO update increment in HZ.
int buttonstate = 0;
String hertz = "10 Hz";
int  hertzPosition = 5;
byte ones, tens, hundreds, thousands, tenthousands, hundredthousands, millions ; //Placeholders
String freq;
// string to hold the frequency
int_fast32_t timepassed = millis();
// int to hold the arduino miilis since startup
//int memstatus = 1;
// value to notify if memory is current or old. 0=old, 1=current.
//int ForceFreq = 1;
// Change this to 0 after you upload and run a working sketch to activate the EEPROM memory.  YOU MUST PUT THIS BACK TO 0 AND UPLOAD THE SKETCH AGAIN AFTER STARTING FREQUENCY IS SET!

volatile boolean TurnDetected;
volatile boolean up;

const int PinCLK = 2;
// Used for generating interrupts using CLK signal
const int PinDT = 3;
// Used for reading DT signal
const int PinSW = A0;
// Used for the push button switch

void isr ()
{
    // Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK
    if (digitalRead(PinCLK))
    up = !digitalRead(PinDT);
    else
    up = digitalRead(PinDT);
    TurnDetected = true;
}

void setup()
{
    ad9850.Init();
    ad9850.SetFrequency(7250000);
    Serial.begin(9600);
    
    
    irrecv.enableIRIn(); // Start the receiver
    
    //lcd.init();
    lcd.begin(20, 4);
    //initialize the lcd
    lcd.backlight();
    pinMode(PinSW, INPUT);
    attachInterrupt (0, isr, FALLING);
    // interrupt 0 is always connected to pin 2 on Arduino UNO
    // Load the stored frequency
    // if (ForceFreq == 0) {
    //  freq = String(EEPROM.read(0))+String(EEPROM.read(1))+String(EEPROM.read(2))+String(EEPROM.read(3))+String(EEPROM.read(4))+String(EEPROM.read(5))+String(EEPROM.read(6));
    //  rx = freq.toInt();
    // }
}

void loop()
{
    InputKeys();
    irtest();
    if (rx != rx2) {
        showFreq();
        ad9850.SetFrequency(rx);
        rx2 = rx;
        
    }
    buttonstate = digitalRead(A0);
    
    
    if (buttonstate == LOW)
    {
        setincrement();
    }
    
    
    
    pinMode(A0, INPUT);
    // Connect to a button that goes to GND on push
    digitalWrite(A0, HIGH);
    
    static long virtualPosition = 0;
    // without STATIC it does not count correctly!!!
    long positionnew = 0;
    long positionold = 0;
    if (!(digitalRead(PinSW)))
    {
        // check if pushbutton is pressed
        virtualPosition = 0;
        // if YES, then reset counter to ZERO
    }
    positionold = virtualPosition;
    if (TurnDetected)
    { // do this only if rotation was detected
        if (up)
        virtualPosition++;
        else
        virtualPosition--;
        TurnDetected = false;
        // do NOT repeat IF loop until new rotation detected
    }
    
    positionnew = virtualPosition;
    
    if (positionnew > positionold)
    {
        rx = rx + increment;
    }
    
    if (positionnew < positionold)
    {
        rx = rx - increment;
    }
    
}



void setincrement()
{
    if (increment == 10) {
        increment = 50;
        hertz = "50 Hz";
        hertzPosition = 8;
    }
    else if (increment == 50) {
        increment = 100;
        hertz = "100 Hz";
        hertzPosition = 7;
    }
    else if (increment == 100) {
        increment = 500;
        hertz = "500 Hz";
        hertzPosition = 7;
    }
    else if (increment == 500) {
        increment = 1000;
        hertz = "1 Khz";
        hertzPosition = 9;
    }
    else if (increment == 1000) {
        increment = 2500;
        hertz = "2.5 Khz";
        hertzPosition = 7;
    }
    else if (increment == 2500) {
        increment = 5000;
        hertz = "5 Khz";
        hertzPosition = 9;
    }
    else if (increment == 5000) {
        increment = 10000;
        hertz = "10 Khz";
        hertzPosition = 8;
    }
    else if (increment == 10000) {
        increment = 100000;
        hertz = "100 Khz";
        hertzPosition = 7;
    }
    else if (increment == 100000) {
        increment = 1000000;
        hertz = "1 Mhz";
        hertzPosition = 9;
    }
    
    
    else {
        increment = 10;
        hertz = "10 Hz";
        hertzPosition = 8;
    };
    lcd.setCursor(0, 2);
    lcd.print("     FREQ. STEP");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    
    lcd.setCursor(hertzPosition, 1);
    lcd.print(hertz);
    delay(400);
    // Adjust this delay to speed up/slow down the button menu scroll speed.
}

void showFreq() {
    String labelFreq = "";
    String freq = "";
    millions = int(rx / 1000000);
    hundredthousands = ((rx / 100000) % 10);
    tenthousands = ((rx / 10000) % 10);
    thousands = ((rx / 1000) % 10);
    hundreds = ((rx / 100) % 10);
    tens = ((rx / 10) % 10);
    ones = ((rx / 1) % 10);
    lcd.setCursor(0, 0);
    lcd.print("                   ");
    if (millions > 9) {
        lcd.setCursor(1, 0);
    }
    else {
        lcd.setCursor(4, 0);
    }
    
    freq = String(rx);
    
    if(rx < 1000)
    {
        labelFreq = "Hz ";
    }
    else if (rx < 1000000)
    {
        labelFreq = "kHz";
        //freq = 1000
        if(rx < 10000)
        {
            freq = freq.substring(0,1) + "." + freq.substring(1,4);
        }
        //freq = 10000
        else if (rx < 100000)
        {
            freq = freq.substring(0,2) + "." + freq.substring(2,5);
        }
        //freq = 10000
        else
        {
            freq = freq.substring(0,3) + "." + freq.substring(3,6);
        }
    }
    else
    {
        labelFreq = "MHz";
        // freq = 1000000
        if(rx < 10000000)
        {
            freq = freq.substring(0,1) + "." + freq.substring(1,7);
        }
        else
        {
            freq = freq.substring(0,2) + "." + freq.substring(2,8);
        }
    }
    
    lcd.setCursor(0,0);
    lcd.print("                 ");
    lcd.setCursor(16 - freq.length(),0);
    lcd.print(freq);
    lcd.setCursor(17,0);
    lcd.print(labelFreq);
    timepassed = millis();
    
}

void InputKeys()
{
    
    char key = keypad.getKey();
    if (key) {
        switch (key) {
            case '#':
            Frequency = num ;
            //Set Frequency to number entered
            
            rx = Frequency;
            lcd.setCursor(0, 3);
            lcd.print("            ");
            
            
            Serial.println("---------------------");
            Serial.println("the frequency is");
            Serial.print(Frequency);
            Serial.print(" Hz");
            Serial.println("");
            Serial.println("--------------------");
            num = 0;
            // clearLCD();
            delay (100);
            break;
            
            case '*'://clear input
            num = 0;
            //   clearLCD();
            break;
            
            default:
            key = key - '0';
            //convert from Char to integer otherwise will be out by 48
            num = (num * 10 + key);
            //move left 1 decimal place and enter next digit
            Serial.println(num);
            //print number
            
            lcd.setCursor(2, 3);
            lcd.print(num);
            
            break;
        }
        //end switch
        
    }
    //end if
    delay(10);
    
}
//end keypad subrutine


void irtest()
{
    
    
    if (irrecv.decode(&results)) // have we received an IR signal?
    
    {
        if (results.value != 0xFFFFFFFF)
        {
            Serial.println(results.value, HEX); // UN Comment to see raw values
            translateIR();
        }
        irrecv.resume(); // receive the next value
    }
}/* --(end main loop )-- */

/*-----( Declare User-written Functions )-----*/
void translateIR() // takes action based on IR code received

// describing Car MP3 IR codes

{
    
    switch (results.value)
    
    {
        
        case 0xFD28D7:
        Serial.println("             ");
        break;
        
        case 0xFD8877:
        rx = rx + increment;
        Serial.println(" up             ");
        break;
        
        case 0xFD9867:
        rx = rx - increment;
        Serial.println(" down           ");
        break;
        
        case 0xFDA857:
        setincrement();
        Serial.println(" OK             ");
        break;
        
        case 0xFD00FF:
        numA = (numA * 10 + 1);
        Serial.println(" 1              ");
        break;
        
        case 0xFD807F:
        numA = (numA * 10 + 2);
        Serial.println(" 2           ");
        break;
        
        case 0xFD40BF:
        numA = (numA * 10 + 3);
        Serial.println(" 3          ");
        break;
        
        case 0xFD20DF:
        numA = (numA * 10 + 4);
        Serial.println(" 4              ");
        break;
        
        case 0xFDA05F:
        numA = (numA * 10 + 5);
        Serial.println(" 5              ");
        break;
        
        case 0xFD609F:
        numA = (numA * 10 + 6);
        Serial.println(" 6              ");
        break;
        
        case 0xFD10EF:
        numA = (numA * 10 + 7);
        Serial.println(" 7              ");
        break;
        
        case 0xFD906F:
        numA = (numA * 10 + 8);
        Serial.println(" 8              ");
        break;
        
        case 0xFD50AF:
        numA = (numA * 10 + 9);
        Serial.println(" 9              ");
        break;
        
        case 0xFD30CF:
        setfreq = HIGH;
        Serial.println(" *              ");
        Serial.println(setfreq);
        break;
        
        case 0xFDB04F:
        numA = (numA * 10 + 0);
        Serial.println(" 0              ");
        break;
        
        case 0xFD708F:
        Serial.println(" #              ");
        break;
        
        default:
        Serial.println(" other button   ");
        
        
    }
    Serial.println(setfreq);
    if (setfreq == HIGH)
    {
        Serial.println(" set freq");
        Serial.println(NEWFrequency);
        Frequency = NEWFrequency;
        
        
        setfreq = LOW;
        rx = Frequency;
        NEWFrequency = 0;
        numA = 0;
        lcd.setCursor(2, 3);
        lcd.print("                  ");
    }
    NEWFrequency = numA ;
    lcd.setCursor(2, 3);
    lcd.print(numA);
    //Set Frequency to number entered
    delay(100);
    
    
} //END translateIR

