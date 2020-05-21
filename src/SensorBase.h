/**
 * @file SensorBase.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the Sensor class.
 */

// Header Guards
#ifndef SRC_SENSORBASE_H_
#define SRC_SENSORBASE_H_

// Debugging Statement
// #define MS_SENSORBASE_DEBUG

#ifdef MS_SENSORBASE_DEBUG
#define MS_DEBUGGING_STD "SensorBase"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#include <pins_arduino.h>

// The largest number of variables from a single sensor
#define MAX_NUMBER_VARS 8


class Variable;  // Forward declaration

/**
 *
 * @brief The "Sensor" class is used for all sensor-level operations - waking,
 * sleeping, and taking measurements.
 */
class Sensor {
 public:
    /**
     *
     * @brief Construct a new Sensor object.
     *
     * @param sensorName The name of the sensor.  Defaults to "Unknown".
     * @param numReturnedVars The number of variable results returned by the
     * sensor.  Defaults to 1.
     * @param warmUpTime_ms The time in ms between when the sensor is powered on
     * and when it is ready to receive a wake command.  Defaults to 0.
     * @param stabilizationTime_ms The time in ms between when the sensor
     * receives a wake command and when it is able to return stable values.
     * Defaults to 0.
     * @param measurementTime_ms The time in ms between when a measurement is
     * started and when the result value is available.  Defaults to 0.
     * @param powerPin A pin on the mcu controlling power to the sensor.
     * Defaults to -1.
     * @param dataPin A pin on the mcu receiving data from the sensor.  Defaults
     * to -1.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor.  Defaults to 1.
     */
    Sensor(const char*   sensorName      = "Unknown",
           const uint8_t numReturnedVars = 1, uint32_t warmUpTime_ms = 0,
           uint32_t stabilizationTime_ms = 0, uint32_t measurementTime_ms = 0,
           int8_t powerPin = -1, int8_t dataPin = -1,
           uint8_t measurementsToAverage = 1);
    /**
     *
     * @brief Destroy the Sensor object - no action taken.
     */
    virtual ~Sensor();

    // These functions are dependent on the constructor and return the
    // constructor values.
    /**
     *
     * @brief Get the pin or connection location between the mcu and the sensor.
     *
     * @return String A String with either the data pin number of other
     * information about how the sensor is attached to the mcu.
     */
    virtual String getSensorLocation(void);
    /**
     *
     * @brief Get the name of the sensor.
     *
     * @return String The sensor name as given in the constructor.
     */
    virtual String getSensorName(void);
    /**
     *
     * @brief Concatentate and returns the name and location.
     *
     * @return String A concatenation of the sensor name and its "location" -
     * how it is connected to the mcu.
     */
    String getSensorNameAndLocation(void);
    /**
     *
     * @brief Get the pin number controlling sensor power.
     *
     * @return int8_t The pin on the mcu controlling power to the sensor.
     */
    virtual int8_t getPowerPin(void);

    /**
     *
     * @brief Set the number measurements to average.
     *
     * @param nReadings The number of readings to take and average to create a
     * result from the sensor.  Overrides any value given in the constructor.
     */
    void setNumberMeasurementsToAverage(int nReadings);
    /**
     *
     * @brief Get the number of measurements to average.
     *
     * @return uint8_t The number of readings to take and average to create a
     * result from the sensor.
     */
    uint8_t getNumberMeasurementsToAverage(void);

    /**
     *
     * @brief Get the 8-bit code for the current status of the sensor.
     *
     * Bit 0 - 0=Has NOT been successfully set up, 1=Has been setup
     * Bit 1 - 0=No attempt made to power sensor, 1=Attempt made to power sensor
     * Bit 2 - 0=Power up attampt failed, 1=Power up attempt succeeded
     *       - Use the isWarmedUp() function to check if enough time has passed
     *         to be ready for sensor communication.
     * Bit 3 - 0=Activation/wake attempt made, 1=No activation/wake attempt made
     *       - check _millisSensorActivated or bit 4 to see if wake() attempt
     *       was successful
     *       - a failed activation attempt will give _millisSensorActivated = 0
     * Bit 4 - 0=Wake/Activate failed, 1=Is awake/actively measuring
     *       - Use the isStable() function to check if enough time has passed
     *         to begin a measurement.
     * Bit 5 - 0=Start measurement requested attempt made, 1=No measurements
     * have been requested
     *       - check _millisMeasurementRequested or bit 6 to see if
     *       startSingleMeasurement() attempt was successful
     *       - a failed request attempt will give _millisMeasurementRequested =
     *       0
     * Bit 6 - 0=Measurement start failed, 1=Measurement attempt succeeded
     *       - Use the isMeasurementComplete() to check if enough time has
     *       passed
     *         for a measurement to have been completed.
     * Bit 7 - 0=No known errors, 1=Some sort of error has occurred
     */
    uint8_t getStatus(void);

    /**
     *
     * @brief Do any one-time preparations needed before the sensor will be able
     * to take readings.
     *
     * May not require any action.  Generally, the sensor must be powered on for
     * setup.
     *
     * @return true The setup was successful
     * @return false Some part of the setup failed
     */
    virtual bool setup(void);

    /**
     *
     * @brief Update the sensor's values.
     *
     * This clears the values array, starts and averages as many measurement
     * readings as requested, and then notifies the registered variables of the
     * new results.  All possible waits are included in this function!
     *
     * @return true All steps of the sensor update completed successfully
     * @return false One or more of the update steps failed.
     */
    virtual bool update(void);

    /**
     *
     * @brief Turn on the sensor power, if applicable.
     *
     * Generally this is done by setting the power pin HIGH.  Also sets the
     * _millisPowerOn timestamp.
     */
    virtual void powerUp(void);
    /**
     *
     * @brief Turn off the sensor power, if applicable.
     *
     * Generally this is done by setting the power pin LOW.  Also un-sets the
     * _millisPowerOn timestamp (sets _millisPowerOn to 0.)
     *
     */
    virtual void powerDown(void);

    /**
     *
     * @brief Wake the sensor up, if necessary.
     *
     * Does whatever it takes to get a sensor in the proper state to begin a
     * measurement after the power is on. This *may* require a waitForWarmUp()
     * before wake commands can be sent. The wait is NOT included in this
     * function! This also sets the _millisSensorActivated timestamp.
     *
     * By default, verifies the power is on and returns true.
     *
     * @return true The wake function completed successfully.
     * @return false Wake did not complete successfully.
     */
    virtual bool wake(void);
    /**
     *
     * @brief Puts the sensor to sleep, if necessary.
     *
     * This also un-sets the _millisSensorActivated timestamp (sets it to 0) if
     * any action is taken to put the sensor to sleep.  This does NOT power down
     * the sensor!
     *
     * The default implentation takes no action - not even un-setting the time
     * stamp.
     *
     * @return true The sleep function completed successfully.
     * @return false Sleep did not complete successfully.
     */
    virtual bool sleep(void);

    /**
     *
     * @brief Tell the sensor to start a single measurement, if needed.
     *
     * This also sets the _millisMeasurementRequested timestamp.
     *
     * The default implentation only sets the timestamp.
     *
     * @note The sensor *may* require a waitForWarmUp() before measurement
     * commands can bee sent. It *may* also require a waitForStability() before
     * returned measurements will be any good. The waits are NOT included in
     * this function!
     *
     * @return true The start measurement function completed successfully.
     * @return false The start measurement function did not complete
     * successfully.
     */
    virtual bool startSingleMeasurement(void);

    /**
     *
     * @brief Get the results from a single measurement.
     *
     * This also un-sets the _millisMeasurementRequested timestamp (sets
     * _millisMeasurementRequested to 0).
     *
     * @note The sensor *may* also require awaitForMeasurementCompletion() to
     * ensure a measurement is done.  The wait is NOT included in this function.
     *
     * @return true The function completed successfully.
     * @return false The function did not complete successfully.
     */
    virtual bool addSingleMeasurementResult(void) = 0;

    /**
     *
     * @brief The array of result values for each sensor.
     */
    float sensorValues[MAX_NUMBER_VARS];
    // This is a string with a pretty-print of the values array
    // String getStringValueArray(void);
    /**
     *
     * @brief Clear the values array - that is, sets all values to -9999.
     *
     */
    void clearValues();
    /**
     *
     * @brief Verify that a measurement is OK (ie, not -9999) before adding it
     * to the result array
     *
     * @param resultNumber The position of the result within the result array.
     * @param resultValue The value of the result.
     */
    void verifyAndAddMeasurementResult(uint8_t resultNumber, float resultValue);
    /**
     *
     * @brief Verify that a measurement is OK (ie, not -9999) before adding it
     * to the result array
     *
     * @param resultNumber The position of the result within the result array.
     * @param resultValue The value of the result.
     */
    void verifyAndAddMeasurementResult(uint8_t resultNumber,
                                       int16_t resultValue);
    /**
     *
     * @brief Average the results of all measurements by dividing the sum of
     * all measurements by the number of measurements taken.
     */
    void averageMeasurements(void);

    /**
     *
     * @brief Register a variable object to a sensor.
     *
     * @param sensorVarNum The position the variable result holds in the
     * variable result array.
     * @param var A ponter to the Variable object.
     *
     * @note Only one variable can be assigned to each place in the array!
     */
    void registerVariable(int sensorVarNum, Variable* var);
    /**
     *
     * @brief Notify attached variables of new values.
     */
    void notifyVariables(void);


    /**
     *
     * @brief Check if the power pin is currently high.
     *
     * @param debug True to output the result to the debugging Serial
     * @return true Indicates the power pin is currently HIGH
     * @return false Indicates the power pin is currently not high (ie, it's
     * low)
     */
    bool checkPowerOn(bool debug = false);
    /**
     *
     * @brief Check whether or not enough time has passed between the sensor
     * receiving power and being ready to respond to logger commands.
     *
     * @param debug True to output the result to the debugging Serial
     * @return true Indicates that enough time has passed
     * @return false Indicates that the sensor is not yet ready to respond to
     * commands
     */
    virtual bool isWarmedUp(bool debug = false);
    /**
     *
     * @brief Hold all further program execution until this sensor is ready to
     * receive commands.
     */
    void waitForWarmUp(void);

    /**
     *
     * @brief Check whether or not enough time has passed between the sensor
     * being awoken/activated and being ready to output stable values.
     *
     * @param debug True to output the result to the debugging Serial
     * @return true Indicates that enough time has passed
     * @return false Indicates that the sensor has not yet stabilized
     */
    virtual bool isStable(bool debug = false);
    /**
     *
     * @brief Hold all further program execution until this sensor is reporting
     * stable values.
     */
    void waitForStability(void);

    /**
     *
     * @brief Check whether or not enough time has passed between when the
     * sensor was asked to take a single measurement and when that measurement
     * is expected to be complete.
     *
     * @param debug True to output the result to the debugging Serial
     * @return true Indicates that enough time has passed
     * @return false Indicates that the measurement is not expected to have
     * completed
     */
    virtual bool isMeasurementComplete(bool debug = false);
    /**
     *
     * @brief Hold all further program execution until this sensor is has
     * finished the current measurement.
     */
    void waitForMeasurementCompletion(void);


 protected:
    /**
     *
     * @brief Internal reference to the sensor data pin
     *
     * @note SIGNED int, to allow negative numbers for unused pins
     */
    int8_t _dataPin;
    /**
     *
     * @brief Internal reference to the sensor power pin
     *
     * @note SIGNED int, to allow negative numbers for unused pins
     */
    int8_t _powerPin;
    /**
     *
     * @brief Internal reference to the sensor name.
     */
    const char* _sensorName;
    /**
     *
     * @brief Internal reference to the number of values the sensor is capable
     * of reporting.
     */
    const uint8_t _numReturnedVars;
    /**
     *
     * @brief Internal reference to the number of measurements from the sensor
     * to average.
     */
    uint8_t _measurementsToAverage;
    /**
     *
     * @brief Array with the number of valid measurement values taken by the
     * sensor in the current update cycle.
     */
    uint8_t numberGoodMeasurementsMade[MAX_NUMBER_VARS];

    /**
     *
     * @brief The time needed from the when a sensor has power until it's ready
     * to talk.
     */
    uint32_t _warmUpTime_ms;
    /**
     *
     * @brief The processor elapsed time when the power was turned on for the
     * sensor.
     *
     * The _millisPowerOn value is set in the powerUp() function.  It is un-set
     * in the powerDown() function.
     */
    uint32_t _millisPowerOn;

    /**
     *
     * @brief The time needed from the when a sensor is activated until the
     * readings are stable.
     *
     */
    uint32_t _stabilizationTime_ms;
    /**
     *
     * @brief The processor elapsed time when the sensor was activiated - ie,
     * when the wake function was run.
     *
     * The _millisSensorActivated value is *usually* set in the wake() function,
     * but may also be set in the startSingleMeasurement() function.  It is
     * generally un-set in the sleep() function.
     *
     */
    uint32_t _millisSensorActivated;

    /**
     *
     * @brief The time needed from the when a sensor is told to take a single
     * reading until that reading is expected to be complete
     *
     */
    uint32_t _measurementTime_ms;
    /**
     *
     * @brief The processor elapsed time when a measuremnt was started - ie,
     * when the startSingleMeasurement function was run.
     *
     * The _millisMeasurementRequested value is set in the
     * startSingleMeasurement() function. It *may* be unset in the
     * addSingleMeasurementResult() function.
     *
     */
    uint32_t _millisMeasurementRequested;

    /**
     *
     * @brief An 8-bit code for the sensor status
     */
    uint8_t _sensorStatus;

    /**
     *
     * @brief An array for each sensor containing the variable objects tied to
     * that sensor.  The MAX_NUMBER_VARS cannot be determined on a per-sensor
     * basis, because of the way memory is used on an Arduino.  It must be
     * defined once for the whole class.
     */
    Variable* variables[MAX_NUMBER_VARS];
};

#endif  // SRC_SENSORBASE_H_
