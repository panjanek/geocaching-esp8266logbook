
// power mode: 25mA
// sleep: 25uA

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ESP8266WiFi.h"
#include <LittleFS.h>
#include "RTClib.h"
#include <ArduinoOTA.h>

#define ADMIN_PASSWORD        "admin"

#define LANG_EN 0
#define LANG_PL 1

// select LANG_EN or LANG_PL
#define LANGUAGE LANG_EN
#define WELCOME_LINE5        " <cache>"
#define WELCOME_LINE6        "    <name>"
#define WELCOME_LINE8        "PanJanek123"

#if LANGUAGE == LANG_EN

#define WELCOME_LINE1        "Digital"
#define WELCOME_LINE2        "Logbook"
#define WELCOME_LINE3        "   of"
#define WELCOME_LINE4        "geocache"
#define WELCOME_LINE7        "by"
#define WELCOME_LINE8        "PanJanek123"
#define MENU_LOG             "log entry"
#define MENU_BROWSE          "browse"
#define MENU_FINISH          "finish"
#define MSG_ENTER_NICK       "Enter nick:"
#define BUTTON_CONTINUE      "ok ->"
#define BUTTON_CANCEL        "cancel"
#define CONFIRM_LINE1        "Final confirmation"
#define CONFIRM_LINE2        "is nick correct"
#define CONFIRM_LINE3        "later change"
#define CONFIRM_LINE4        "won't be possible"
#define SLEEP_LINE1          "Sleeping..."
#define SLEEP_LINE2          "to save battery"
#define BUTTON_BACK          "back/undo"
#define BUTTON_SAVE          "  ok  "
#define MSG_SAVED             "Saved"

#else

#define WELCOME_LINE1        "Cyfrowy"
#define WELCOME_LINE2        "Logbook"
#define WELCOME_LINE3        "skrytki"
#define WELCOME_LINE4        "geocache"
#define WELCOME_LINE7        "by"
#define MENU_LOG             "dopisz sie"
#define MENU_BROWSE          "przegladaj"
#define MENU_FINISH          "koniec"
#define MSG_ENTER_NICK       "Podaj nick:"
#define BUTTON_CONTINUE      "dalej"
#define BUTTON_CANCEL        "anuluj"
#define CONFIRM_LINE1        "Potwierdz ostatecznie"
#define CONFIRM_LINE2        "poprawnosc nicka,"
#define CONFIRM_LINE3        "Potem zmiana"
#define CONFIRM_LINE4        "nie bedzie mozliwa."
#define SLEEP_LINE1          "Zasypiam..."
#define SLEEP_LINE2          "by oszczedzac baterie"
#define BUTTON_BACK          "cofnij/popraw"
#define BUTTON_SAVE          "zapisz"
#define MSG_SAVED            "Zapisano"

#endif

#define ADMIN_TRIGGERED_BY_BUTTONS_AT_START   false
#define LOW_BATTERY_TRESHOLD                  2800
#define STOP_SYSLOG_BELOW_MEMORY              300000

#define SCREEN_ROTATION 2
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define POWER_PIN 13
#define BUTTON_SELECT 12
#define BUTTON_ENTER 14
#define SLEEP_AFTER_SECONDS 30

#define APP_VERSION "2.01"
#define LOGBOOK_FILE_NAME "/logbook.txt"
#define STATS_FILE_NAME "/stats.dat"
#define SYSLOG_FILE_NAME "/syslog.txt"
#define DATETIME_FORMAT "YYYY-MM-DD hh:mm:ss"
#define AP_WIFI_SSID "GeocacheLogbook"
#define AP_WIFI_PASS "geocaching"

#define OLED_RESET -1 

#define MODE_HOME 0
#define MODE_INPUT 1
#define MODE_CONFIRM 2
#define MODE_BROWSE 3
#define MODE_ADMIN 4
#define MODE_STATS 5
#define MODE_SYSINFO 6
#define MODE_SYSINFO2 7
#define MODE_CLOCK 8
#define MODE_CLEAR 9
#define MODE_TX 10

#define INPUT_TYPE_UPPER 0
#define INPUT_TYPE_LOWER 1
#define INPUT_TYPE_DIGITS 2
#define INPUT_TYPE_SPEC 3

#define POSITION_HOME_INPUT  0 
#define POSITION_HOME_BROWSE 1
#define POSITION_HOME_END    2

#define POSITION_INPUT_CHANGE     21 
#define POSITION_INPUT_BACKSPACE  22 
#define POSITION_INPUT_NEXT       23 
#define POSITION_INPUT_CANCEL     24 

#define POSITION_CONFIRM_BACK     0 
#define POSITION_CONFIRM_OK       1 

#define POSITION_BROWSE_UP        0
#define POSITION_BROWSE_DOWN      1
#define POSITION_BROWSE_TOP       2
#define POSITION_BROWSE_BOTTOM    3
#define POSITION_BROWSE_BACK      4

#define POSITION_ADMIN_STATS     0
#define POSITION_ADMIN_SYSINFO   1
#define POSITION_ADMIN_CLOCK     2
#define POSITION_ADMIN_TX        3
#define POSITION_ADMIN_CLEAR     4
#define POSITION_ADMIN_BACK      5

#define POSITION_STATS_BACK      0
#define POSITION_SYSINFO_BACK    0
#define POSITION_SYSINFO_NEXT    1

#define POSITION_CLOCK_YEARUP     0
#define POSITION_CLOCK_YEARDOWN   1
#define POSITION_CLOCK_MONTHUP    2
#define POSITION_CLOCK_MONTHDOWN  3
#define POSITION_CLOCK_DAYUP      4
#define POSITION_CLOCK_DAYDOWN    5
#define POSITION_CLOCK_HOURUP     6
#define POSITION_CLOCK_HOURDOWN   7
#define POSITION_CLOCK_MINUTEUP   8
#define POSITION_CLOCK_MINUTEDOWN 9
#define POSITION_CLOCK_SET        10
#define POSITION_CLOCK_CANCEL     11

#define POSITION_CLEAR_CANCEL     0
#define POSITION_CLEAR_LOGBOOK    1
#define POSITION_CLEAR_STATS      2
#define POSITION_CLEAR_SYSLOG     3
#define POSITION_CLEAR_CONFIRM    4

const char date_format[] PROGMEM = DATETIME_FORMAT;
const char admin_password[] PROGMEM = ADMIN_PASSWORD;

static const unsigned char PROGMEM backspace_icon[] =
{ 0b00011111, 0b11111110,
  0b00110000, 0b00000010,
  0b01100001, 0b00100010,
  0b11000000, 0b11000010,
  0b11000000, 0b11000010,  
  0b01100001, 0b00100010,
  0b00110000, 0b00000010,
  0b00011111, 0b11111110 };

static const unsigned char PROGMEM up_icon[] =
{ 0b00000000, 
  0b00000000, 
  0b00011000, 
  0b00111100, 
  0b01111110,  
  0b00000000, 
  0b00000000, 
  0b00000000 };

static const unsigned char PROGMEM down_icon[] =
{ 0b00000000, 
  0b00000000, 
  0b01111110, 
  0b00111100, 
  0b00011000,  
  0b00000000, 
  0b00000000, 
  0b00000000 };  

static const unsigned char PROGMEM upup_icon[] =
{ 0b00000000, 
  0b00011000, 
  0b00111100, 
  0b01111110,  
  0b00011000, 
  0b00111100, 
  0b01111110,
  0b00000000 };

static const unsigned char PROGMEM downdown_icon[] =
{ 0b00000000,  
  0b01111110, 
  0b00111100, 
  0b00011000,  
  0b01111110, 
  0b00111100, 
  0b00011000,
  0b00000000 };    

const unsigned char welcome_logo [] PROGMEM=
{
	0x00, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 
	0x18, 0x0c, 0x04, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x1c, 0x1e, 0x0c, 0x07, 0xf0, 0x00, 0x00, 0x00, 
	0x0c, 0x1f, 0x18, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x06, 0x1f, 0x38, 0x07, 0xf0, 0x00, 0x00, 0x00, 
	0x03, 0x8e, 0x70, 0x07, 0xfc, 0x00, 0x00, 0x00, 0x0f, 0x80, 0xe0, 0x07, 0xff, 0x80, 0x00, 0x00, 
	0x38, 0xf1, 0xe0, 0x07, 0xff, 0xf0, 0x00, 0x00, 0x01, 0xff, 0xc0, 0x07, 0xff, 0xf8, 0x00, 0x00, 
	0x06, 0xff, 0x80, 0x07, 0xff, 0xfe, 0x00, 0x00, 0x0c, 0x7f, 0x80, 0x07, 0xff, 0xfe, 0x00, 0x00, 
	0x00, 0x3f, 0xe0, 0x07, 0xf1, 0xfe, 0x00, 0x00, 0x00, 0x7f, 0xf0, 0x07, 0xf0, 0x7c, 0x00, 0x00, 
	0x00, 0x7f, 0xfc, 0x07, 0xf0, 0x18, 0x00, 0x00, 0x00, 0xf8, 0x7e, 0x07, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0xf8, 0x3f, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x1f, 0x87, 0xf0, 0x00, 0x00, 0x00, 
	0x01, 0xf0, 0x0f, 0x87, 0xf0, 0x00, 0x0e, 0x00, 0x01, 0xf0, 0x07, 0xc7, 0xf0, 0x00, 0x1e, 0x00, 
	0x01, 0xf0, 0x07, 0xe7, 0xf0, 0x00, 0x7f, 0x00, 0x03, 0xe0, 0x00, 0x07, 0xf0, 0x00, 0x3f, 0x00, 
	0x03, 0xe0, 0x00, 0x07, 0xf0, 0x00, 0x3f, 0x80, 0x03, 0xe0, 0x00, 0x07, 0xf0, 0x00, 0x1f, 0x80, 
	0x03, 0xe0, 0x00, 0x07, 0xf0, 0x00, 0x1f, 0x80, 0x03, 0xe0, 0x00, 0x07, 0xf0, 0x00, 0x0f, 0x80, 
	0x03, 0xe0, 0x00, 0x07, 0xf0, 0x00, 0x0f, 0xc0, 0x03, 0xe0, 0x00, 0x07, 0xf0, 0x00, 0x0f, 0xc0, 
	0x03, 0xf0, 0x00, 0x0f, 0xf0, 0x00, 0x0f, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0x01, 0xf0, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 
	0x01, 0xf8, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0xfc, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x07, 0xf0, 0x3f, 0xff, 0xf0, 
	0x00, 0xfc, 0x00, 0x07, 0xe0, 0x3f, 0xff, 0xc0, 0x00, 0x7e, 0x00, 0x07, 0xf0, 0x3f, 0xff, 0x80, 
	0x00, 0x7e, 0x00, 0x07, 0xe0, 0x3f, 0xff, 0x00, 0x00, 0x38, 0x00, 0x07, 0xe0, 0x1f, 0xfc, 0x00, 
	0x00, 0x30, 0x00, 0x07, 0xe0, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x1f, 0xe0, 0x00, 
	0x00, 0x00, 0x00, 0x07, 0xe0, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x08, 0x07, 0xe0, 0x1f, 0x00, 0x00, 
	0x00, 0x00, 0x1c, 0x07, 0xe0, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x87, 0xe0, 0x0c, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0xef, 0xe0, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xe0, 0x0c, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0xff, 0xe0, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xe0, 0x0c, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0xff, 0xe0, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xe0, 0x04, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xe0, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x06, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x07, 0xe0, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x06, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00
};

const unsigned char sleep_logo [] PROGMEM = {
	0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x40, 0x00, 0x1c, 
	0x01, 0xc0, 0x00, 0x38, 0x03, 0x80, 0x00, 0x70, 0x0f, 0x80, 0x00, 0xfe, 0x0f, 0x00, 0x01, 0xff, 
	0x1f, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x3f, 0x00, 0xfc, 0x00, 0x7f, 0x00, 0xfc, 0x00, 
	0x7f, 0x00, 0x38, 0x00, 0x7f, 0x00, 0x70, 0x00, 0xff, 0x00, 0xfc, 0x00, 0xff, 0x00, 0xfc, 0x00, 
	0xff, 0x80, 0x00, 0x00, 0xff, 0x80, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 
	0x7f, 0xe0, 0x00, 0x00, 0x7f, 0xf8, 0x00, 0x08, 0x7f, 0xfc, 0x00, 0x18, 0x3f, 0xff, 0x00, 0x70, 
	0x3f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xe0, 0x0f, 0xff, 0xff, 0xc0, 0x07, 0xff, 0xff, 0x80, 
	0x03, 0xff, 0xff, 0x00, 0x01, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0x07, 0x80, 0x00
};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//RTC_DS3231 rtc;
//RTC_PCF8523 rtc;
RTC_PCF8523 rtc;
uint8_t button_select = 0;
uint8_t button_enter = 0;
uint8_t previous_button_select = 0;
uint8_t previous_button_enter = 0;
bool select_pressed = false;
bool enter_pressed = false;
bool beginning = false;
bool input_type_changed = false;
bool rtc_ok = false;
unsigned long last_activity = 0;
uint8_t mode = MODE_HOME;
uint8_t position = 0;
uint8_t input_type = INPUT_TYPE_UPPER;
int logbook_size = -1;
int scroll = 0;
long cycles = 0;
int low_battery = 0;
struct statsStruct {
    long power;
    long select;
    long enter;
    long upseconds;
    long manualoffs;
} stats;
char input[22];
char buffer[22];
char buffer2[22];
char buffer3[22];
char buffer4[22];
char buffer5[22];

WiFiServer server(80);

ADC_MODE(ADC_VCC);

void setup() {
  delay(50);
  rtc_ok = false; 
  WiFi.mode( WIFI_OFF );
  WiFi.forceSleepBegin();
  delay(1);
  WiFi.persistent( false );
  pinMode(0, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);  
  pinMode(POWER_PIN, OUTPUT);
  pinMode(16, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
  pinMode(BUTTON_ENTER, INPUT_PULLUP);
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW);
  Serial.begin(9600);
  delay(10);
  Serial.println();
  serialDebugP(F("setup"));

  if (!LittleFS.begin()) {
    serialDebugP(F("LittleFS error"));
  }

  rtc_ok = rtc.begin(); 
  if (!rtc_ok)
    writeToSyslog("RTC ERR");

  readStats();
  stats.power++;
  welcome();
}

void serialDebugDateTime() {
  readDateTime(buffer4);
  buffer4[19] = 0;
  Serial.print(F("["));
  Serial.print(buffer4);
  Serial.print(F("] "));
}

void serialDebugP(const __FlashStringHelper* msg) {
  serialDebugDateTime();
  Serial.print(msg);
  serialDebugInfo();
}

void serialDebug(const char* msg) {
  serialDebugDateTime();
  Serial.print(msg);
  serialDebugInfo();
}

void serialDebugInfo() {
  Serial.print(F(" Vcc="));
  Serial.print(ESP.getVcc());
  Serial.print(F(" fs_space="));
  Serial.print(getFreeSpace());
  Serial.print(F(" free_heap="));
  Serial.println(ESP.getFreeHeap());  
}

void initData()
{
   last_activity = millis();
   mode = MODE_HOME;
   position = 0;
   logbook_size = -1;
   memset(input, ' ', 21);
   input[21] = 0;
}

void readDateTime(char* buf)
{
  if (rtc_ok) {
    DateTime now = rtc.now();
    strcpy_P(buf, date_format);
    now.toString(buf);
  } else {
    strcpy(buf, "0000-00-00 00:00:00");
  }
}

void oledTextP(uint8_t size, int16_t x, int16_t y, const __FlashStringHelper *text, bool invert = false, bool button = false)
{
  display.setTextSize(size); 
  if (invert || button) {
    int16_t x1,y1;
    uint16_t w, h;
    display.getTextBounds(text, x, y, &x1, &y1, &w, &h);
    if (invert) {
      display.fillRoundRect(x1-2, y1-2, w+4, h+4, 2, BLACK);
      display.fillRoundRect(x1-2, y1-2, w+4, h+4, 2, WHITE);
    } else {
      display.drawRoundRect(x1-2, y1-2, w+4, h+4, 2, WHITE);
    }
  }
  display.setTextColor(invert ? BLACK : WHITE, invert ? WHITE : BLACK);
  display.setCursor(x, y);
  display.print(text);
  display.setTextColor(WHITE);
}

void oledTextP(uint8_t size, int16_t y, const __FlashStringHelper *text, bool invert = false, bool button = false)
{
  int length = strlen_P((PGM_P)text);
  oledTextP(size, SCREEN_WIDTH/2 - length*6*size/2, y, text, invert, button);
}

void oledLabelAndValue(int16_t y, const __FlashStringHelper *label, const char *value)
{
  display.setTextSize(1); 
  display.setTextColor(WHITE);
  display.setCursor(0, y);
  display.print(label);
  int len = strlen(value);
  display.setCursor(127-6*len, y);
  display.print(value);
}

void oledLabelAndValue(int16_t y, const __FlashStringHelper *label, unsigned long value)
{
  ultoa(value, buffer, 10);
  oledLabelAndValue(y, label, buffer);
}

void oledText(uint8_t size, int16_t x, int16_t y, const char *text, bool invert = false, bool button = false)
{
  display.setTextSize(size); 
  if (invert || button) {
    int16_t x1,y1;
    uint16_t w, h;
    display.getTextBounds(text, x, y, &x1, &y1, &w, &h);
    if (invert) {
      display.fillRoundRect(x1-2, y1-2, w+4, h+4, 2, WHITE);
    } else {
      display.fillRoundRect(x1-2, y1-2, w+4, h+4, 2, BLACK);
      display.drawRoundRect(x1-2, y1-2, w+4, h+4, 2, WHITE);
    }
  }
  display.setTextColor(invert ? BLACK : WHITE, invert ? WHITE : BLACK);
  display.setCursor(x, y);
  display.print(text);
  display.setTextColor(WHITE);
}

void oledText(uint8_t size, int16_t y, const char *text, bool invert = false, bool button = false)
{
  int length = strlen(text);
  oledText(size, SCREEN_WIDTH/2 - length*6*size/2, y, text, invert, button);
}

void oledIcon(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, bool invert, bool button)
{
  if (invert || button) {
    if (invert) {
      display.fillRoundRect(x-2, y-2, w+4, h+4, 2, WHITE);
    } else {
      display.fillRoundRect(x-2, y-2, w+4, h+4, 2, BLACK);
      display.drawRoundRect(x-2, y-2, w+4, h+4, 2, WHITE);
    }
  }  
  display.drawBitmap(x, y, bitmap, w, h, invert ? BLACK : WHITE);
}

void readStats()
{
  memset((uint8_t*) &stats, 0, sizeof(stats));
  if (LittleFS.exists(STATS_FILE_NAME))
  { 
      serialDebugP(F("stats exists"));
      File file = LittleFS.open(STATS_FILE_NAME, "r");
      file.read((uint8_t*) &stats, sizeof(stats));
      file.close();
  }
  else
  {
    writeToSyslog("CREATESTATS");
    File file = LittleFS.open(STATS_FILE_NAME, "w");
    file.write((uint8_t*) &stats, sizeof(stats));
    file.close();
  }
}

void saveStats()
{
  File file = LittleFS.open(STATS_FILE_NAME, "w");
  file.write((uint8_t*) &stats, sizeof(stats));
  file.close();
}

int getLogbookSize()
{
  if (LittleFS.exists(LOGBOOK_FILE_NAME))
  { 
      serialDebugP(F("logbook file exists"));
      File file = LittleFS.open(LOGBOOK_FILE_NAME, "r");
      int size = file.size() / 42;
      file.close();
      return size;
  }
  else
  {
    writeToSyslog("CREATELOGBOOK");
    File file = LittleFS.open(LOGBOOK_FILE_NAME, "w");
    file.close();
    return 0;
  }
}

void saveToLogbook()
{
  int logbook_size = getLogbookSize();
  File file = LittleFS.open(LOGBOOK_FILE_NAME, "a");
  readDateTime(buffer);
  file.write(buffer,19);
  file.write('\t');
  file.write(input, 21);
  file.write('\n');
  file.close();
  writeToSyslog("LOG");
}

void readLogbook(int nr, char *time, char* text)
{
  File file = LittleFS.open(LOGBOOK_FILE_NAME, "r");
  file.seek(nr*42);
  file.readBytes(time, 19);
  file.read();
  file.readBytes(text, 21);
  file.close();
}

long getFreeSpace() {
  FSInfo fs_info;
  LittleFS.info(fs_info);
  return fs_info.totalBytes - fs_info.usedBytes;
}

void writeToSyslog(const char* msg)
{
  if (!LittleFS.exists(SYSLOG_FILE_NAME))
  {
    serialDebugP(F("creating syslog"));
    File create = LittleFS.open(SYSLOG_FILE_NAME, "w");
    create.close();
  } else {
    serialDebugP(F("syslog exists"));
  }

  long left = getFreeSpace();
  if (left >= STOP_SYSLOG_BELOW_MEMORY)
  {
    File file = LittleFS.open(SYSLOG_FILE_NAME, "a");

    //write datetimee
    readDateTime(buffer);
    buffer[19] = '\t';
    file.write(buffer,20);

    //write msg
    strcpy(buffer, msg);
    int l = strlen(buffer);
    buffer[l] = '\t';
    file.write(buffer, l+1);
    uint16_t vcc = ESP.getVcc();

    // write Vcc
    itoa(vcc,buffer,10);
    l = strlen(buffer);
    buffer[l] = '\t';
    file.write(buffer, l+1);

    //write free space
    ltoa(left,buffer,10);
    l = strlen(buffer);
    buffer[l] = '\t';
    file.write(buffer, l+1);

    //write heap
    itoa(ESP.getFreeHeap(),buffer,10);
    l = strlen(buffer);
    buffer[l] = '\n';
    file.write(buffer, l+1);

    file.close();
  }

  serialDebug(msg);
}

void welcome() {
  digitalWrite(POWER_PIN, LOW);
  delay(100);
  digitalWrite(POWER_PIN, HIGH);
  delay(10);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    writeToSyslog("OLED ERR");
  }

  bool got_to_admin = false;
  if (ADMIN_TRIGGERED_BY_BUTTONS_AT_START) {
    if (digitalRead(BUTTON_SELECT) == LOW && digitalRead(BUTTON_ENTER) == LOW) {
      got_to_admin = true;
    }
  }

  display.clearDisplay();
  display.display();
  writeToSyslog("ON");
  display.ssd1306_command(SSD1306_DISPLAYON);
  display.setRotation(SCREEN_ROTATION);   
  display.clearDisplay();
  display.
  display.drawBitmap(0, 0, welcome_logo, 64,64, WHITE);
  oledTextP(1, 64+32-7*6/2, 0,  F(WELCOME_LINE1)); 
  oledTextP(1, 64+32-7*6/2, 8,  F(WELCOME_LINE2)); 
  oledTextP(1, 64+32-7*6/2, 16, F(WELCOME_LINE3)); 
  oledTextP(1, 64+32-9*6/2, 24, F(WELCOME_LINE4)); 
  oledTextP(1, 64+32-8*6/2, 32, F(WELCOME_LINE5)); 
  oledTextP(1, 56, 40, F(WELCOME_LINE6));  
  oledTextP(1, 64+32-2*6/2, 48, F(WELCOME_LINE7)); 
  oledTextP(1, 128-11*6, 56, F(WELCOME_LINE8)); 
  display.display();
  waitUntilButtonOrTime(5000);
  initData();
  if (got_to_admin) {
    mode = MODE_ADMIN;
    position = 0;
  }
}

void goodbye() {
  stats.upseconds += (millis()/1000);
  saveStats();
  writeToSyslog("OFF");
  display.clearDisplay();
  display.drawBitmap(48, 16, sleep_logo, 32,32, WHITE);
  oledTextP(1, 4 ,F(SLEEP_LINE1)); 
  oledTextP(1, 52, F(SLEEP_LINE2)); 
  display.display();
  delay(2000);    
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  delay(10);
  Serial.end();

  pinMode(0, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(SDA, INPUT);
  pinMode(SCL, INPUT);  
  pinMode(13, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  digitalWrite(POWER_PIN, LOW);
  delay(5);
  ESP.deepSleep(0, RF_DISABLED);
}

void readButtons() {
  previous_button_select = button_select;
  previous_button_enter = button_enter;
  button_select = digitalRead(BUTTON_SELECT);
  button_enter = digitalRead(BUTTON_ENTER);

  select_pressed = previous_button_select == HIGH && button_select == LOW;
  enter_pressed = previous_button_enter == HIGH && button_enter == LOW;
  if (button_enter == LOW)
    last_activity = millis();  
  if (select_pressed || enter_pressed) {
    last_activity = millis();
    delay(5); //debounce
  }

  if (select_pressed)
    stats.select++;
  if (enter_pressed)
    stats.enter++;
}

void waitUntilButtonOrTime(int ms)
{
   int c = 0;
   do{
     delay(5);
     c+=5;
     readButtons();
   } while (c<ms && !(select_pressed || enter_pressed));
}

void advance()
{
  char c = input[position];
  if (input_type == INPUT_TYPE_UPPER) {
    if (c==' ')
      c = 'A';
    else if (c >='A' && c < 'Z')
      c = c+1;
    else 
      c = ' ';
  } 
  else if (input_type == INPUT_TYPE_LOWER)
  {
    if (c==' ')
      c = 'a';
    else if (c >='a' && c < 'z')
      c = c+1;
    else 
      c = ' ';
  } 
  else if (input_type == INPUT_TYPE_DIGITS)
  {
    if (c==' ')
      c = '0';
    else if (c >='0' && c < '9')
      c = c+1;
    else 
      c = ' ';
  }
  else
  {
    if (c == ' ')
      c = '.';
    else if (c == '.')
      c = ',';
    else if (c == ',')
      c = '!';
    else if (c == '!')
      c = '?';
    else if (c == '?')
      c = '-';
    else if (c == '-')
      c = '_';        
    else
      c = ' ';                
  }

  input[position] = c;
}

void clockAdjustment(uint8_t idx, short delta, uint8_t min, uint8_t max) {
  memcpy(buffer, input, 19);
  uint8_t val = (input[idx] - 48) * 10 + (input[idx+1] - 48);
  val += delta;
  if (val < min)
    val = min;
  if (val > max)
    val = max;
  
  buffer[idx] = 48 + val/10;
  buffer[idx+1] = 48 + val%10;
  buffer[10] = 'T';
  buffer[16] = ':';
  buffer[19] = 0;
  DateTime test_date(buffer);
  if (test_date.isValid()) {
    input[idx] = 48 + val/10;
    input[idx+1] = 48 + val%10;
  }
}

void startAccessPoint() {
  writeToSyslog("WIFI-START");
  display.clearDisplay();
  oledTextP(1, 28, F("Initializing WiFi..."));
  display.display();
  if (!WiFi.softAP(AP_WIFI_SSID, AP_WIFI_PASS))
    writeToSyslog("WIFI-ERR");
  IPAddress myIP = WiFi.softAPIP();
  server.begin();
  sprintf(buffer3, "%d.%d.%d.%d", myIP[0], myIP[1], myIP[2], myIP[3]);
  ArduinoOTA.onStart([]() {
    oledTextP(1, 32, 52, F("OTA Start   "));
    display.display();
    writeToSyslog("OTA-START");
  });

  ArduinoOTA.onEnd([]() {  
    oledTextP(1, 32, 52, F("OTA Done    ")); 
    display.display();
    writeToSyslog("OTA-DONE");
  });

  ArduinoOTA.onError([](ota_error_t error) {
    sprintf(buffer5, "OTA-ERROR %u ",error);
    oledText(1, 32, 52, buffer5);
    display.display();
    writeToSyslog(buffer5);
    ESP.restart();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    long percent = (progress * 100l) / total;
    sprintf(buffer5, "OTA %d%%    ", percent);
    oledText(1, 32, 52, buffer5);
    display.display();
  });  

  ArduinoOTA.begin();
}

void handleAccessPoint() {
  ArduinoOTA.handle();
  WiFiClient client = server.accept();
  if (!client)
    return;
  client.setTimeout(5000);  // default is 1000
  String req = client.readStringUntil('\r');
  if (req.indexOf(F(LOGBOOK_FILE_NAME)) != -1) {
    client.print(F("HTTP/1.1 200 OK\r\nContent-Disposition: attachment;filename=logbook.txt\r\n\r\n"));
    downloadFile(LOGBOOK_FILE_NAME, client);
  } else if (req.indexOf(F(STATS_FILE_NAME)) != -1) {-
    client.print(F("HTTP/1.1 200 OK\r\nContent-Disposition: attachment;filename=stats.dat\r\n\r\n"));
    downloadFile(STATS_FILE_NAME, client);
  } else if (req.indexOf(F(SYSLOG_FILE_NAME)) != -1) {
    client.print(F("HTTP/1.1 200 OK\r\nContent-Disposition: attachment;filename=syslog.txt\r\n\r\n"));
    downloadFile(SYSLOG_FILE_NAME, client);
  } else {
    client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>"));
    client.print(F("<head><title>Geocaching Logbook by PanJanek123</title></head><body>"));
    client.print(F("<h1>Geocaching Logbook</h1><br/>"));
    client.print(F("<h3>Download data page</h3><br/>"));
    client.print(F("<a href='" LOGBOOK_FILE_NAME "'>Download " LOGBOOK_FILE_NAME "</a><br/>"));
    client.print(F("<a href='" STATS_FILE_NAME "'>Download " STATS_FILE_NAME "</a><br/>"));
    client.print(F("<a href='" SYSLOG_FILE_NAME "'>Download " SYSLOG_FILE_NAME "</a><br/>"));
    client.print(F("</body></html>"));
  }
}

void downloadFile(const char* fileName, WiFiClient client)
{
  writeToSyslog("DOWNLOAD");
  File file = LittleFS.open(fileName, "r");
  int r;
  do {
    r = file.readBytes(buffer2, 20);
    client.write(buffer2, r);
  } while (r>0);
  file.close();
}

void stopAccessPoint() {
  display.clearDisplay();
  oledTextP(1, 28, F("Stopping WiFi..."));
  display.display();
  delay(500);
  ArduinoOTA.end();
  server.stop();
  server.close();
  WiFi.mode( WIFI_OFF );
  WiFi.forceSleepBegin();
  writeToSyslog("WIFI-STOP");
}

void navigate()
{
  if (mode == MODE_HOME) // ----------------------- main menu ------------------------
  {
    if (select_pressed) {
      position = (position+1)%3;
    } else if (enter_pressed) {
      if (position == POSITION_HOME_INPUT) {
        logbook_size = getLogbookSize();
        int left = getFreeSpace();
        if (left < 1000) {
          display.clearDisplay();
          oledTextP(1, 2, F("Logbook")); 
          oledTextP(2, 16, F("full!"));
          oledTextP(1, 40, F("author service")); 
          oledTextP(1, 48, F("required.")); 
          display.display();
          delay(2000);  
          mode = MODE_HOME;
          position = POSITION_HOME_BROWSE;
        } else {
          if (logbook_size < 3) {
            display.clearDisplay();
            oledTextP(4, 16, logbook_size == 0 ? F("FTF:)") : (logbook_size == 1 ? F("STF:)") : F("TTF:)")) );
            display.display();
            delay(1500); 
          }

          mode = MODE_INPUT;   // --> input
          input_type = INPUT_TYPE_UPPER;
          input_type_changed = false;
          memset(input, ' ', 21);
          input[21] = 0;
          beginning = true;
        }
        
      }
      else if (position == POSITION_HOME_BROWSE) { // --> browse
        logbook_size = getLogbookSize();
        if (logbook_size == 0) {
          display.clearDisplay();
          oledTextP(2, 24, F("Empty!")); 
          display.display();
          delay(2000);  
          mode = MODE_HOME;
          position = POSITION_HOME_INPUT;
        } else {
          mode = MODE_BROWSE;
          position = 0;
          beginning = true;
          scroll = logbook_size*16 - 64;
          if (scroll < 0)
            scroll = 0;
        }
      }
      else if (position == POSITION_HOME_END) {  // --> shutdown
        stats.manualoffs++;
        goodbye();
      }
    }
  } 
  else if (mode == MODE_INPUT) // ---------------------- input -----------------------
  {
      uint8_t inputLen = 20;
      while (inputLen>= 0 && input[inputLen] == ' ') inputLen--;
      inputLen++;
      if (select_pressed) {
        if (position == POSITION_INPUT_CHANGE && input_type_changed) {
          position = inputLen % 21;
        }
        else {
          if ((position < 20) && (position > 0) && input[position] == ' ' && input[position-1] == ' ') {
            position = POSITION_INPUT_CHANGE;
          } else if (position == 0 && input[position] == ' ') {
            position = POSITION_INPUT_CHANGE;
          } else {
            position = (position+1)%25;
          }
          
          if ((position == POSITION_INPUT_BACKSPACE && inputLen == 0) || (position == POSITION_INPUT_NEXT && inputLen < 3))
            position = POSITION_INPUT_CANCEL;

          if (position >= 1 && beginning && input_type == INPUT_TYPE_UPPER && input[0] >= 'A' && input[0] <= 'Z'){
            beginning = false;
            input_type = INPUT_TYPE_LOWER;
          }
        }

        input_type_changed = false;
      }
      else if (enter_pressed) {
        if (position<21) {  
          advance();
        } else if (position == POSITION_INPUT_CHANGE) {   // change input type
          input_type = (input_type+1)%4;
          beginning = false;
          input_type_changed = true;
        } else if (position == POSITION_INPUT_BACKSPACE) { // backspace
          position = inputLen-1;
          input[position] = ' '; 
        } else if (position == POSITION_INPUT_NEXT){   
          if (strlen_P(admin_password) == inputLen && memcmp_P(input, admin_password, strlen_P(admin_password)) == 0) {  // --> admin
            mode = MODE_ADMIN; 
            position = POSITION_ADMIN_STATS;
            writeToSyslog("ADMIN");
          } else {
            mode = MODE_CONFIRM;                // --> confirm
            position = POSITION_CONFIRM_BACK;
          }
        } else if (position == POSITION_INPUT_CANCEL) {  // --> home menu
          mode = MODE_HOME;
          position = POSITION_HOME_INPUT;
        }
      }
  }
  else if (mode == MODE_CONFIRM) // ---------------------- confirm -----------------------
  {
    if (select_pressed) {
      position = (position+1) % 2;
    }
    else if (enter_pressed) {
      if (position == POSITION_CONFIRM_BACK) {
        mode = MODE_INPUT; // --> input
      }
      else if (position == POSITION_CONFIRM_OK){  // --> save and back to home menu
        saveToLogbook();
        display.clearDisplay();
        oledTextP(2, 24, F(MSG_SAVED)); 
        display.display();
        delay(1000);  
        mode = MODE_HOME;
        position = POSITION_HOME_BROWSE;
      }
    }
  }
  else if (mode == MODE_BROWSE) // --------------------- browse --------------------
  {
    int maxScroll = logbook_size*16 - 64;
    if (maxScroll < 0)
      maxScroll = 0;

    if (select_pressed) {
      position = (position+1) % 5;
      beginning = false;
    }
    else if (enter_pressed) {
      beginning = false;
      if (position == POSITION_BROWSE_BACK) { // back to menu
        mode = MODE_HOME;  
        position = POSITION_HOME_BROWSE;
      }
    }

    if (button_enter == LOW && !beginning) {
      if (position == POSITION_BROWSE_UP) {
        scroll--;
        if (scroll < 0)
          scroll = 0;
      } else if (position == POSITION_BROWSE_DOWN) {
        scroll++;
        if (scroll > maxScroll)     
          scroll = maxScroll;    
      } else if (position == POSITION_BROWSE_TOP) { // top
        scroll = scroll - 64;
        if (scroll < 0)
          scroll = 0;
      } else if (position == POSITION_BROWSE_BOTTOM) { // bottom
        scroll = scroll + 64;
        if (scroll > maxScroll)
          scroll = maxScroll;    
      }
    }
  }
  else if (mode == MODE_ADMIN) {    // --------------------- admin -------------------
    if (select_pressed) {
      position = (position+1) % 6;
    }
    else if (enter_pressed) {
      if (position == POSITION_ADMIN_STATS) { // --> stats
        mode = MODE_STATS;
        position = POSITION_STATS_BACK;
      } else if (position == POSITION_ADMIN_SYSINFO) { // -->sysinfo
        mode = MODE_SYSINFO;
        position = POSITION_SYSINFO_BACK;
      } else if (position == POSITION_ADMIN_CLOCK) { // --> clock
        readDateTime(input);
        input[16] = 0;
        mode = MODE_CLOCK;
        position = 0;
      } else if (position == POSITION_ADMIN_CLEAR) { // --> clear 
        mode = MODE_CLEAR;
        position = POSITION_CLEAR_CANCEL;
        input[0] = 0;
        input[1] = 0;
        input[2] = 0;
      } else if (position == POSITION_ADMIN_TX) {  // --> tx
        mode = MODE_TX;
        startAccessPoint();
      } else if (position == POSITION_ADMIN_BACK) { // --> home menu
        mode = MODE_HOME;
        position = POSITION_HOME_INPUT;
      }
    }
  } 
  else if (mode == MODE_STATS) { // ------------- stats ----------------
    if (enter_pressed) {
      mode = MODE_ADMIN;
      position = POSITION_ADMIN_STATS;
    }
  } else if (mode == MODE_SYSINFO) { // -------------- sysinfo -----------
    if (select_pressed) {
      position = (position+1)%2;
    } else if (enter_pressed) {
      if (position == 0) { // --> admin
        mode = MODE_ADMIN;
        position = POSITION_ADMIN_SYSINFO;        
      } else if (position == 1) {
        mode = MODE_SYSINFO2;
        position = POSITION_SYSINFO_BACK;
      }
    }
  } else if (mode == MODE_SYSINFO2) { // -------------- sysinfo -----------
    if (select_pressed) {
      position = (position+1)%2;
    } else if (enter_pressed) {
      if (position == 0) { // --> sysinfo 1
        mode = MODE_SYSINFO;
        position = POSITION_SYSINFO_BACK;        
      } else if (position == 1) {
        mode = MODE_ADMIN;
        position = POSITION_ADMIN_SYSINFO; 
      }
    }
  } else if (mode == MODE_CLOCK) { // -------------- clock -----------
    if (select_pressed) {
      position = (position+1)%12;
    } else if (enter_pressed) {
      if (position == POSITION_CLOCK_YEARUP) {
        clockAdjustment(2,1,1,99);
      } else if (position == POSITION_CLOCK_YEARDOWN) {
        clockAdjustment(2,-1,1,99);
      } else if (position == POSITION_CLOCK_MONTHUP) {
        clockAdjustment(5,1,1,12);
      } else if (position == POSITION_CLOCK_MONTHDOWN) {
        clockAdjustment(5,-1,1,12);
      } else if (position == POSITION_CLOCK_DAYUP) {
        clockAdjustment(8,1,1,31);   //todo 
      } else if (position == POSITION_CLOCK_DAYDOWN) {
        clockAdjustment(8,-1,1,31); //todo
      } else if (position == POSITION_CLOCK_HOURUP) {
        clockAdjustment(11,1,0,23);
      } else if (position == POSITION_CLOCK_HOURDOWN) {
        clockAdjustment(11,-1,0,23);
      } else if (position == POSITION_CLOCK_MINUTEUP) {
        clockAdjustment(14,1,0,59);
      } else if (position == POSITION_CLOCK_MINUTEDOWN) {
        clockAdjustment(14,-1,0,59);
      } else if (position == POSITION_CLOCK_CANCEL) {
        mode = MODE_ADMIN;
        position = POSITION_ADMIN_CLOCK;         
      } else if (position == POSITION_CLOCK_SET) {
        input[10] = 'T';
        input[16] = ':';
        input[17] = '0';
        input[18] = '0';
        input[19] = 0;
        DateTime new_time(input);
        rtc.adjust(new_time);
        display.clearDisplay();
        oledTextP(2, 24, F("OK!")); 
        display.display();
        delay(1000);  
        writeToSyslog("SETCLOCK");
        mode = MODE_ADMIN;
        position = POSITION_ADMIN_CLOCK;         
      }
    }
  } else if (mode == MODE_CLEAR) { // ----------- clear data ---------------
    if (select_pressed) {
      position = (position+1)%5;
      if (input[0] == 0 && input[1] == 0  && input[2] == 0 && position == POSITION_CLEAR_CONFIRM)
        position = POSITION_CLEAR_CANCEL;
    } else if (enter_pressed) {
      if (position == POSITION_CLEAR_CANCEL) {
        mode = MODE_ADMIN;
        position = POSITION_ADMIN_CLEAR;       
      } else if (position == POSITION_CLEAR_LOGBOOK) {
        input[0] = !input[0];
      } else if (position == POSITION_CLEAR_STATS) {
        input[1] = !input[1];
      } else if (position == POSITION_CLEAR_SYSLOG) {
        input[2] = !input[2];        
      } else if (position == POSITION_CLEAR_CONFIRM) {
        if (input[0] != 0) {
          LittleFS.remove(LOGBOOK_FILE_NAME);
          writeToSyslog("CLEARLOGBOOK");
        }
        if (input[1] != 0) {
          LittleFS.remove(STATS_FILE_NAME);
          writeToSyslog("CLEARSTATS");
          readStats(); 
        }
        if (input[2] != 0) {
          LittleFS.remove(SYSLOG_FILE_NAME);
        }        
        display.clearDisplay();
        oledTextP(2, 24, F("Deleted")); 
        display.display();
        delay(1000);          
        mode = MODE_ADMIN;
        position = POSITION_ADMIN_BACK;        
      }
    }
  } else if (mode == MODE_TX) {   // ----------- tx -------------
    if (enter_pressed) {
      stopAccessPoint();
      mode = MODE_ADMIN;
      position = POSITION_ADMIN_TX;
    }
  }
}

void show()
{
  display.clearDisplay();
  if (mode == MODE_HOME) // --------------------------------- HOME MENU ------------------------------
  {
    oledTextP(1, 2, F("Menu"));
    oledTextP(1, 18, F(MENU_LOG), position==POSITION_HOME_INPUT, true);
    oledTextP(1, 32, F(MENU_BROWSE), position==POSITION_HOME_BROWSE, true);
    oledTextP(1, 46, F(MENU_FINISH), position==POSITION_HOME_END, true);
  } else if (mode == MODE_INPUT) { // ----------------------------- INPUT -------------------------------
    if (logbook_size == -1)
      logbook_size = getLogbookSize();
    readDateTime(buffer);
    buffer[10] = 0;
    oledText(1, 67, 0, buffer);
    oledTextP(1, 0, 0, F("Nr "));
    itoa(logbook_size + 1, buffer, 10);
    oledText(1, 18, 0, buffer);
    oledTextP(1, 0, 8, F(MSG_ENTER_NICK));

    if (logbook_size == 0)
      oledTextP(1, 103, 8, F("FTF!"));
    else if (logbook_size == 1)
      oledTextP(1, 103, 8, F("STF!"));
    else if (logbook_size == 2)
      oledTextP(1, 103, 8, F("TTF!"));

    for(int i=0; i<21; i++) {    
      bool invert = position == i && (millis() % 300 < 150); 
      display.setCursor(i*6, 20);
      display.setTextColor(invert ? BLACK : WHITE, invert ? WHITE : BLACK);
      display.print(input[i]);
      display.drawLine(i*6+1, 28, i*6+4, 28, WHITE);
    }

    oledTextP(1, 4, 36, input_type == INPUT_TYPE_UPPER ? F("ABC") : (input_type == INPUT_TYPE_LOWER ? F("abc") : (input_type == INPUT_TYPE_DIGITS ? F("123") : F("!-?"))), position==POSITION_INPUT_CHANGE, true);
    oledTextP(1, 28, 36, F("   "), position==POSITION_INPUT_BACKSPACE, true);
    display.drawBitmap(30, 36, backspace_icon, 16, 8, position == POSITION_INPUT_BACKSPACE ? BLACK : WHITE);
    oledTextP(1, 94, 36, F(BUTTON_CONTINUE), position==POSITION_INPUT_NEXT, true);  
    oledTextP(1, 4, 50, F(BUTTON_CANCEL), position==POSITION_INPUT_CANCEL, true);    
  }
  else if (mode == MODE_CONFIRM){ // ---------------------------------- confirm ------------------------------
    oledTextP(1, 0, F(CONFIRM_LINE1));
    oledTextP(1, 8, F(CONFIRM_LINE2));
    oledTextP(1, 16, F(CONFIRM_LINE3));
    oledTextP(1, 24, F(CONFIRM_LINE4));
    for(int i=0; i<21; i++) {    
      display.setCursor(i*6, 37);
      display.print(input[i]);
      display.drawLine(i*6+1, 45, i*6+4, 45, WHITE);
    }
    oledTextP(1, 2, 50, F(BUTTON_BACK), position==POSITION_CONFIRM_BACK, true);
    oledTextP(1, 87, 50, F(BUTTON_SAVE), position==POSITION_CONFIRM_OK, true);
  }
  else if (mode == MODE_BROWSE) // -------------------------------------- browse ----------------------------------
  {
    for(int i=0; i<6; i++) {
      int idx = (i-1)+(scroll/16);
      if (idx>=0 && idx < logbook_size)
      {
        int y = (i-1)*16-(scroll%16);
        oledTextP(1, 0, y, F("Nr "));
        itoa(idx+1, buffer, 10);
        int l = strlen(buffer);
        buffer[l] = ':';
        buffer[l+1] = 0;
        oledText(1, 18, y, buffer);
        if (idx == 0)
          oledTextP(1, 18+l*6+6, y, F("(FTF)"));
        else if (idx==1)
          oledTextP(1, 18+l*6+6, y, F("(STF)"));
        else if (idx==2)
          oledTextP(1, 18+l*6+6, y, F("(TTF)"));          
        readLogbook(idx, buffer, buffer2);
        buffer[10]=0;
        buffer2[21]=0;
        oledText(1, 67, y, buffer);
        oledText(1, 0, y + 8, buffer2);
      }
    }

    if (millis() - last_activity < 3000) // hide buttons after 3s
    {
      display.fillRect(100, 0, 28, 13, BLACK);
      display.fillRect(82, 50, 46, 14, BLACK);
      oledIcon(117, 2, up_icon, 8, 8, position == POSITION_BROWSE_UP, true);
      oledIcon(117, 53, down_icon, 8, 8,  position == POSITION_BROWSE_DOWN, true);
      oledIcon(103, 2, upup_icon, 8, 8,  position == POSITION_BROWSE_TOP, true);
      oledIcon(103, 53, downdown_icon, 8, 8,  position == POSITION_BROWSE_BOTTOM, true);
      oledTextP(1, 85, 53, F("<-"), position==POSITION_BROWSE_BACK, true);
      if (logbook_size > 4 && button_enter == LOW && !beginning) { //scroll bar
        display.fillRect(117, 14, 8, 36, BLACK);
        display.drawRect(117, 14, 8, 36, WHITE);
        int h = 34*4 / logbook_size;
        int y = ((34 - h) * scroll / (logbook_size*16-64));
        if (y < 0)
          y = 0;
        if (y > 34-h)
          y = 34-h;
        display.fillRect(117, 15+y, 8, h, WHITE);
      }
    }
  } else if (mode == MODE_ADMIN) // --------------------------------- ADMINISTRATOR MENU ------------------------------
  {
    oledTextP(1, 2, F("Administrator mode"));
    oledTextP(1, 7, 18, F("stats"), position==POSITION_ADMIN_STATS, true);
    oledTextP(1, 50, 18, F("system info"), position==POSITION_ADMIN_SYSINFO, true);
    oledTextP(1, 6, 32, F("clock"), position==POSITION_ADMIN_CLOCK, true);
    oledTextP(1, 49, 32, F("wifi"), position==POSITION_ADMIN_TX, true);
    oledTextP(1, 86, 32, F("clear"), position==POSITION_ADMIN_CLEAR, true);
    oledTextP(1, 46, F("home"), position==POSITION_ADMIN_BACK, true);
  } else if (mode == MODE_STATS)  // --------------- stats --------------------
  {
    oledLabelAndValue(0,  F("power ups  :"), stats.power);
    oledLabelAndValue(8,  F("seconds up :"), stats.upseconds);
    oledLabelAndValue(16, F("selects    :"), stats.select);
    oledLabelAndValue(24, F("enters     :"), stats.enter);
    oledLabelAndValue(32, F("manual offs:"), stats.manualoffs);
    oledLabelAndValue(40, F("logged     :"), getLogbookSize());
    oledTextP(1, 2, 52, F("back"), position==POSITION_STATS_BACK, true);  
  } else if (mode == MODE_SYSINFO)  // --------------- system info --------------------
  {
    uint16_t vcc = ESP.getVcc();
    itoa(vcc,buffer,10);
    buffer[4]=buffer[3];
    buffer[3]=buffer[2];
    buffer[2]=buffer[1];
    buffer[1]='.';
    buffer[5]='v';
    buffer[6]=0;
    oledLabelAndValue(0,  F("Vcc         :"), buffer);
    FSInfo fs_info;;
    LittleFS.info(fs_info);
    oledLabelAndValue(8,  F("CPU MHz     :"), ESP.getCpuFreqMHz());
    oledLabelAndValue(16, F("ESP core ver:"), ESP.getCoreVersion().c_str());
    oledLabelAndValue(24, F("ESP ChipId  :"), ESP.getChipId());
    oledLabelAndValue(32, F("FS total [b]:"), fs_info.totalBytes);
    oledLabelAndValue(40, F("FS used [b] :"), fs_info.usedBytes);
    oledTextP(1, 2, 52, F("back"), position==POSITION_SYSINFO_BACK, true);  
    oledTextP(1, 99, 52, F("next"), position==POSITION_SYSINFO_NEXT, true);  
  } else if (mode == MODE_SYSINFO2)  // --------------- system info --------------------
  {
    oledLabelAndValue(0,  F("Boot ver.   :"), ESP.getBootVersion());
    oledLabelAndValue(8,  F("Free fs     :"), getFreeSpace());
    oledLabelAndValue(16, F("Free heap   :"), ESP.getFreeHeap());
    oledLabelAndValue(24, F("Logbook ver.:"), APP_VERSION); 
    oledLabelAndValue(32, F("Sketch size :"), ESP.getSketchSize());
    oledLabelAndValue(40, F("Free sketch :"), ESP.getFreeSketchSpace());
    oledTextP(1, 2, 52, F("previous"), position==POSITION_SYSINFO_BACK, true);  
    oledTextP(1, 111, 52, F("ok"), position==POSITION_SYSINFO_NEXT, true);  
  } else if (mode == MODE_CLOCK){ // ------------------- adjust clock ---------------------------
    oledTextP(1, 0, F("Adjust date and time"));
    oledText(1, 24, input);
    oledIcon(16 + 6 + 2, 12, up_icon, 8, 8,  position == POSITION_CLOCK_YEARUP, true);
    oledIcon(16 + 6 + 2, 35, down_icon, 8, 8,  position == POSITION_CLOCK_YEARDOWN, true);
    oledIcon(16 + 30 + 2, 12, up_icon, 8, 8,  position == POSITION_CLOCK_MONTHUP, true);
    oledIcon(16 + 30 + 2, 35, down_icon, 8, 8,  position == POSITION_CLOCK_MONTHDOWN, true);
    oledIcon(16 + 48 + 2, 12, up_icon, 8, 8,  position == POSITION_CLOCK_DAYUP, true);
    oledIcon(16 + 48 + 2, 35, down_icon, 8, 8,  position == POSITION_CLOCK_DAYDOWN, true);
    oledIcon(16 + 66 + 2, 12, up_icon, 8, 8,  position == POSITION_CLOCK_HOURUP, true);
    oledIcon(16 + 66 + 2, 35, down_icon, 8, 8,  position == POSITION_CLOCK_HOURDOWN, true);
    oledIcon(16 + 84 + 2, 12, up_icon, 8, 8,  position == POSITION_CLOCK_MINUTEUP, true);
    oledIcon(16 + 84 + 2, 35, down_icon, 8, 8,  position == POSITION_CLOCK_MINUTEDOWN, true);
    oledTextP(1, 2, 52, F("cancel"), position==POSITION_CLOCK_CANCEL, true);
    oledTextP(1, 105, 52, F("set"), position==POSITION_CLOCK_SET, true);
  } else if (mode==MODE_CLEAR) {  //------------------- clear data -------------------------
    oledTextP(1, 0, F("Clear stored data"));
    oledTextP(1, 32, 10, input[0] == 0 ? F(" ") : F("x"), position == POSITION_CLEAR_LOGBOOK, true);
    oledTextP(1, 44, 10, F("logbook"), false, false);
    oledTextP(1, 32, 24, input[1] == 0 ? F(" ") : F("x"), position == POSITION_CLEAR_STATS, true);
    oledTextP(1, 44, 24, F("stats"), false, false);
    oledTextP(1, 32, 38, input[2] == 0 ? F(" ") : F("x"), position == POSITION_CLEAR_SYSLOG, true);
    oledTextP(1, 44, 38, F("syslog"), false, false);  
    oledTextP(1, 2, 52, F("cancel"), position==POSITION_CLEAR_CANCEL, true);
    oledTextP(1, 81, 52, F("confirm"), position==POSITION_CLEAR_CONFIRM, true);
  } else if (mode==MODE_TX) { // --------------- enable download by wifi -----------------------
    oledTextP(1, 0, F("Download/OTA by WiFi"));
    oledTextP(1, 0, 18, F("WiFi: " AP_WIFI_SSID));
    oledTextP(1, 0, 26, F("Pass: " AP_WIFI_PASS));
    oledTextP(1, 0, 34, F("Web :"));
    oledText(1, 36, 34, buffer3);
    oledTextP(1, 2, 52, F("exit"), true, true);        
    handleAccessPoint();
  }

  int sleepAfterSeconds = mode==MODE_TX ? SLEEP_AFTER_SECONDS * 5 : SLEEP_AFTER_SECONDS;
  int ttl128 = (sleepAfterSeconds * 1000 - (millis() - last_activity)) * 128 / (sleepAfterSeconds * 1000);
  display.drawLine(0,63,ttl128,63,WHITE);
  display.display();
}

void loop() {
  readButtons();
  navigate();
  show();

  int sleepAfterSeconds = mode==MODE_TX ? SLEEP_AFTER_SECONDS * 5 : SLEEP_AFTER_SECONDS;
  if (millis() - last_activity > sleepAfterSeconds * 1000) {
    writeToSyslog("AUTO-SLEEP");
    goodbye();
  }

  if (cycles%5 == 0 && cycles < 10000 && mode != MODE_TX && low_battery != -1)
  {
    if (ESP.getVcc() < LOW_BATTERY_TRESHOLD)
    low_battery++;
    if (low_battery > 25) 
    {
      low_battery = -1;
      writeToSyslog("LOW-BAT");
      display.clearDisplay();
      oledTextP(1, 2, F("Battery")); 
      oledTextP(2, 16, F("low!"));
      oledTextP(1, 40, F("Potrzebny serwis")); 
      oledTextP(1, 48, F("autora.")); 
      display.display();
      delay(2000);  
    }
  }

  delay(5);
  cycles++;
  if (cycles % 100 == 0)
    serialDebugP(F("ok"));
}
