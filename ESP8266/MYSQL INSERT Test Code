#include <ESP8266WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

IPAddress server_addr(10, 0, 1, 35); //MYSQL 서버
char user[] = "root";                //MYSQL 유저이름
char password[] = "secret";          //MYSQL 비밀번호
int port =3306;                      //MYSQL 포트번호
char INSERT_SQL[] = "INSERT INTO test_arduino.hello_arduino (message) VALUES ('Hello, Arduino!')"; //데이터를 넣어야하는 쿼리문

char ssid[] = "your-ssid";           //WiFi 이름
char pass[] = "ssid-password";       //WiFi 비밀번호

WiFiClient client;
MySQL_Connection conn(&client);
MySQL_Cursor *cursor;

void setup(){
    Serial.begin(115200);
    while (!Serial);

    Serial.printf("\nConnecting to %s", ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected to network");
    Serial.print("My IP address is: ");
    Serial.println(WiFi.localIP());

    Serial.print("Connecting to SQL...  ");
    if (conn.connect(server_addr, port, user, password)) Serial.println("OK.");
    else Serial.println("FAILED.");

    cursor = new MySQL_Cursor(&conn);
}

void loop(){
    if (conn.connected()) cursor->execute(INSERT_SQL);

    delay(5000);
}
