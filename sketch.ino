#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Adafruit_BME280.h>

#define WIFI_SSID ""        // WiFi SSID
#define WIFI_PASSWORD ""    // WiFi password

#define TELEGRAM_BOT_TOKEN ""        // Telegram bot token
#define TELEGRAM_CHANNEL_ID ""       // Telegram channel/group chat ID

const unsigned long BOT_MTBS = 1000;

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, secured_client);
unsigned long bot_lasttime;

Adafruit_BME280 temperatureSensor;
const int buzzer = 13;

void setup() {
    connectWiFi();
    setupTime();

    temperatureSensor.begin();
    setupSpeaker();
}

void loop() {
    delay(2000);

    float temperature = temperatureSensor.readTemperature();
    Serial.printf("Temperature: %.1f\n", temperature);

    if (temperature > 30) {
        tone(buzzer, 1000);
        bot.sendMessage(TELEGRAM_CHANNEL_ID, "Температура піднялась до " + String(round(temperature)) + "°C", "");
        delay(800);
    }

    noTone(buzzer);
    delay(1000);
}

void connectWiFi() {
    Serial.begin(115200);

    Serial.print("Connecting to Wifi SSID ");
    Serial.print(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
}

void setupTime() {
    Serial.print("Retrieving time: ");
    configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
    time_t now = time(nullptr);
    while (now < 24 * 3600) {
        Serial.print(".");
        delay(100);
        now = time(nullptr);
    }
    Serial.println(now);
}

void setupSpeaker() {
    pinMode(buzzer, OUTPUT);
}

