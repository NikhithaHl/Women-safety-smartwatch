#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

HardwareSerial GSM(1);

// =====================================================
// PIN DEFINITIONS
// =====================================================

#define GSM_RX 27
#define GSM_TX 26

#define SOS_BUTTON 4

#define LED_PIN 13
#define BUZZER_PIN 23

#define OLED_SDA 21
#define OLED_SCL 22

// =====================================================
// OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// =====================================================
// PHONE NUMBER
// =====================================================

const char PARENT_NUMBER[] = "+918277123568";

// =====================================================
// STARTING GPS LOCATION
// =====================================================

double currentLatitude = 12.865851;
double currentLongitude = 74.924951;

// =====================================================
// SYSTEM VARIABLES
// =====================================================

bool emergencyTriggered = false;
bool gpsTracking = false;

unsigned long lastGPS = 0;

// =====================================================
// OLED FUNCTION
// =====================================================

void showOLED(String line1, String line2, String line3)
{
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 5);
  display.println(line1);

  display.setCursor(0, 25);
  display.println(line2);

  display.setCursor(0, 45);
  display.println(line3);

  display.display();
}

// =====================================================
// LATITUDE → NMEA
// =====================================================

String toNMEALatitude(double latitude)
{
  double absoluteValue = abs(latitude);

  int degrees = (int)absoluteValue;

  double minutes =
    (absoluteValue - degrees) * 60.0;

  char result[20];

  sprintf(
    result,
    "%02d%07.4f",
    degrees,
    minutes
  );

  return String(result);
}

// =====================================================
// LONGITUDE → NMEA
// =====================================================

String toNMEALongitude(double longitude)
{
  double absoluteValue = abs(longitude);

  int degrees = (int)absoluteValue;

  double minutes =
    (absoluteValue - degrees) * 60.0;

  char result[20];

  sprintf(
    result,
    "%03d%07.4f",
    degrees,
    minutes
  );

  return String(result);
}

// =====================================================
// NMEA CHECKSUM
// =====================================================

void printNMEA(const String &sentence)
{
  byte checksum = 0;

  for (
    unsigned int i = 0;
    i < sentence.length();
    i++
  )
  {
    checksum ^= sentence[i];
  }

  Serial.print("$");
  Serial.print(sentence);
  Serial.print("*");

  if (checksum < 16)
  {
    Serial.print("0");
  }

  Serial.println(checksum, HEX);
}

// =====================================================
// SIMULATE GPS MOVEMENT
// =====================================================

void updateGPSLocation()
{
  /*
     Small changes simulate movement.

     0.00001 degree latitude ≈ 1.1 metres
     0.00001 degree longitude ≈ 1.1 metres
     approximately at this latitude.

     The random movement below is roughly
     a few metres every second.
  */

  double latitudeChange =
    random(-15, 16) / 1000000.0;

  double longitudeChange =
    random(-15, 16) / 1000000.0;

  currentLatitude += latitudeChange;
  currentLongitude += longitudeChange;
}

// =====================================================
// GENERATE GPS NMEA DATA
// =====================================================

void generateGPS()
{
  String lat =
    toNMEALatitude(currentLatitude);

  String lon =
    toNMEALongitude(currentLongitude);

  String northSouth =
    currentLatitude >= 0 ? "N" : "S";

  String eastWest =
    currentLongitude >= 0 ? "E" : "W";

  // =================================================
  // GPRMC
  // =================================================

  printNMEA(
    "GPRMC,170445.00,A," +
    lat + "," +
    northSouth + "," +
    lon + "," +
    eastWest +
    ",1.20,45.00,180926,,,A"
  );

  // =================================================
  // GPGGA
  // =================================================

  printNMEA(
    "GPGGA,170445.00," +
    lat + "," +
    northSouth + "," +
    lon + "," +
    eastWest +
    ",1,08,0.90,42.0,M,-0.5,M,,"
  );
}

// =====================================================
// DISPLAY CURRENT GPS LOCATION
// =====================================================

void displayLocation()
{
  // Serial
  Serial.println();
  Serial.println("================================");
  Serial.println(" GPS LOCATION");
  Serial.println("================================");

  Serial.print("Latitude : ");
  Serial.println(currentLatitude, 6);

  Serial.print("Longitude : ");
  Serial.println(currentLongitude, 6);

  Serial.print("Google Maps: ");

  Serial.print(
    "https://maps.google.com/?q="
  );

  Serial.print(currentLatitude, 6);

  Serial.print(",");

  Serial.println(currentLongitude, 6);

  Serial.println("================================");

  // =================================================
  // OLED
  // =================================================

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("SOS - GPS ACTIVE");

  display.setCursor(0, 16);
  display.print("LAT:");
  display.println(currentLatitude, 6);

  display.setCursor(0, 31);
  display.print("LON:");
  display.println(currentLongitude, 6);

  display.setCursor(0, 48);
  display.println("TRACKING ACTIVE");

  display.display();
}

// =====================================================
// SEND SMS
// =====================================================

void sendSMS()
{
  Serial.println();
  Serial.println("Sending emergency SMS...");

  showOLED(
    "SOS ACTIVATED",
    "SENDING SMS...",
    "PLEASE WAIT"
  );

  // SMS mode
  GSM.println("AT+CMGF=1");

  delay(1000);

  // Recipient
  GSM.print("AT+CMGS=\"");
  GSM.print(PARENT_NUMBER);
  GSM.println("\"");

  delay(1500);

  // =================================================
  // SMS
  // =================================================

  GSM.println("EMERGENCY ALERT!");

  GSM.println(
    "Please help me. I am in danger."
  );

  GSM.println();

  GSM.print("Latitude: ");
  GSM.println(currentLatitude, 6);

  GSM.print("Longitude: ");
  GSM.println(currentLongitude, 6);

  GSM.println();

  GSM.print("Location: ");

  GSM.print(
    "https://maps.google.com/?q="
  );

  GSM.print(currentLatitude, 6);

  GSM.print(",");

  GSM.println(currentLongitude, 6);

  // CTRL + Z
  GSM.write(26);

  delay(7000);

  Serial.println("SMS SENT.");

  showOLED(
    "SOS ACTIVATED",
    "SMS SENT",
    "CALLING PARENT"
  );
}

// =====================================================
// MAKE CALL
// =====================================================

void makeCall()
{
  Serial.println();
  Serial.println("Calling parent...");

  GSM.print("ATD");
  GSM.print(PARENT_NUMBER);
  GSM.println(";");

  delay(1000);

  Serial.println("CALL STARTED.");

  showOLED(
    "SOS ACTIVATED",
    "CALLING PARENT",
    "STAY SAFE!"
  );
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  delay(1000);

  // Random seed
  randomSeed(
    micros()
  );

  Serial.println();
  Serial.println("================================");
  Serial.println(" SURAKSHA+ SYSTEM");
  Serial.println("================================");

  // =================================================
  // GSM
  // =================================================

  GSM.begin(
    9600,
    SERIAL_8N1,
    GSM_RX,
    GSM_TX
  );

  Serial.println("GSM INITIALIZED");

  // =================================================
  // SOS BUTTON
  // =================================================

  pinMode(
    SOS_BUTTON,
    INPUT_PULLUP
  );

  // =================================================
  // LED
  // =================================================

  pinMode(
    LED_PIN,
    OUTPUT
  );

  digitalWrite(
    LED_PIN,
    LOW
  );

  // =================================================
  // BUZZER
  // =================================================

  pinMode(
    BUZZER_PIN,
    OUTPUT
  );

  digitalWrite(
    BUZZER_PIN,
    LOW
  );

  // =================================================
  // OLED
  // =================================================

  Wire.begin(
    OLED_SDA,
    OLED_SCL
  );

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C))
  {
    Serial.println("OLED NOT FOUND!");

    while (1)
    {
      delay(1000);
    }
  }

  // =================================================
  // INITIAL OLED
  // =================================================

  showOLED(
    "WOMEN SAFETY",
    "SYSTEM READY",
    "Press SOS"
  );

  Serial.println();
  Serial.println("SYSTEM READY");
  Serial.println("GPS TRACKING: OFF");
  Serial.println("Press SOS to activate GPS.");
  Serial.println();
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
  // =================================================
  // WAIT FOR SOS
  // =================================================

  if (!emergencyTriggered)
  {
    if (digitalRead(SOS_BUTTON) == LOW)
    {
      delay(50);

      if (digitalRead(SOS_BUTTON) == LOW)
      {
        // =============================================
        // SOS ACTIVATED
        // =============================================

        emergencyTriggered = true;
        gpsTracking = true;

        Serial.println();
        Serial.println("================================");
        Serial.println(" SOS ACTIVATED");
        Serial.println("================================");

        Serial.println("GPS TRACKING STARTED");
        Serial.println();

        // =============================================
        // ALARM
        // =============================================

        digitalWrite(
          LED_PIN,
          HIGH
        );

        digitalWrite(
          BUZZER_PIN,
          HIGH
        );

        // =============================================
        // OLED
        // =============================================

        showOLED(
          "SOS ACTIVATED",
          "GPS TRACKING",
          "PLEASE WAIT"
        );

        delay(2000);

        // =============================================
        // FIRST GPS LOCATION
        // =============================================

        generateGPS();

        displayLocation();

        // =============================================
        // SEND SMS
        // =============================================

        sendSMS();

        // =============================================
        // CALL PARENT
        // =============================================

        delay(3000);

        makeCall();

        Serial.println();
        Serial.println("================================");
        Serial.println(" EMERGENCY SYSTEM ACTIVE");
        Serial.println(" GPS TRACKING ACTIVE");
        Serial.println("================================");
      }
    }
  }

  // =================================================
  // GPS TRACKING AFTER SOS
  // =================================================

  if (gpsTracking)
  {
    if (millis() - lastGPS >= 1000)
    {
      lastGPS = millis();

      // Change simulated location
      updateGPSLocation();

      // Print new GPS position
      generateGPS();

      // Display new position
      displayLocation();
    }
  }

  // =================================================
  // FLASH LED + BUZZER
  // =================================================

  if (emergencyTriggered)
  {
    digitalWrite(
      LED_PIN,
      HIGH
    );

    digitalWrite(
      BUZZER_PIN,
      HIGH
    );

    delay(300);

    digitalWrite(
      LED_PIN,
      LOW
    );

    digitalWrite(
      BUZZER_PIN,
      LOW
    );

    delay(300);
  }

  // =================================================
  // GSM RESPONSE
  // =================================================

  while (GSM.available())
  {
    Serial.write(
      GSM.read()
    );
  }
}
