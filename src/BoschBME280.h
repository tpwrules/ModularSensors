/*
 *BoschBME280.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Initial library developement done by Sara Damiano (sdamiano@stroudcenter.org).
 *
 *This file is for the Bosch BME280 Digital Pressure and Humidity Sensor
 *It is dependent on the Adafruit BME280 Library
 *
 *Documentation for the sensor can be found at:
 *https://www.bosch-sensortec.com/bst/products/all_products/bme280
 *
 * For Barometric Pressure:
 *  Resolution is 0.18Pa
 *  Absolute Accuracy is ±1hPa
 *  Relative Accuracy is ±0.12hPa
 *  Range is 300 to 1100 hPa
 *
 * For Temperature:
 *  Resolution is 0.01°C
 *  Accuracy is ±0.5°C
 *  Range is -40°C to +85°C
 *
 * For Humidity:
 *  Resolution is 0.008 % RH (16 bit)
 *  Accuracy is ± 3 % RH
 *
 * Sensor takes about 100ms to respond
 * Slowest response time (humidity): 1sec
 * Assume sensor is immediately stable
*/

#ifndef BoschBME280_h
#define BoschBME280_h

#include "SensorBase.h"
#include "VariableBase.h"

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#include <Adafruit_BME280.h>

#define BME280_NUM_VARIABLES 4
#define BME280_WARM_UP 100
#define BME280_STABILITY 0
#define BME280_RESAMPLE 1000

#define BME280_TEMP_RESOLUTION 2
#define BME280_TEMP_VAR_NUM 0

#define BME280_HUMIDITY_RESOLUTION 3
#define BME280_HUMIDITY_VAR_NUM 1

#define BME280_PRESSURE_RESOLUTION 2
#define BME280_PRESSURE_VAR_NUM 2

#define BME280_ALTITUDE_RESOLUTION 0
#define BME280_ALTITUDE_VAR_NUM 3
#define SEALEVELPRESSURE_HPA (1013.25)

// The main class for the Bosch BME280
class BoschBME280 : public Sensor
{
public:
    BoschBME280(int powerPin, uint8_t i2cAddressHex = 0x76, int readingsToAverage = 1);

    bool wake(void) override;
    SENSOR_STATUS setup(void) override;
    SENSOR_STATUS getStatus(void) override;
    String getSensorLocation(void) override;

    bool startSingleMeasurement(void) override;
    bool addSingleMeasurementResult(void) override;
protected:
    Adafruit_BME280 bme_internal;
    uint8_t _i2cAddressHex;
};


// Defines the Temperature Variable
class BoschBME280_Temp : public Variable
{
public:
    BoschBME280_Temp(Sensor *parentSense,
                     String UUID = "", String customVarCode = "")
      : Variable(parentSense, BME280_TEMP_VAR_NUM,
               F("temperature"), F("degreeCelsius"),
               BME280_TEMP_RESOLUTION,
               F("BoschBME280Temp"), UUID, customVarCode)
    {}
};


// Defines the Humidity Variable
class BoschBME280_Humidity : public Variable
{
public:
    BoschBME280_Humidity(Sensor *parentSense,
                         String UUID = "", String customVarCode = "")
      : Variable(parentSense, BME280_HUMIDITY_VAR_NUM,
               F("relativeHumidity"), F("percent"),
               BME280_HUMIDITY_RESOLUTION,
               F("BoschBME280Humidity"), UUID, customVarCode)
    {}
};


// Defines the Pressure Variable
class BoschBME280_Pressure : public Variable
{
public:
    BoschBME280_Pressure(Sensor *parentSense,
                         String UUID = "", String customVarCode = "")
      : Variable(parentSense, BME280_PRESSURE_VAR_NUM,
               F("barometricPressure"), F("pascal"),
               BME280_PRESSURE_RESOLUTION,
               F("BoschBME280Pressure"), UUID, customVarCode)
    {}
};


// Defines the Altitude Variable
class BoschBME280_Altitude : public Variable
{
public:
    BoschBME280_Altitude(Sensor *parentSense,
                         String UUID = "", String customVarCode = "")
      : Variable(parentSense, BME280_ALTITUDE_VAR_NUM,
               F("heightAboveSeaFloor"), F("meter"),
               BME280_ALTITUDE_RESOLUTION,
               F("BoschBME280Altitude"), UUID, customVarCode)
    {}
};


#endif
