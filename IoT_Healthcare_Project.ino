#include <Wire.h>
#include "MAX30105.h"
#include "MPU6050.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "ThingSpeak.h"

// Sensor Objects
MAX30105 particleSensor;
MPU6050 mpu;
int16_t ax, ay, az;

// WiFi and ThingSpeak Config
const char* ssid = "Fast Faculty";
const char* password = "Faculty@Fast987";
unsigned long channelID = 2770301;
const char* writeAPIKey = "YQ08MFVTT1AC331C";

// Sensor Pins
const int LM35_PIN = 34;

// Heart Rate Variables
const byte RATE_SIZE = 10;
const unsigned long MIN_BEAT_INTERVAL = 600;
const unsigned long MAX_BEAT_INTERVAL = 1200;
const int MIN_IR_THRESHOLD = 50000;
const float SIGNAL_QUALITY_THRESHOLD = 0.8;
byte rates[RATE_SIZE];
byte rateSpot = 0;
byte bpm;
int beatAvg;
float oxygenSaturation;
float temperature;

// MPU6050 Variables
const int CALIBRATION_SAMPLES = 1000;
int16_t ax_offset, ay_offset, az_offset;
float ax_scale, ay_scale, az_scale;

// Step Counter Variables
float threshold = 0.8;
int stepCount = 0;
bool isPeak = false;
unsigned long lastStepTime = 0;

WiFiClient client;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    initializeSensors();
    connectWiFi();
}

void initializeSensors() {
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        Serial.println("MAX30102 not found!");
        while(1);
    }
    
    particleSensor.setup(0x3F, 4, 2, 400, 411, 2048);
    particleSensor.setPulseAmplitudeRed(0x3F);
    particleSensor.setPulseAmplitudeIR(0x3F);
    particleSensor.setPulseAmplitudeGreen(0);
    particleSensor.enableFIFORollover();
    
    initializeMPU();
}

void initializeMPU() {
    mpu.initialize();
    mpu.setFullScaleAccelRange(0); // 0 = ±2g range
    calibrateMPU();
}

void calibrateMPU() {
    long ax_sum = 0, ay_sum = 0, az_sum = 0;
    
    for(int i = 0; i < CALIBRATION_SAMPLES; i++) {
        mpu.getAcceleration(&ax, &ay, &az);
        ax_sum += ax;
        ay_sum += ay;
        az_sum += az;
        delay(2);
    }
    
    ax_offset = ax_sum / CALIBRATION_SAMPLES;
    ay_offset = ay_sum / CALIBRATION_SAMPLES;
    az_offset = az_sum / CALIBRATION_SAMPLES;
    
    az_scale = 16384.0 / (az_sum / CALIBRATION_SAMPLES);
}

void connectWiFi() {
    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected");
        ThingSpeak.begin(client);
    } else {
        Serial.println("\nWiFi Connection Failed");
    }
}

float checkSignalQuality(long irValue, long redValue) {
    static const int WINDOW_SIZE = 10;
    static long ir_values[WINDOW_SIZE];
    static int index = 0;
    
    ir_values[index] = irValue;
    index = (index + 1) % WINDOW_SIZE;
    
    long max_val = 0, min_val = LONG_MAX;
    for(int i = 0; i < WINDOW_SIZE; i++) {
        max_val = max(max_val, ir_values[i]);
        min_val = min(min_val, ir_values[i]);
    }
    
    return (float)(max_val - min_val) / max_val;
}

void updateHeartRate(long irValue, long redValue) {
    static unsigned long lastBeat = 0;
    static int validBeatsCount = 0;
    
    if (irValue < MIN_IR_THRESHOLD || checkSignalQuality(irValue, redValue) < SIGNAL_QUALITY_THRESHOLD) {
        beatAvg = 0;
    }
    
    unsigned long currentTime = millis();
    unsigned long beatInterval = currentTime - lastBeat;
    
    if (beatInterval > MIN_BEAT_INTERVAL && beatInterval < MAX_BEAT_INTERVAL) {
      bpm = 60000 / beatInterval;
    }    
    rates[rateSpot++] = bpm;
    rateSpot %= RATE_SIZE;
        
    if (++validBeatsCount >= RATE_SIZE) {
        long sum = 0;
        for (byte x = 0; x < RATE_SIZE; x++) {
            sum += rates[x];
        }
        beatAvg = sum / RATE_SIZE;
    }
        
    lastBeat = currentTime;
  
}

void detectSteps() {
    mpu.getAcceleration(&ax, &ay, &az);
    
    float accelX = (ax - ax_offset) * ax_scale / 16384.0;
    float accelY = (ay - ay_offset) * ay_scale / 16384.0;
    float accelZ = (az - az_offset) * az_scale / 16384.0;
    
    float vectorMagnitude = sqrt(pow(accelX, 2) + pow(accelY, 2) + pow(accelZ, 2));
    
    if (vectorMagnitude > threshold && !isPeak && millis() - lastStepTime > 250) {
        stepCount++;
        lastStepTime = millis();
        isPeak = true;
    }
    
    if (vectorMagnitude < threshold) {
        isPeak = false;
    }
}

float calculateSpO2(long irValue) {
    if (irValue < MIN_IR_THRESHOLD) return 0;
    return constrain(map(irValue, 50000, 100000, 95, 100), 90, 100);
}

float getTemperature() {
    int rawValue = analogRead(LM35_PIN);
    float voltage = rawValue * (3.3 / 4095.0);
    return voltage * 100.0;
}

void uploadToThingSpeak() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected. Attempting reconnection...");
        connectWiFi();
        return;
    }

    ThingSpeak.setField(1, beatAvg);
    ThingSpeak.setField(2, oxygenSaturation);
    ThingSpeak.setField(3, temperature);
    ThingSpeak.setField(4, stepCount);

    int result = ThingSpeak.writeFields(channelID, writeAPIKey);
    if (result != 200) {
        Serial.println("ThingSpeak upload failed");
    }
}

void loop() {
    static unsigned long lastUploadTime = 0;
    long irValue = particleSensor.getIR();
    long redValue = particleSensor.getRed();

    if (irValue > MIN_IR_THRESHOLD) {
        updateHeartRate(irValue, redValue);
    } else {
        beatAvg = 0;
    }

    temperature = getTemperature();
    oxygenSaturation = calculateSpO2(irValue);
    detectSteps();

    if (millis() - lastUploadTime > 5000) {
        uploadToThingSpeak();
        lastUploadTime = millis();
    }

    Serial.printf("HR: %d BPM, SpO2: %.1f%%, Temp: %.1f°C, Steps: %d\n", 
        beatAvg, oxygenSaturation, temperature, stepCount);

    delay(100);
}