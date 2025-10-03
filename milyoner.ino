#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

#define OLED_RESET 0
Adafruit_SSD1306 display(OLED_RESET);

ESP8266WebServer server(80);
WiFiClientSecure client;

const char* ssid = "";
const char* password = "";
const char* Gemini_Token = "";
const char* Gemini_Max_Tokens = "100";
const char* fingerprint = "";  // SHA1 fingerprint

int currentQuestion = 0;
int score = 0;
String currentCorrectAnswer = "";
String questionHTML = "";

void setup() {
  Serial.begin(115200);
  Wire.begin(D2, D1);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED bulunamadı"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("WiFi baglantisi yapiliyor...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Baglandi!");
  display.display();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/submit", HTTP_POST, handleSubmit);
  server.on("/next", HTTP_POST, handleNext);
  server.on("/quit", HTTP_POST, handleQuit);
  server.on("/restart", HTTP_POST, handleRestart);
  server.begin();

  Serial.println("HTTP sunucu baslatildi");
  Serial.println(WiFi.localIP());
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(WiFi.localIP());
  display.display();
  getNewQuestion();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", questionHTML);
}

void handleSubmit() {
  if (!server.hasArg("choice")) return;
  String userAnswer = server.arg("choice");
  String result = "";

  if (userAnswer == currentCorrectAnswer) {
    result = "<div class='message correct'>Dogru bildiniz!</div>";
    score += 1000;
    questionHTML = generateControlHTML(result);
  } else {
    result = "<div class='message wrong'>Kaybettiniz!</div>";
    questionHTML = result + generateResultHTML();
  }

  server.send(200, "text/html", questionHTML);
}

void handleNext() {
  currentQuestion++;
  getNewQuestion();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleQuit() {
  questionHTML = generateQuitHTML();
  server.send(200, "text/html", questionHTML);
}

void handleRestart() {
  currentQuestion = 0;
  score = 0;
  getNewQuestion();
  server.sendHeader("Location", "/");
  server.send(303);
}

void getNewQuestion() {
  String prompt = "Dünyada az bilinen bir bilgi içeren çoktan seçmeli (4 şıklı) bir genel kültür sorusu oluştur. Format: Soru: ...\\nA) ...\\nB) ...\\nC) ...\\nD) ...\\nCevap: A/B/C/D";
  String response = askGemini(prompt);

  int soruStart = response.indexOf("Soru:") + 6;
  int aStart = response.indexOf("A)");
  int bStart = response.indexOf("B)");
  int cStart = response.indexOf("C)");
  int dStart = response.indexOf("D)");
  int answerStart = response.indexOf("Cevap:");

  String soru = response.substring(soruStart, aStart);
  soru.trim();

  String A = response.substring(aStart + 2, bStart);
  A.trim();

  String B = response.substring(bStart + 2, cStart);
  B.trim();

  String C = response.substring(cStart + 2, dStart);
  C.trim();

  String D = response.substring(dStart + 2, answerStart);
  D.trim();

  currentCorrectAnswer = response.substring(answerStart + 7, answerStart + 8);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(("Soru " + String(currentQuestion + 1)).c_str());
  display.display();

  questionHTML = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Quiz</title><style>";
  questionHTML += "body{font-family:sans-serif;text-align:center;background:#f0f0f0;padding-top:50px;}";
  questionHTML += "button{margin:10px;padding:10px 20px;border:none;border-radius:8px;background:#2196F3;color:white;cursor:pointer;font-size:16px;}";
  questionHTML += ".container{width:90%;max-width:500px;margin:auto;background:#fff;padding:20px;border-radius:10px;box-shadow:0 0 10px rgba(0,0,0,0.1);}";
  questionHTML += "</style></head><body><div class='container'>";
  questionHTML += "<h2>" + String(currentQuestion + 1) + ". Soru</h2><p>" + soru + "</p>";
  questionHTML += "<form method='POST' action='/submit'>";
  questionHTML += "<button name='choice' value='A'>A) " + A + "</button><br>";
  questionHTML += "<button name='choice' value='B'>B) " + B + "</button><br>";
  questionHTML += "<button name='choice' value='C'>C) " + C + "</button><br>";
  questionHTML += "<button name='choice' value='D'>D) " + D + "</button><br></form>";
  questionHTML += "<form method='POST' action='/quit'><button>Yarismadan Cekil</button></form>";
  questionHTML += "</div></body></html>";
}

String generateControlHTML(String message) {
  String html = "<html><head><meta charset='UTF-8'><style>";
  html += "body{font-family:sans-serif;text-align:center;background:#f0f0f0;padding-top:50px;}";
  html += "button{margin:10px;padding:10px 20px;border:none;border-radius:8px;background:#4CAF50;color:white;font-size:16px;cursor:pointer;}";
  html += ".message{margin:20px auto;padding:15px;width:300px;border-radius:10px;font-size:18px;}";
  html += ".correct{background:#dff0d8;color:#3c763d;}";
  html += ".wrong{background:#f2dede;color:#a94442;}";
  html += "</style></head><body>";
  html += message;
  html += "<h3>Devam etmek ister misiniz?</h3>";
  html += "<form method='POST' action='/next'><button>Devam Et</button></form>";
  html += "<form method='POST' action='/quit'><button>Yarismadan Cekil</button></form></body></html>";
  return html;
}

String generateResultHTML() {
  String html = "<html><head><meta charset='UTF-8'><style>";
  html += "body{font-family:sans-serif;text-align:center;background:#fff;padding-top:50px;}";
  html += "button{margin-top:20px;padding:10px 20px;border:none;border-radius:8px;background:#2196F3;color:white;font-size:16px;cursor:pointer;}";
  html += "h2{color:#d32f2f;}";
  html += "</style></head><body>";
  html += "<h2>Yarisma Bitti!</h2>";
  html += "<p>Toplam Puan: " + String(score) + "</p>";
  html += "<form method='POST' action='/restart'><button>Yeniden Basla</button></form>";
  html += "</body></html>";
  return html;
}

String generateQuitHTML() {
  String html = "<html><head><meta charset='UTF-8'><style>";
  html += "body{font-family:sans-serif;text-align:center;background:#fff;padding-top:50px;}";
  html += "button{margin-top:20px;padding:10px 20px;border:none;border-radius:8px;background:#2196F3;color:white;font-size:16px;cursor:pointer;}";
  html += "h2{color:#ff9800;}";
  html += "</style></head><body>";
  html += "<h2>Yarismadan Cekildiniz</h2>";
  html += "<p>Yanıtlanan Soru: " + String(currentQuestion + 1) + "</p>";
  html += "<p>Kazanc: " + String(score) + " TL</p>";
  html += "<form method='POST' action='/restart'><button>Yeniden Basla</button></form>";
  html += "</body></html>";
  return html;
}

String askGemini(String question) {
  client.setInsecure();
  HTTPClient https;
  String url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=" + String(Gemini_Token);

  if (!https.begin(client, url)) return "Baglanti basarisiz";

  https.addHeader("Content-Type", "application/json");
  String payload = "{\"contents\":[{\"parts\":[{\"text\":\"" + question + "\"}]}],\"generationConfig\":{\"maxOutputTokens\":" + String(Gemini_Max_Tokens) + "}}";
  int httpCode = https.POST(payload);

  if (httpCode == HTTP_CODE_OK) {
    String response = https.getString();
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, response);
    String answer = doc["candidates"][0]["content"]["parts"][0]["text"].as<String>();
    return answer;
  } else {
    return "Hata: " + https.errorToString(httpCode);
  }

  https.end();
  return "Bilinmeyen hata";
}
