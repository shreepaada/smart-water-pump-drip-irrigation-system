#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define TELEGRAM_BOT_TOKEN ""
//#define TELEGRAM_CHAT_ID 
String chatIds[] = {"", "","",""};

//

WiFiClientSecure client;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, client);

const int TAP_PIN = D1;
bool timerRunning = false;

int timer = 0;

void setup() {
  Serial.begin(115200);

  pinMode(TAP_PIN, OUTPUT);
  digitalWrite(TAP_PIN, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setInsecure();
}

void loop() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    Serial.println("New message(s) received:");

    for (int i = 0; i < numNewMessages; i++) {
      String chatId = String(bot.messages[i].chat_id);
      String text = bot.messages[i].text;

      Serial.println("Chat ID: " + chatId);
      Serial.println("Text: " + text);

      if (text == "/start") {
        for (int i = 0; i < sizeof(chatIds) / sizeof(chatIds[0]); i++) {
         bot.sendMessage(chatIds[i], "Welcome! Here are the available commands:\n/on - turn on the tap\n/off - turn off the tap\n/cancel - to cancel timer (terminate)\n\n/status - check the status of the tap\n\n\n\n/timer15 - turn on the tap for 15 minutes\n\n/timer30 - turn on the tap for 30 minutes\n");
        }
      } else if (text == "/on") {
        digitalWrite(TAP_PIN, HIGH);
        for (int i = 0; i < sizeof(chatIds) / sizeof(chatIds[0]); i++) {
        bot.sendMessage(chatIds[i], "Tap turned on");
        }
        timer = 0;
      } else if (text == "/off") {
        digitalWrite(TAP_PIN, LOW);
        for (int i = 0; i < sizeof(chatIds) / sizeof(chatIds[0]); i++) {
         bot.sendMessage(chatIds[i], "Tap turned off");
        }
        timer = 0;
      } else if (text == "/timer15") {
        digitalWrite(TAP_PIN, HIGH);
        for (int i = 0; i < sizeof(chatIds) / sizeof(chatIds[0]); i++) {
        bot.sendMessage(chatIds[i], "Tap turned on for 15 minutes");
        }
        timer = 15;
      } else if (text == "/timer30") {
        digitalWrite(TAP_PIN, HIGH);
        for (int i = 0; i < sizeof(chatIds) / sizeof(chatIds[0]); i++) {
        bot.sendMessage(chatIds[i], "Tap turned on for 30 minutes");
        }
        timer = 30;
      } else if (text == "/status") {
        String status = getStatus();
        for (int i = 0; i < sizeof(chatIds) / sizeof(chatIds[0]); i++) {
        bot.sendMessage(chatIds[i], "Tap status: " + status);
        }
      } else if (text == "/cancel") {
        if (timer != 0) {
          digitalWrite(TAP_PIN, LOW);
          for (int i = 0; i < sizeof(chatIds) / sizeof(chatIds[0]); i++) {
          bot.sendMessage(chatIds[i], "Tap timer cancelled");
          }
          timer = 0;
        } else {
          bot.sendMessage(chatId, "No active timer to cancel");
        }
      } else {
        bot.sendMessage(chatId, "Invalid command");
      }
    }

    bot.last_message_received = bot.messages[numNewMessages - 1].update_id + 1;

    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }

  // Check for active timer
  if (timer != 0 && !timerRunning) {
    timerRunning = true;
    delay(timer * 60000); // timer value is in minutes
    digitalWrite(TAP_PIN, LOW);
    for (int i = 0; i < sizeof(chatIds) / sizeof(chatIds[0]); i++) {
    bot.sendMessage(chatIds[i], "Tap turned off");
    }
    timerRunning = false;
    timer = 0;
  }

  delay(100); // 1 second
}
String getStatus() {
  String status = "";
  for (int i = 0; i < sizeof(chatIds) / sizeof(chatIds[0]); i++) {
    int state = digitalRead(TAP_PIN);
    if (timer != 0) {
      status += "Tap is on for " + String(timer) + " minutes. ";
    } else if (state == HIGH) {
      status += "Tap is on. ";
    } else {
      status += "Tap is off. ";
    }
  }
  return status;
}
