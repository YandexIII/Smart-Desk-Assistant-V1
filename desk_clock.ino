#include <LiquidCrystal_I2C.h>
#include <Arduino_SensorKit.h>
#include <Wire.h>
#include <RTClib.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 48
#define RST_PIN 49
MFRC522 mfrc522(SS_PIN, RST_PIN);

const int buttonPin = 4;
const int buzzerPin = 10;
const int lightsensorPin = A2;
const int ledPin = 6;
const int treshold = 700;

int mode = 0;
int modes = 2;
int lastButtonState = HIGH;
int white = false;
int blue = false;
bool alarm_on = false;
int alarm;

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;

const char* daysOfWeek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char* monthsOfYear[] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void checkButton();
void checkRFID();
void checkLightLevel();
void updateDisplay();


void setup() {
  lcd.init();
  lcd.backlight();
  
  pinMode(buttonPin, INPUT_PULLUP); 
  pinMode(buzzerPin, OUTPUT); 
  pinMode(ledPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); 
  
  SPI.begin();      
  mfrc522.PCD_Init();
  
  Serial.begin(9600);
  Environment.begin(); 

  if (!rtc.begin()) {
    Serial.println("RTC not found!");
    while (1); 
  }
  
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, adjusting to compile time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

// --- LOOP ULTRA PROPRE ---
void loop() {
  checkButton();
  checkRFID();
  checkLightLevel();
  updateDisplay();
  checkAlarm();
  
  delay(50); // Petite pause pour stabiliser le processeur
}

// ==========================================
// --- DÉFINITION DES FONCTIONS ---
// ==========================================

void checkButton() 
{
  int state = digitalRead(buttonPin);
  if (state == LOW && lastButtonState == HIGH) {
    mode++;
    if (mode > modes) mode = 0; 
    lcd.clear();
  }
  lastButtonState = state;
}

void checkRFID() 
{
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase(); 

    //blue
    if (content.substring(1) == "4C 44 AF 32" && white == false) {
      mode = 2;
      blue = !blue;
      lcd.clear();
    } 
    //white
    else if (content.substring(1) == "83 03 61 A3" && blue == false) {
      mode = 2;
      white = !white;
      lcd.clear();
    } 
    else {
      Serial.println("Accès Refusé");
    }

    mfrc522.PICC_HaltA();
  }
}

void checkLightLevel() 
{
  int lightLevel = analogRead(lightsensorPin);
  if (lightLevel > treshold) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void updateDisplay() 
{
  DateTime now = rtc.now();

  if (mode == 0) {
    lcd.setCursor(0,0);
    lcd.print("Temp:");
    lcd.print(Environment.readTemperature(), 1); 
    lcd.print("C ");
    lcd.setCursor(0,1);
    lcd.print("Humidity:");
    lcd.print(Environment.readHumidity(), 0); 
    lcd.print("%      ");
  }
  else if (mode == 1) {
    lcd.setCursor(0,0);
    lcd.print(daysOfWeek[now.dayOfTheWeek()]);
    lcd.print(", ");
    lcd.print(now.day());
    lcd.print(" ");
    lcd.print(monthsOfYear[now.month()]);
    lcd.print(" ");
    lcd.print(now.year());
    lcd.print("     "); 

    lcd.setCursor(3,1);
    int displayHour = now.hour() % 12;
    if (displayHour == 0) displayHour = 12;
    
    if (displayHour < 10) lcd.print("0");
    lcd.print(displayHour);
    lcd.print(":");
    if(now.minute() < 10) lcd.print("0");
    lcd.print(now.minute());
    lcd.print(":");
    if(now.second() < 10) lcd.print("0");
    lcd.print(now.second());
    
    if (now.hour() >= 12) lcd.print(" PM");
    else lcd.print(" AM");
  }
  else if (mode == 2) {
    if (white)
    {
      lcd.setCursor(0,0);
      lcd.print("Logged as White");
      alarm = 7;
      lcd.setCursor(0,1);
      lcd.print("Alarm set to 7am");
    } 

    else if (blue)
    {
      lcd.setCursor(0,0);
      lcd.print("Logged as Blue");
      alarm = now.hour();
      lcd.setCursor(0,1);
      lcd.print("Alarm set to now");
    }

    else
    {
      lcd.setCursor(0,0);
      lcd.print("No one is logged");
    }
  }
}

void checkAlarm()
{
  DateTime now = rtc.now();
  if(white || blue) alarm_on = true;
  else alarm_on = false;

  if (alarm_on)
  {
    if(now.hour() == alarm)
    {
      digitalWrite(buzzerPin, HIGH);
    }
  }
  else
  {
    digitalWrite(buzzerPin, LOW);
  }
}