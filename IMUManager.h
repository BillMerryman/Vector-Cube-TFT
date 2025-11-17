#include <Arduino_LSM6DS3.h>

class IMUManager
{
  public:
  
    enum SITTING_ON
    {
      BOTTOM, 
      TOP,
      LEFT, 
      RIGHT, 
      BACK, 
      FRONT
    };

    IMUManager(LSM6DS3Class& _IMU, unsigned long _pollFrequency, unsigned long currentTime);
    void setPollFrequency(unsigned long _pollFrequency);
    void poll(unsigned long currentTime);
    SITTING_ON SittingOn();
    bool isShaken();
    float GetXAcceleration();
    float GetYAcceleration();
    float GetZAcceleration();
    float GetXAngular();
    float GetYAngular();
    float GetZAngular();
    float GetTemperature();

  protected:
    LSM6DS3Class& IMU;
    unsigned long pollFrequency;
    unsigned long nextPollTime;
    float shakeForce;
    uint8_t shakeCount;
    uint8_t shakeThreshold;
    float xAccel;
    float yAccel;
    float zAccel;
    float xAngular;
    float yAngular;
    float zAngular;
    float temperature;
};