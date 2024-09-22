#include <Arduino_LSM6DS3.h>

class IMUManager
{
  public:
    IMUManager(LSM6DS3Class& _IMU, unsigned long _pollFrequency, unsigned long currentTime);
    void setPollFrequency(unsigned long _pollFrequency);
    void poll(unsigned long currentTime);
    bool isShaken();
    float GetXAcceleration();
    float GetYAcceleration();
    float GetZAcceleration();
    float GetXAngular();
    float GetYAngular();
    float GetZAngular();

  protected:
    LSM6DS3Class& IMU;
    unsigned long nextPollTime;
    unsigned long pollFrequency;
    float shakeForce;
    uint8_t shakeCount;
    uint8_t shakeThreshold;
    float xAccel;
    float yAccel;
    float zAccel;
    float xAngular;
    float yAngular;
    float zAngular;
};