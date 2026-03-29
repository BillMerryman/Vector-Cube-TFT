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

//set up your own secrets.h file that creates defines for
//your router SSID and password. You can use the 
//secrets.h.example file as a template
#include "secrets.h"
const char* ssid     = SSID;
const char* password = PASS;

#define SERIAL_SPEED          9600

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 canvas = GFXcanvas16(SCREEN_WIDTH, SCREEN_HEIGHT);
AnimationPlayer animationPlayer = AnimationPlayer(tft, canvas);
byte *animation;
int animationSize;
WiFiServer server(TCP_PORT);
WiFiClient imageStream;
Adafruit_NeoPixel pixels(NUMBER_OF_LEDS, LEDS_DATA_PIN, NEO_GRB + NEO_KHZ800);
IMUManager imuManager(IMU, 50, 0);

void setup(void)
{
  Serial.begin(SERIAL_SPEED);
  Serial.println("Vector Cube Startup.\n");
  initializeTFT();
  initializeLEDs();
  initializeWiFi();
  initializeIMU();
  Serial.println("Init complete.");
  tft.println("Init complete.");

  delay(2000);

  canvas.setTextSize(4);
  canvas.setTextColor(0Xffff);
  canvas.setCursor(0, 0);
  animationPlayer.start(animation_blink, millis());
  animation = (byte *)malloc(sizeof(int));
  animationSize = sizeof(int);
}

void loop()
{

  unsigned long loopTime = millis();
  //Serial.println(loopTime); Last check this loop is executing 15 times per milli WITH the serial.println...

  imuManager.poll(loopTime);
  if(imuManager.isShaken()) LEDCycle();
  float zAccel = imuManager.GetZAcceleration();
  if(zAccel > .95 && zAccel < 1.5)
  {
    tft.setRotation(ROTATION + 2);
    canvas.fillScreen(0);
    canvas.setCursor(0, 0);
    canvas.print(imuManager.GetTemperature());
    tft.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT);
    tft.setRotation(ROTATION);
    return;
  }

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
        if(bytewiseReceive((uint8_t *)animation, 4))
        {
          uint32_t incoming_size = *(uint32_t *)animation;
          if(incoming_size > animationSize)
          {
            uint8_t *new_buf = (byte *)realloc(animation, 4 + incoming_size);
            if (!new_buf) break;
            animation = new_buf;
            animationSize = incoming_size;
          }
          if(bytewiseReceive((uint8_t *)(animation + 4), incoming_size))
          {
            //TODO: Call the animation validation here before conditionally starting the animation
            //Note: the difference I may be seeing with single frames displaying shorter than frame duration
            //could be because I am passing 'loopTime' instead of 'millis()', and the frame received code could
            //be eating up a lot of the time between setting 'loopTime' and this point in the code.
            //Needs further investigation... 
            animationPlayer.start(animation, loopTime);
            Serial.println("Animation received successfully.\n");
          }
          else
          {
            Serial.println("Animation receive failed.\n");
          }
        }
        else
        {
          Serial.println("Animation length receive failed.\n");
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
  tft.println("Vector Cube Startup.");
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

void LEDCycle()
{
  uint8_t red = 255, green = 255, blue = 255;

  switch(esp_random() % 7)
  {
    case 0:
      //white
      break;
    case 1:
      //red
      green = blue = 0;
      break;
    case 2:
      //green
      blue = red = 0;
      break;
    case 3:
      //blue
      red = green = 0;
      break;
    case 4:
      //yellow
      blue = 0;
      break;
    case 5:
      //bluish green
      red = 0;
      break;
    case 6:
      //purple
      green = 0;
      break;
  }

  for(int duty = 100; duty > 0; duty-=3)
  {
    for(int loopCounter = 0; loopCounter < 4; loopCounter++)
    {
      for(int pixelCounter = 0; pixelCounter < 4; pixelCounter++)
      {
        pixels.setPixelColor(pixelCounter, (pixelCounter==loopCounter)?pixels.Color(red, green, blue):pixels.Color(0, 0, 0));
      }
      pixels.show();
      delay(duty);
    }
  }
  pixels.clear();
  pixels.show();
}