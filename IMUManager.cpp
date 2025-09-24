#include "IMUManager.h"

IMUManager::IMUManager(LSM6DS3Class& _IMU, unsigned long _pollFrequency, unsigned long currentTime):IMU(_IMU), pollFrequency(_pollFrequency)
{
  shakeCount = 0;
  shakeForce = 2.5;
  shakeThreshold = 3;
  nextPollTime = currentTime + pollFrequency;
}

void IMUManager::poll(unsigned long currentTime)
{
  if(currentTime < nextPollTime) return;
  nextPollTime = currentTime + pollFrequency;

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(xAccel, yAccel, zAccel);
  }

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(xAngular, yAngular, zAngular);
  }

  if((abs(xAccel) > shakeForce) || (abs(yAccel) > shakeForce) || (abs(zAccel) > shakeForce))
  {
    shakeCount++;
  }
  else
  {
    if(shakeCount > 0) shakeCount--;
  }
}

bool IMUManager::isShaken()
{
  if(shakeCount > shakeThreshold)
  {
    shakeCount = 0;
    return true;
  }
  return false;
}

float IMUManager::GetXAcceleration()
{
  return xAccel;
}

float IMUManager::GetYAcceleration()
{
  return yAccel;
}

float IMUManager::GetZAcceleration()
{
  return zAccel;
}

float IMUManager::GetXAngular()
{
  return xAngular;
}

float IMUManager::GetYAngular()
{
  return yAngular;
}

float IMUManager::GetZAngular()
{
  return zAngular;
}

