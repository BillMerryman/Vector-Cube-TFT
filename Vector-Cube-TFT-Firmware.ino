#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino_LSM6DS3.h>
#include "WiFi.h"
#include "TFT.h"
#include "LED.h"
#include "TCP.h"
#include "Protocol.h"
#include "AnimationPlayer.h"
#include "IMUManager.h"
#include "blink.h"
#include "lookleft.h"
#include "lookright.h"

const char* ssid     = "DieFamilieMerryman2.4G";
const char* password = "020805032103041977teddie";

#define SERIAL_SPEED          9600

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
AnimationPlayer animationPlayer = AnimationPlayer(tft);
Animation animation;
GFXcanvas16 canvas = GFXcanvas16(SCREEN_WIDTH, SCREEN_HEIGHT);
WiFiServer server(TCP_PORT);
WiFiClient imageStream;
Adafruit_NeoPixel pixels(NUMBER_OF_LEDS, LEDS_DATA_PIN, NEO_GRB + NEO_KHZ800);
IMUManager imuManager(IMU, 50, 0);

void setup(void)
{
  Serial.begin(SERIAL_SPEED);
  Serial.println("Vector Cube Startup.\n");
  initializeTFT();
  tft.println("Vector Cube Startup.\n");
  initializeLEDs();
  initializeWiFi();
  initializeIMU();
  Serial.println("Init complete.");
  tft.println("Init complete.");

  char buf [10];
  sprintf (buf, "%d", sizeof(Animation));
  tft.println(buf);
  delay(5000);
  tft.fillScreen(ST77XX_BLACK);

  animationPlayer.start(animation_blink, millis());
}

void loop()
{

  unsigned long loopTime = millis();

  switch(random(100000))
  {
    case 1:
      animationPlayer.start(animation_blink, loopTime);
      break;
    case 2:
      animationPlayer.start(animation_lookleft, loopTime);
      break;
    case 3:
      animationPlayer.start(animation_lookright, loopTime);
      break;
  }

  imageStream = server.available();
  if(imageStream)
  {
    TransmissionType transmissionType = getTransmissionType();
    switch(transmissionType){
      case _Animation:
        if(bytewiseReceive((uint8_t *)&animation, sizeof(animation)))
        {
          animationPlayer.start((uint8_t *)&animation, loopTime);
          Serial.println("Animation received successfully.\n");
        }
        else
        {
          Serial.println("Animation receive failed.\n");
        }
        break;
      case _Image:
        uint8_t * canvasPointer;
        canvasPointer = (uint8_t *)canvas.getBuffer();
        if(bytewiseReceive(canvasPointer, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t)))
        {
          tft.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT);
          Serial.println("Image received successfully.\n");
          delay(5000);
        }
        else
        {
          Serial.println("Image receive failed.\n");
        }
        break;
      default:
        break;
    }
    imageStream.flush();
  }
  animationPlayer.update(loopTime);
  imuManager.poll(loopTime);
  if(imuManager.isShaken()) testLEDs();
}

void initializeTFT(void)
{
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);
  tft.init(SCREEN_WIDTH, SCREEN_HEIGHT);
  tft.setSPISpeed(SPI_SPEED);
  tft.setRotation(ROTATION);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);

  Serial.println("TFT initialized.");
  tft.println("TFT initialized.");
}

void initializeLEDs(void)
{
  pixels.begin();
  pixels.clear();
  pixels.show();
  Serial.println("LEDs initialized.");
  tft.println("LEDs initialized.");
}

void initializeWiFi(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  tft.println("Connecting to: ");
  tft.println(ssid);
  while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      tft.print(".");
  }
  Serial.println("");
  tft.println("");
  Serial.println("WiFi connected!");
  tft.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  tft.println("IP address: ");
  tft.println(WiFi.localIP());
  server.begin();
  Serial.println("WiFi initialized.");
  tft.println("WiFi initialized.");
}

void initializeIMU()
{
  if (!IMU.begin()) 
  {
    Serial.println("Failed to initialize IMU!");
    tft.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.println("IMU initialized.");
  tft.println("IMU initialized.");
}

bool bytewiseReceive(uint8_t *inputBuffer, size_t size)
{
  unsigned long startTime;

  for(int index = 0; index < size; index++)
  {
    startTime = millis();
    while(!imageStream.available())
    {
      if(millis() - startTime > TCP_TIMEOUT)
      {
        imageStream.flush();
        return false;
      }
    }
    imageStream.read(inputBuffer + index, 1);
  }
  return true;
}

TransmissionType getTransmissionType()
{  
  return (TransmissionType)getUint16tFromStream();
}

uint16_t getUint16tFromStream()
{
  while(!imageStream.available());
  uint16_t lowerByte = (uint16_t)imageStream.read();
  while(!imageStream.available());
  uint16_t upperByte = (uint16_t)imageStream.read() << 8;
  return upperByte + lowerByte;
}

void testLEDs()
{
  for(int rTog = 0; rTog < 256; rTog++)
  {
    for(int counter = 0; counter < 4; counter ++)
    {
      pixels.setPixelColor(counter, pixels.Color(rTog, 255 - rTog, 0));
    }
    pixels.show();
    delay(10);
  }
  for(int gTob = 0; gTob < 256; gTob++)
  {
    for(int counter = 0; counter < 4; counter ++)
    {
      pixels.setPixelColor(counter, pixels.Color(255 - gTob, 0, gTob));
    }
    pixels.show();
    delay(10);
  }
  for(int bTor = 0; bTor < 256; bTor++)
  {
    for(int counter = 0; counter < 4; counter ++)
    {
      pixels.setPixelColor(counter, pixels.Color(0, bTor, 255 - bTor));
    }
    pixels.show();
    delay(10);
  }
  for(int counter = 0; counter < 4; counter ++)
  {
    pixels.setPixelColor(counter, pixels.Color(0, 0, 0));
  }
  pixels.show();
  delay(1000);
  for(int counter = 0; counter < 4; counter ++)
  {
    pixels.setPixelColor(counter, pixels.Color(0, 255, 0));
  }
  pixels.show();
  delay(1000);
  for(int counter = 0; counter < 4; counter ++)
  {
    pixels.setPixelColor(counter, pixels.Color(255, 255, 0));
  }
  pixels.show();
  delay(1000);
  for(int counter = 0; counter < 4; counter ++)
  {
    pixels.setPixelColor(counter, pixels.Color(255, 0, 0));
  }
  pixels.show();
  delay(1000);
  for(int counter = 0; counter < 4; counter ++)
  {
    pixels.setPixelColor(counter, pixels.Color(255, 0, 255));
  }
  pixels.show();
  delay(1000);
  for(int counter = 0; counter < 4; counter ++)
  {
    pixels.setPixelColor(counter, pixels.Color(0, 0, 255));
  }
  pixels.show();
  delay(1000);
  for(int counter = 0; counter < 4; counter ++)
  {
    pixels.setPixelColor(counter, pixels.Color(0, 255, 255));
  }
  pixels.show();
  delay(1000);
  for(int counter = 0; counter < 4; counter ++)
  {
    pixels.setPixelColor(counter, pixels.Color(255, 255, 255));
  }
  pixels.show();
  delay(1000);
  pixels.clear();
  pixels.show();
}