#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

//와이파이 이름
#define WIFI_SSID ""
//와이파이 비밀번호
#define WIFI_PASSWORD ""

//Firebase API키
#define API_KEY ""
//Firebase 주소
#define DATABASE_URL "" 

//Firebase설정을 이메일이 필요하도록 설정했을때 사용
//만약 다 true로 되어있다면 필요없음
#define USER_EMAIL ""
#define USER_PASSWORD ""

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup(){
  
  //시리얼연결
  Serial.begin(115200);

  //와이파이 연결
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    //연결이 안된다면 계속 .을 출력
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  
  //와이파이 연결후 IP출력
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //Firebase 연결
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  
  //Test/test라는 테이블에 36.5를 삽입하는 코드 실수 정수 상관없음
  Firebase.RTDB.setInt(&fbdo,"Test/test",36.5);
  delay(1000);
}