#include <Ethernet.h>
#include <PubSubClient.h>
#include <LiquidCrystal.h>
#include <TimerOne.h> // TimerOne kirjasto tuulen nopeuden laskentaan

LiquidCrystal lcd(8, 7, 6, 5, 4, 3);

// Valintamuuttuja: 0 = valoisuus, 1 = tuulen nopeus
int mode = 0; // Vaihda tähän 0 (valoisuus) tai 1 (tuulen nopeus)

// --- Valoisuusmuuttujat ---
int lightSensorPin = A6;
float voltage, maxVoltage = 0.0, minVoltage = 5.0, amplitude = 0.0, highestAmplitude = 0.0;
String lightLevel = "Unknown";

// --- Tuulen nopeus -muuttujat ---
volatile unsigned long lastPulseTime = 0; // Edellisen pulssin aika (mikrosekunneissa)
volatile int pulseCount = 0;              // Pulssilaskuri
float windSpeed = 0;                      // Laskettu tuulen nopeus [m/s]
float windSpeeds[10];                     // Taulukko 10 sekunnin arvojen tallennukseen
int currentIndex = 0;                     // Indeksi tuulen nopeuksille
float averageWindSpeed = 0;               // Keskiarvoinen tuulen nopeus

byte server[] = { 192, 168, 0, 1 }; // Dummy IP
unsigned int Port = 1883;
char* clientId = "DemoClient";
char* deviceSecret = "Secret123";
char* deviceId = "Device001";
#define outTopic "Demo_out"


unsigned long lastSendTime = 0; // Edellisen viestin aikaleima
unsigned long interval = 10000;  // Viestin lähetysajanjakso (ms)

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("Initializing...");

    pinMode(2, INPUT); // Pulssianturin pinni tuulen nopeudelle
    attachInterrupt(digitalPinToInterrupt(2), handlePulse, RISING); // Pulssien keskeytys
    Timer1.initialize(1000000); // Aseta TimerOne keskeytyksiin 1 sekunnin välein
    Timer1.attachInterrupt(calculateWindSpeed); // Linkitetään laskentafunktio keskeytykseen

    fetch_IP(); // Ethernet-yhteyden alustaminen
    delay(2000);
    lcd.clear();
}

void loop() {
    if (mode == 0) {
        handleLightSensor(); // Käsittele valoisuus-tila
    }
    else if (mode == 1) {
        handleWindSpeedSensor(); // Käsittele tuulen nopeus -tila
    }
}

// --- Valoisuus-tila ---
void handleLightSensor() {
    // Lue valoisuusanturin arvo
    int sensorValue = analogRead(lightSensorPin);
    voltage = sensorValue * (5.0 / 1023.0);

    // Päivitä jännitteen arvot
    if (voltage > maxVoltage) maxVoltage = voltage;
    if (voltage < minVoltage) minVoltage = voltage;

    amplitude = maxVoltage - minVoltage;
    if (amplitude > highestAmplitude) highestAmplitude = amplitude;

    // Määritä valoisuuden taso
    if (amplitude < 1.5) lightLevel = "Dark";
    else if (amplitude < 3.0) lightLevel = "Dim";
    else lightLevel = "Bright";

    // Päivitä LCD ja lähetä MQTT-viesti 10 sekunnin välein
    static unsigned long lastUpdate = millis();
    if (millis() - lastUpdate > 10000) {
        displayLightOnLCD(); // Päivitä valoisuus LCD-näytöllä
        sendLightMQTTMessage(); // Lähetä valoisuuden tiedot MQTT:lle
        lastUpdate = millis();

        // Nollaa arvot seuraavaa sykliä varten
        maxVoltage = 0.0;
        minVoltage = 5.0;
        highestAmplitude = 0.0;
    }
    delay(10);
}

void displayLightOnLCD() {
    // Näytä valoisuus LCD-näytöllä
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Light: ");
    lcd.print(lightLevel);

    lcd.setCursor(0, 1);
    lcd.print("Percent: ");
    lcd.print(highestAmplitude * 20, 1);
    lcd.print("%");
}

void sendLightMQTTMessage() {
    // Lähetä valoisuusdata MQTT-palvelimelle
    if (!client.connected()) {
        connectMQTTServer();
    }
    if (client.connected()) {
        char valueBuffer[10];
        dtostrf(highestAmplitude * 20, 6, 2, valueBuffer);

        char buf[100];
        snprintf(buf, sizeof(buf), "IOTJS={\"S_name\":\"Nimeton_Valoisuus\",\"S_value\":%s}", valueBuffer);
        Serial.println(buf);
        client.publish(outTopic, buf);
    }
    else {
        Serial.println("Failed to send message: not connected to MQTT server.");
    }
}

// --- Tuulen nopeus -tila ---
void handleWindSpeedSensor() {
    // Käsittele tuulen nopeus -anturi
    unsigned long currentTime = millis();
    if (currentTime - lastSendTime >= interval) {
        sendWindSpeedMQTTMessage();
        lastSendTime = currentTime;
    }

    // Näytä tuulen nopeus ja keskiarvo LCD-näytöllä
    lcd.setCursor(0, 0);
    lcd.print("Speed: ");
    lcd.print(windSpeed, 2);
    lcd.print(" m/s    ");

    lcd.setCursor(0, 1);
    lcd.print("Average: ");
    lcd.print(averageWindSpeed, 2);
    lcd.print(" m/s    ");

    // Tulosta sarjamonitorille
    Serial.print("Wind Speed: ");
    Serial.print(windSpeed);
    Serial.print(" m/s | Average: ");
    Serial.print(averageWindSpeed);
    Serial.println(" m/s");
    delay(1000);
}

void sendWindSpeedMQTTMessage() {
    // Lähetä tuulen nopeusdata MQTT-palvelimelle
    if (!client.connected()) {
        connectMQTTServer();
    }
    if (client.connected()) {
        char valueBuffer[10];
        dtostrf(averageWindSpeed, 6, 2, valueBuffer);

        char buf[100];
        snprintf(buf, sizeof(buf), "IOTJS={\"S_name\":\"Nimeton_Tuulen_Nopeus\",\"S_value\":%s}", valueBuffer);
        Serial.println(buf);
        client.publish(outTopic, buf);
    }
    else {
        Serial.println("Failed to send message: not connected to MQTT server.");
    }
}

void handlePulse() {
    // Keskeytysrutiini: pulssien havaitseminen
    unsigned long now = micros();
    pulseCount++;
    lastPulseTime = now;
}

void calculateWindSpeed() {
    // Lasketaan tuulen nopeus
    noInterrupts();
    int pulses = pulseCount;
    pulseCount = 0;
    interrupts();

    float frequency = pulses * 1.0;
    windSpeed = -0.24 + frequency * 0.699;
    if (windSpeed < 0) windSpeed = 0;

    windSpeeds[currentIndex] = windSpeed;
    currentIndex = (currentIndex + 1) % 10;

    averageWindSpeed = 0;
    for (int i = 0; i < 10; i++) {
        averageWindSpeed += windSpeeds[i];
    }
    averageWindSpeed /= 10;
}

// --- Yhteiset funktiot ---
void fetch_IP() {
    // IP-osoitteen haku Ethernet-moduulille
    bool connectionSuccess = Ethernet.begin(mymac);
    if (!connectionSuccess) {
        Serial.println("Failed to access Ethernet controller");
    }
    else {
        Serial.print("Connected with IP: ");
        Serial.println(Ethernet.localIP());
    }
}

void connectMQTTServer() {
    // Yhdistä MQTT-palvelimelle
    Serial.println("Connecting to MQTT");
    if (client.connect(clientId, deviceId, deviceSecret)) {
        Serial.println("Connected OK");
    }
    else {
        Serial.println("Connection failed.");
        Serial.print("MQTT error code: ");
        Serial.println(client.state());
    }
}

