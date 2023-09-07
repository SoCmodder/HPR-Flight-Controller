#include <SPI.h>
#include <Adafruit_MPL3115A2.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define LAUNCH_INIT_HEIGHT 2  // 5 meters
#define APOGEE_TRIGGER_DISTANCE 20 // 25 meters
#define APOGEE_COUNTER_MAX 2
#define PICO_LED_PIN 25
#define SD_CS_PIN 17
#define SD_CLOCK_PIN 18
#define SD_FILE_NAME "hpr-log.txt"

enum class FLIGHT_STATUS { IDLE, LAUNCH_INIT, APOGEE, DEPLOY };

Adafruit_MPL3115A2 mpl;
RTC_DS3231 rtclock;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float starting_alt = 0;
float current_alt = -10;
float max_alt = 0;
bool launch_initiated = false;
bool apogee = false;
bool chute_deployed = false;
uint16_t apogee_counter = 0;
File file;

/***********************************************
 **************** SETUP *************************
 ***********************************************/
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  pinMode(PICO_LED_PIN, OUTPUT);

  if (!mpl.begin()) {
    Serial.println("Could not find MPL sensor. Check wiring.");
    while (1);
  }
  mpl.setMode(MPL3115A2_ALTIMETER);
  Serial.println("MPL3115A2 OK!");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  // Clear the buffer
  display.clearDisplay();
  // Draw a single pixel in white
  display.drawPixel(10, 10, WHITE);
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();

  initRTC();
  initSD();

  getAltReading();

  starting_alt = mpl.getLastConversionResults(MPL3115A2_ALTITUDE);
  current_alt = starting_alt;
}

/***********************************************
 **************** LOOP *************************
 ***********************************************/
void loop() {
  getAltReading();
  current_alt = mpl.getLastConversionResults(MPL3115A2_ALTITUDE);
  drawData(current_alt);
  // if current alt is more than 2 meters above starting, then rocket has launched
  if (!launch_initiated) {
    if (current_alt > starting_alt + LAUNCH_INIT_HEIGHT) {
      launch_initiated = true;
      writeString("launch");
      delay(200);
    }
  } else {
    if (!chute_deployed && apogee) {
      // Turn on relay 2
      // triggerDeployment();
      chute_deployed = true;
      writeString("deploy");
      delay(200);
    }
    if (current_alt > max_alt) {
      max_alt = current_alt;
      apogee_counter = 0;
    } else if (current_alt + APOGEE_TRIGGER_DISTANCE < max_alt) {
      apogee_counter++;
      apogee = apogee_counter >= APOGEE_COUNTER_MAX;
      writeString("apogee+1");
      delay(200);
    }

    writeString(String(current_alt));

    // now get results
    Serial.println(String(current_alt));

    delay(200);
  }
}

void writeString(String s) {
  String ts = getTimeStamp();
  openFile();
  writeDataLineBlocking(ts + " " + s);
  closeFile();  
}

void writeDataLineBlocking(String data) {
  if(!file.println(data)) {
    Serial.println("Failed to write data to file...");  
  }
  delay(200);
}

void openFile() {
  file = SD.open(SD_FILE_NAME, FILE_WRITE);
  if (!file) {
    Serial.print("error opening ");
    Serial.println(SD_FILE_NAME);
    // while (1);
  }
  delay(200);
}

void closeFile() {
  file.close();
  delay(200);
}

void writeLogHeader() {
  DateTime now = rtclock.now();
  file.println("");
  file.println("*****");
  file.print("Log Date: ");
  file.print(String(now.day()));
  file.print("/");
  file.print(String(now.month()));
  file.print("/");
  file.print(String(now.year()));
  file.print(" ");
  file.print(String(now.hour()));
  file.print(":");
  file.print(String(now.minute()));
  file.print(":");
  file.print(String(now.second()));
  file.println("");
  file.println("*****");
  delay(200);
}

void getAltReading() {
  // start a conversion
  mpl.startOneShot();
}

void drawData(float alt) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  // Time
  display.setCursor(0, 0);
  display.println("T" + getTimeStamp());
  // Altitude
  display.setCursor(61, 0);
  display.println("ALT " + String(alt));
  display.display();      // Show initial text
}

void initRTC() {
  if (!rtclock.begin()) {
    Serial.println("Couldn't find RTC!");
    //Serial.flush();
    while (1) delay(10);
  }
  if (rtclock.lostPower()) {
    // this will adjust to the date and time at compilation
    rtclock.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  //we don't need the 32K Pin, so disable it
  rtclock.disable32K();
  Serial.println("RTC OK!");
}

void initSD() {
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  // try to open the file for writing
  openFile();
  writeLogHeader();
  closeFile();
  Serial.println("SD OK!");
}

String getTimeStamp() {
  DateTime now = rtclock.now();
  return String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());  
}
