#include <IRremote.h>
#include <LiquidCrystal.h>

#define ACTIVATE_LED 13
#define ALARM_LED 7
#define ALARM_BUZZER 9
#define PIRPIN 2
#define IR_RECEIVE_PIN 10

// HEX codes for remote device 0..9
#define BUTTON1 0xFE016229
#define BUTTON2 0xFD026229
#define BUTTON3 0xFC036229
#define BUTTON4 0xFB046229
#define BUTTON5 0xFA056229
#define BUTTON6 0xF9066229
#define BUTTON7 0xF8076229
#define BUTTON8 0xF7086229
#define BUTTON9 0xF6096229
#define BUTTON0 0xFF006229

LiquidCrystal lcd(12, 11, 6, 5, 4, 3); // initialization of lcd

String password;
volatile byte state = LOW;      // flag for motion detection
boolean isSystemActive = false; // flag for system activation

void setup()
{
  pinMode(ACTIVATE_LED, OUTPUT);
  pinMode(ALARM_LED, OUTPUT);
  pinMode(ALARM_BUZZER, OUTPUT);
  pinMode(PIRPIN, INPUT_PULLUP);                                          // INPUT_PULLUP checks the status of the PIR sensor
  attachInterrupt(digitalPinToInterrupt(PIRPIN), motionDetected, CHANGE); // any changes to sensor status motionDetected function will be called.

  lcd.begin(16, 2);
  digitalWrite(ACTIVATE_LED, HIGH);
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK); // Start the receiver
}
void loop()
{
  if (IrReceiver.decode() != 0) // if the receiver is not being received 0 which means null
  {
    if (password.length() < 4)
    {
      unsigned long key = IrReceiver.decodedIRData.decodedRawData;
      // check the key
      switch (key)
      {
      case BUTTON1:
        password += '1';
        break;
      case BUTTON2:
        password += '2';
        break;
      case BUTTON3:
        password += '3';
        break;
      case BUTTON4:
        password += '4';
        break;
      case BUTTON5:
        password += '5';
        break;
      case BUTTON6:
        password += '6';
        break;
      case BUTTON7:
        password += '7';
        break;
      case BUTTON8:
        password += '8';
        break;
      case BUTTON9:
        password += '9';
        break;
      case BUTTON0:
        password += '0';
        break;
      }
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
      delay(500);
      IrReceiver.resume(); // Receive the next value
    }
    else
    {
      // check the password
      if (password.equals("1234"))
      {
        if (isSystemActive)
        { // if system is active but the password is given, deactive the system.
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("NOT LOCKED");
          isSystemActive = false;
        }
        else
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("LOCKED"); // alarm system is locked, any detection after now will be trigger the alarm.
          isSystemActive = true;
        }
      }
      password = "";
    }
  }
}

// Function to check if motion sensor detect some movement
void motionDetected()
{
  if (isSystemActive)
  {
    state = !state;
    if (state == HIGH)
    {
      Serial.println("Motion detected");
      digitalWrite(ALARM_LED, HIGH);
      tone(ALARM_BUZZER, 440);
    }
    if (state == LOW) // this means there was a movement but it has stopped.
    {
      Serial.println("motion stopped");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LOCKED");
      digitalWrite(ALARM_LED, LOW);
      noTone(ALARM_BUZZER);
      IrReceiver.start();
    }
    password = ""; // password is resetted
  }
}
