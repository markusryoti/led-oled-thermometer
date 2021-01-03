// I2C OLED 0.96″ 128×64
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>

// Oled inits ===================================================================
// ==============================================================================
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// ==============================================================================

// DS18B20 inits ================================================================
// ==============================================================================
#define ONE_WIRE_BUS 2

// Create a new instance of the oneWire class to communicate with any OneWire device:
OneWire oneWire(ONE_WIRE_BUS);

// Pass the oneWire reference to DallasTemperature library:
DallasTemperature sensors(&oneWire);

DeviceAddress temp_sensor_address;
// ==============================================================================

// LED inits ================================================================
// ==============================================================================
#define RED_PIN 11
#define GREEN_PIN 10
#define BLUE_PIN 9
#define TEMP_MIN 16.0f
#define TEMP_MAX 30.0f

// ==============================================================================

void print_serial(float temp)
{
  // Print the temperature in Celsius in the Serial Monitor:
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" \xC2\xB0"); // shows degree symbol
  Serial.print("C\n");
}

void print_text(String text = "Init")
{
  display.clearDisplay();
  display.setTextSize(2);      // The fontsize
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(20, 30);   // Start at top-left corner
  display.print(text);         //the text
  display.display();           //call to display
}

void set_led(float temp)
{
  float temp_diff = TEMP_MAX - TEMP_MIN;

  if (temp >= TEMP_MAX)
    temp = TEMP_MAX;

  if (temp <= TEMP_MIN)
    temp = TEMP_MIN;

  float scaled_temp = (temp - TEMP_MIN) / (TEMP_MAX - TEMP_MIN) * temp_diff;

  if (scaled_temp <= 0)
  {
    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 255);
    return;
  }

  // Linear change
  // int red_value = (int)(255.0 / temp_diff * scaled_temp + 0);
  // int blue_value = (int)(-255.0 / temp_diff * scaled_temp + 255);

  // Logarithmic change
  double base = pow(temp_diff, (1.0 / 255.0));

  int red_value = (int)(log(scaled_temp) / log(base));
  int blue_value = (int)(abs(log(scaled_temp) / log(base) - 255.0));

  // Serial.println(scaled_temp);
  // Serial.println(red_value);
  // Serial.println(blue_value);

  analogWrite(RED_PIN, red_value);
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN, blue_value);
}

void setup()
{
  Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  // Clear the buffer
  display.clearDisplay();
  print_text();
  delay(1500);

  sensors.begin();
  if (!sensors.getAddress(temp_sensor_address, 0))
    Serial.println("Unable to find address for Device 0");
  delay(1500);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

void loop()
{
  // Send the command for all devices on the bus to perform a temperature conversion:
  sensors.requestTemperatures();

  // Fetch the temperature in degrees Celsius for device index:
  float tempC = sensors.getTempCByIndex(0); // the index 0 refers to the first device
  String tempStr = String(tempC, 1) + " C";

  print_serial(tempC);
  print_text(tempStr);
  set_led(tempC);

  delay(1000);
}
