#include <PubSubClient.h>
#include <WiFi.h>
#include "DHTesp.h"
#include <NTPClient.h>
#include <ESP32Servo.h>

#define LED_BUILTIN 2
#define BUZZER 12
#define DHT_PIN 15
#define LDR_PIN 34      
#define SERVO_PIN 13  

WiFiClient espClient;
PubSubClient mqttClient(espClient);
DHTesp dhtSensor;
Servo windowServo;     

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

bool isScheduleON = false;
unsigned long scheduleOnTime;

float lightIntensity = 0.0;  
float latestLightSample = 0.0; 
float temperature = 25.0;    
char tempAr[6];
char lightAr[6];
char samplingIntervalAr[6]; 
char sendingIntervalAr[6]; 

unsigned long lastSampleTime = 0;
unsigned long lastSendTime = 0;
int samplingInterval = 5000;    
int sendingInterval = 120000;   
float lightReadings[24]; 
int readingIndex = 0;
int readingCount = 0;

float motorAngleOffset = 30.0;  
float controllingFactor = 0.75; 
float idealTemperature = 30.0;  
float currentMotorAngle = 30.0; 

void setup() {
  Serial.begin(115200);
  setupWifi();
  setupMqtt();
  
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  
  updateTemperature();
  
  timeClient.begin();
  timeClient.setTimeOffset(5.5*3600);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  
  pinMode(LDR_PIN, INPUT);
  
  windowServo.attach(SERVO_PIN);
  windowServo.write(motorAngleOffset);
  
  for (int i = 0; i < 24; i++) {
    lightReadings[i] = 0;
  }

  sendLightIntensity();
  
  publishConfiguration();
}

void loop() {
  if (!mqttClient.connected()) {
    connectToBroker();
  }
  mqttClient.loop();
  
  checkSchedule();
  
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastSampleTime >= samplingInterval) {
    lastSampleTime = currentMillis;
    sampleLightIntensity();
    updateTemperature(); 
  }
  
  if (currentMillis - lastSendTime >= sendingInterval) {
    lastSendTime = currentMillis;
    sendLightIntensity();
  }
  
  delay(100);
}

void sampleLightIntensity() {
  int rawValue = analogRead(LDR_PIN);
  latestLightSample = rawValue / 4095.0;
  
  lightReadings[readingIndex] = latestLightSample;
  readingIndex = (readingIndex + 1) % 24;
  if (readingCount < 24) {
    readingCount++;
  }

  char latestLightAr[6];
  String(latestLightSample, 2).toCharArray(latestLightAr, 6);
  mqttClient.publish("ENTC-MEDIBOX-LIGHT-LATEST", latestLightAr);
  
  adjustServoMotor();
  
  Serial.print("Light sample: ");
  Serial.println(latestLightSample);
}

void sendLightIntensity() {
  float sum = 0;
  for (int i = 0; i < readingCount; i++) {
    sum += lightReadings[i];
  }
  lightIntensity = sum / readingCount;
  
  String(lightIntensity, 2).toCharArray(lightAr, 6);
  mqttClient.publish("ENTC-MEDIBOX-LIGHT", lightAr);
  
  Serial.print("Average light intensity sent: ");
  Serial.println(lightAr);
}

void updateTemperature() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  
  if (isnan(data.temperature) || data.temperature < -40 || data.temperature > 80) {
    Serial.println("Warning: Invalid temperature reading detected!");
  } else {
    temperature = data.temperature;
    String(temperature, 2).toCharArray(tempAr, 6);
    mqttClient.publish("ENTC-MEDIBOX-TEMP", tempAr);
    
    Serial.print("Temperature sent: ");
    Serial.println(tempAr);
  }
}

void adjustServoMotor() {
  // θ = θ_offset + (180 - θ_offset) × I × γ × ln(ts/tu) × (T/T_med)
  float ts = samplingInterval / 1000.0; 
  float tu = sendingInterval / 1000.0;  
  float ratio = ts / tu;
  
  if (ratio <= 0) {
    ratio = 0.0001; 
  }
  
  float tempRatio = temperature / idealTemperature;
  if (tempRatio <= 0 || isnan(tempRatio)) {
    tempRatio = 25.0 / idealTemperature; 
    Serial.println("Warning: Invalid tempRatio, using default T=25°C");
  }
  
  float logTerm = abs(log(ratio)); 
  
  // Debug intermediate values
  Serial.print("ts: "); Serial.print(ts);
  Serial.print(", tu: "); Serial.print(tu);
  Serial.print(", ratio: "); Serial.print(ratio);
  Serial.print(", logTerm: "); Serial.print(logTerm);
  Serial.print(", I: "); Serial.print(latestLightSample);
  Serial.print(", γ: "); Serial.print(controllingFactor);
  Serial.print(", T: "); Serial.print(temperature);
  Serial.print(", T_med: "); Serial.print(idealTemperature);
  Serial.print(", T/T_med: "); Serial.print(tempRatio);
  Serial.println();
  
  float angle = motorAngleOffset + (180.0 - motorAngleOffset) * latestLightSample * 
                controllingFactor * logTerm * tempRatio;
  
  angle = constrain(angle, motorAngleOffset, 180.0);
  currentMotorAngle = angle;
  
  windowServo.write(angle);
  
  char angleStr[6];
  String(angle, 2).toCharArray(angleStr, 6);
  mqttClient.publish("ENTC-MEDIBOX-ANGLE", angleStr);
  
  Serial.print("Motor angle set to: ");
  Serial.println(angle);
}

void publishConfiguration() {
  char buffer[10];
  
  int samplingSeconds = samplingInterval / 1000;
  String(samplingSeconds).toCharArray(samplingIntervalAr, 6);
  mqttClient.publish("ENTC-MEDIBOX-CURRENT-SAMPLING", samplingIntervalAr);
  Serial.print("Current sampling interval (s): ");
  Serial.println(samplingSeconds);
  
  int sendingSeconds = sendingInterval / 1000;
  String(sendingSeconds).toCharArray(sendingIntervalAr, 6);
  mqttClient.publish("ENTC-MEDIBOX-CURRENT-SENDING", sendingIntervalAr);
  Serial.print("Current sending interval (s): ");
  Serial.println(sendingSeconds);
  
  String(motorAngleOffset, 1).toCharArray(buffer, 10);
  mqttClient.publish("ENTC-MEDIBOX-CURRENT-OFFSET", buffer);
  
  String(controllingFactor, 2).toCharArray(buffer, 10);
  mqttClient.publish("ENTC-MEDIBOX-CURRENT-FACTOR", buffer);
  
  String(idealTemperature, 1).toCharArray(buffer, 10);
  mqttClient.publish("ENTC-MEDIBOX-CURRENT-IDEAL-TEMP", buffer);
}

unsigned long getTime() {
  timeClient.update();
  return timeClient.getEpochTime();
}

void buzzerOn(bool on) {
  if (on) {
    tone(BUZZER, 256);
  } else {
    noTone(BUZZER);
  }
}

void setupWifi() {
  Serial.println("");
  Serial.println("Connecting to ");
  Serial.println("Wokwi-GUEST");
  WiFi.begin("Wokwi-GUEST", "");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupMqtt() {
  mqttClient.setServer("test.mosquitto.org", 1883);
  mqttClient.setCallback(receiveCallback);
}

void connectToBroker() {
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32-Medibox-653652643612")) {
      Serial.println("connected");
      mqttClient.subscribe("ENTC-ADMIN-MAIN-ON-OFF");
      mqttClient.subscribe("ENTC-ADMIN-SCH-ON");
      mqttClient.subscribe("ENTC-MEDIBOX-SAMPLING-INTERVAL");
      mqttClient.subscribe("ENTC-MEDIBOX-SENDING-INTERVAL");
      mqttClient.subscribe("ENTC-MEDIBOX-ANGLE-OFFSET");
      mqttClient.subscribe("ENTC-MEDIBOX-CONTROL-FACTOR");
      mqttClient.subscribe("ENTC-MEDIBOX-IDEAL-TEMP");
    } else {
      Serial.println("failed");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}

void receiveCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  char payloadCharAr[length + 1];
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    payloadCharAr[i] = (char)payload[i];
  }
  payloadCharAr[length] = '\0'; 
  
  Serial.println();
  
  if (strcmp(topic, "ENTC-ADMIN-MAIN-ON-OFF") == 0) {
    buzzerOn(payloadCharAr[0] == '1');
  } 
  else if (strcmp(topic, "ENTC-ADMIN-SCH-ON") == 0) {
    if (payloadCharAr[0] == 'N') {
      isScheduleON = false;
    } else {
      isScheduleON = true;
      scheduleOnTime = atol(payloadCharAr);
    }
  }
  else if (strcmp(topic, "ENTC-MEDIBOX-SAMPLING-INTERVAL") == 0) {
    int newInterval = atoi(payloadCharAr);
    if (newInterval > 0) {
      samplingInterval = newInterval * 1000; 
      Serial.print("New sampling interval set: ");
      Serial.println(samplingInterval);
      adjustServoMotor();
    }
  }
  else if (strcmp(topic, "ENTC-MEDIBOX-SENDING-INTERVAL") == 0) {
    int newInterval = atoi(payloadCharAr);
    if (newInterval > 0) {
      sendingInterval = newInterval * 1000; 
      Serial.print("New sending interval set: ");
      Serial.println(sendingInterval);
      adjustServoMotor();
    }
  }
  else if (strcmp(topic, "ENTC-MEDIBOX-ANGLE-OFFSET") == 0) {
    float newOffset = atof(payloadCharAr);
    if (newOffset >= 0 && newOffset <= 120) {
      motorAngleOffset = newOffset;
      Serial.print("New angle offset set: ");
      Serial.println(motorAngleOffset);
      adjustServoMotor();
    }
  }
  else if (strcmp(topic, "ENTC-MEDIBOX-CONTROL-FACTOR") == 0) {
    float newFactor = atof(payloadCharAr);
    if (newFactor >= 0 && newFactor <= 1) {
      controllingFactor = newFactor;
      Serial.print("New controlling factor set: ");
      Serial.println(controllingFactor);
      adjustServoMotor();
    }
  }
  else if (strcmp(topic, "ENTC-MEDIBOX-IDEAL-TEMP") == 0) {
    float newTemp = atof(payloadCharAr);
    if (newTemp >= 10 && newTemp <= 40) {
      idealTemperature = newTemp;
      Serial.print("New ideal temperature set: ");
      Serial.println(idealTemperature);
      adjustServoMotor();
    }
  }
}

void checkSchedule() {
  if (isScheduleON) {
    unsigned long currentTime = getTime();
    if (currentTime > scheduleOnTime) {
      buzzerOn(true);
      isScheduleON = false;
      mqttClient.publish("ENTC-ADMIN-MAIN-ON-OFF-ESP", "1");
      mqttClient.publish("ENTC-ADMIN-SCH-ESP-ON", "0");
      Serial.println("Schedule ON");
    }
  }
}