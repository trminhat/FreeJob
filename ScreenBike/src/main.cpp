/* freeRTOS library */
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include <freertos/timers.h>

/* Peripheral library*/
#include <Arduino.h>
#include "TFT_Screen.h"
#include <Thermocouple.h>
#include <MAX6675_Thermocouple.h>
#include <ClosedCube_SHT31D.h>
#include <Wire.h>
// #include "st7789v3.h"

/* SPI PIN DEFINITION */
#define SCK_PIN 22
#define CS_PIN 26
#define SO_PIN 21
#define SCK_PIN1 22
#define CS_PIN1 27
#define SO_PIN1 21

/* I2C PIN DEFINITION */
#define SDA_PIN 4
#define SCL_PIN 5

/* SENSOSR_VP */
#define ADC_PIN 36
#define ADC_RESOLUTION 4095.0
#define Vref 3.3
#define R1 300
#define R2 82

/* FILTER CURRENT FOR INA219 SENSORS */
#define UTILS_LOW_PASS_FILTER(value, sample, constant) (value -= (constant) * ((value) - (sample)))
#define UTILS_LOW_PASS_FILTER_2(value, sample, lconstant, hconstant) \
  if (sample < value)                                                \
  {                                                                  \
    UTILS_LOW_PASS_FILTER(value, sample, lconstant);                 \
  }                                                                  \
  else                                                               \
  {                                                                  \
    UTILS_LOW_PASS_FILTER(value, sample, hconstant);                 \
  }

#define SIMPLE_KALMAN_FILTER(value, sample, gain) (value += gain * (sample - value))

TaskHandle_t TFTdisplay_handle_task;
TaskHandle_t UpdateSensors_handle_task;

ClosedCube_SHT31D sht3xd;
SHT31D sht3xd_data;
TimerHandle_t timerHandle;

float sht3TempC, oldTempC, sht3Humi, oldHumi;

Thermocouple *thermocouple;
Thermocouple *thermocouple1;
float thermal1, thermal2, oldThermal1, oldThermal2;

TFT_Screen screen(sht3xd, sht3xd_data);

uint32_t seconds = 0, oldSeconds = 0;
uint32_t minutes = 0, oldMinutes = 0;
uint32_t hours = 0, oldHours = 0;

float Vmax = 0, Vmin = 0, Vnow = 0, Vout = 0, VnowCollect = 0;
float oldVmax = 0, oldVmin = 0, oldVnow = 0;
bool isVmaxChange = false;
bool isVnowChange = false;
bool isVminChange = false;
bool isSHT30TempChange = false;
bool isSHT30HumiChange = false;
bool isThermal1Change = false;
bool isThermal2Change = false;
// st7789v3 display;

void timerCallback(TimerHandle_t xTimer)
{
  seconds++;
  // Serial.print("seconds: ");
  // Serial.println(seconds);
  if (seconds == 60)
  {
    minutes++;
    seconds = 0;
    Serial.print("Minutes: ");
    Serial.println(minutes);
  }
  if (minutes == 60)
  {
    hours++;
    minutes = 0;
  }
  if (hours == 24)
  {
    hours = 0;
    Serial.print("Hours: ");
    Serial.println(hours);
  }
}

/* Get Maximum Current */
void getVmax(float &Vnow, float &Vmax)
{
  int16_t tempV = (int16_t)(Vnow * 100);
  int16_t tempMaxV = (int16_t)(Vmax * 100);

  if (tempV > tempMaxV)
  {
    Vmax = Vnow;
  }
}

/* Get Minumum Current */
void getVmin(float &Vnow, float &Vmin)
{

  int16_t tempV = (int16_t)(Vnow * 100);
  int16_t tempMinV = (int16_t)(Vmin * 100);
  static bool firstCheck = false;

  if (!firstCheck)
  {
    Vmin = Vnow;
  }

  if (tempMinV > tempV)
  {
    Vmin = Vnow;
    firstCheck = true;
  }
}

/* Read Voltage */
void readVoltage()
{
  // int readADC = analogRead(ADC_PIN);
  Vout = analogReadMilliVolts(ADC_PIN);
  Vout /= 1000;          // convert mV -> V
  Vout += (Vout * 0.02); // 2% tolerance resistor
  // Serial.printf("Vnow: %.2f\t", Vnow);

  VnowCollect = (Vout / R2) * (R1 + R2);
  // Serial.printf("VnowCollect: %.2f\n", VnowCollect);

  UTILS_LOW_PASS_FILTER(Vnow, VnowCollect, 0.2);
  // Serial.printf("Vnow: %.2f\n", Vnow);

  getVmax(Vnow, Vmax);
  // Serial.printf("Vmax: %.2f\t", Vmax);
  getVmin(Vnow, Vmin);
  // Serial.printf("OldVmin: %.2f\t", oldVmin);
  // Serial.printf("Vmin: %.2f\n", Vmin);
  if ((uint32_t)(oldVnow * 10) != (uint32_t)(Vnow * 10))
  {
    oldVnow = Vnow;
    isVnowChange = true;
  }
  else
    isVnowChange = false;

  if ((uint32_t)(oldVmax * 10) != (uint32_t)(Vmax * 10))
  {
    oldVmax = Vmax;
    isVmaxChange = true;
  }
  else
    isVmaxChange = false;

  if ((uint32_t)(oldVmin * 10) != (uint32_t)(Vmin * 10))
  {
    oldVmin = Vmin;
    isVminChange = true;
  }
  else
    isVminChange = false;
}

/* Conversion time is 0.2s maximum - accodring to datasheet */
void readThermalcouple()
{
  thermal1 = thermocouple->readCelsius();
  thermal2 = thermocouple1->readCelsius();
  Serial.printf("Thermal: %lf\t%lf\n\n", thermal1, thermal2);

  if ((uint32_t)(oldThermal1 * 10) != (uint32_t)(thermal1 * 10))
  {
    oldThermal1 = thermal1;
    isThermal1Change = true;
  }
  else
    isThermal1Change = false;

  if ((uint32_t)(oldThermal2 * 10) != (uint32_t)(thermal2 * 10))
  {
    oldThermal2 = thermal2;
    isThermal2Change = true;
  }
  else
    isThermal2Change = false;
}

void readSHT3XD()
{
  if (sht3xd_data.error == SHT3XD_NO_ERROR) // check if error
  {
    sht3xd_data = sht3xd.periodicFetchData();
    // Serial.printf("result.t: %.2f\t result.rh: %.2f\n", sht3xd_data.t, sht3xd_data.rh);
    if ((uint32_t)(oldTempC * 10) != (uint32_t)(sht3xd_data.t * 10))
    {
      oldTempC = sht3xd_data.t;
      isSHT30TempChange = true;
    }
    else
      isSHT30TempChange = false;

    if ((uint32_t)(oldHumi * 10) != (uint32_t)(sht3xd_data.rh * 10))
    {
      oldHumi = sht3xd_data.rh;
      isSHT30HumiChange = true;
    }
    else
      isSHT30HumiChange = false;
  }

  else
  {
    Serial.print(": [ERROR] Code #");
    Serial.println(sht3xd_data.error);
  }
}

void UpdateSensors_task(void *pvPara)
{
  while (true)
  {
    readSHT3XD();
    readVoltage();
    readThermalcouple();

    vTaskDelay(200); /* ->> Should set delay task just 0.2s maximum. If faster that sensor cannot read updated */
  }
}

void TFTdisplay_task(void *pvPara)
{
  while (true)
  {
    /* Show all Voltage from ADC */
    if (isVmaxChange)
    {
      screen.showData(VMax, 135, 13, oldVmax, Vmax, 40, 5, 95, 50, 1, TFT_GREEN, TFT_BLACK);
    }
    if (isVnowChange)
    {
      screen.showData(VNow, 135, 73, oldVnow, Vnow, 40, 65, 95, 50, 1, TFT_ORANGE, TFT_BLACK);
    }
    if (isVminChange)
    {
      screen.showData(VMin, 135, 136, oldVmin, Vmin, 40, 128, 95, 50, 1, TFT_YELLOW, TFT_BLACK);
    }

    /* Show Counter from Timer hanlder task */
    screen.showCounter(hours, minutes, seconds, 206, 0, TFT_WHITE, TFT_BLACK);

    /* Show Temperature and Humidity from SHT3 sensor */
    if (isSHT30TempChange)
    {
      screen.showData(SHT30_TEMP, 0, 0, oldTempC, sht3xd_data.t, 195, 50, 25, 20, 1, TFT_WHITE, TFT_BLACK);
    }

    if (isSHT30HumiChange)
    {
      screen.showData(SHT30_HUMI, 305, 50, oldHumi, sht3xd_data.rh, 280, 50, 25, 20, 1, TFT_WHITE, TFT_BLACK);
    }

    /* Show Thermal1 and Thermal2 from MAX6775 sensor */
    if (isSHT30TempChange)
    {
      screen.showData(THERMAL_1, 0, 0, oldThermal1, thermal1, 210, 80, 50, 35, 1, TFT_WHITE, TFT_BLACK);
    }

    if (isSHT30HumiChange)
    {
      screen.showData(THERMAL_2, 0, 0, oldThermal2, thermal2, 210, 128, 50, 35, 1, TFT_WHITE, TFT_BLACK);
    }

    vTaskDelay(500);
  }
}

// the setup function runs once when you press reset or power the board
void setup()
{
  Serial.begin(115200);

  pinMode(ADC_PIN, INPUT);
  Wire.begin(SDA_PIN, SCL_PIN);
  sht3xd.begin(0x44);
  screen.begin();

  thermocouple = new MAX6675_Thermocouple(SCK_PIN, CS_PIN, SO_PIN);
  thermocouple1 = new MAX6675_Thermocouple(SCK_PIN1, CS_PIN1, SO_PIN1);

  Serial.print("Serial #");
  Serial.println(sht3xd.readSerialNumber());
  if (sht3xd.periodicStart(SHT3XD_REPEATABILITY_HIGH, SHT3XD_FREQUENCY_10HZ) != SHT3XD_NO_ERROR)
    Serial.println("[ERROR] Cannot start periodic mode");

  /* Creat the Tasks */
  xTaskCreatePinnedToCore(TFTdisplay_task, "Task TFT display", 5000, NULL, 1, &TFTdisplay_handle_task, 1);
  xTaskCreatePinnedToCore(UpdateSensors_task, "Task Update Sensors", 5000, NULL, 1, &UpdateSensors_handle_task, 1);

  /* Create a timer */
  timerHandle = xTimerCreate("Timer",             // Name of the timer
                             pdMS_TO_TICKS(1000), // Timer period in ticks (1000ms = 1s)
                             pdTRUE,              // Auto-reload timer
                             (void *)0,           // Timer ID (can be NULL)
                             timerCallback        // Callback function )
  );

  if (timerHandle != NULL)
  { // Start the timer with a block time of 0 ticks
    xTimerStart(timerHandle, 0);
  }
}
// the loop function runs over and over again forever
void loop()
{
  vTaskDelete(NULL);
}