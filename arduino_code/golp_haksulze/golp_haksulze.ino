#include <ArduinoJson.h> #json 수정 라이브러리
#include "WiFiEsp.h" #esp01 모듈 라이브러리
#include <SoftwareSerial.h> #시리얼 추가 라이브러리
#include <LiquidCrystal.h> #lcd 라이브러리
#include <MFRC522.h> #rfid 인식기 라이브러리
#include <MFRC522Extended.h> 
#include <SPI.h> #spi 통신 라이브러리 
#include <sha256.h> #암호화 라이브러리
#include <String.h>

/*{핀 정보}
 * sda = 53 <Yellow>
 * sck = 52 <Green>
 * mosi = 51 <Blue>
 * miso = 50 <Orange>
 * gnd = power - gnd 
 * rst = 2 <White>
 * vcc = 3.3v(breadboard b -1)
 * 
 * ----lcd----
 * rs = 12
 * e = 11
 * d4 = 6
 * d5 = 5
 * d6 = 4
 * d7 = 3
 * ----button----
 * Left = 3
 * Enter(Middle)= 4
 * Right = 5
 *
 * ----wifi----
 * RX  = 16 <Gray>
 * TX = 17 <White>
 *


 ------piezo-------
pie = 8<Orange>

------rgbled-------
Red = 12<Yel>
Green = 11 <Gre>
Blue = 10 <Bl>
 
 */
#define btnLeft 44
#define btnEnter 40
#define btnRight 36
 
#define sda 53
#define sck 52
#define mosi 51
#define miso 50
#define rst 2

#define wifi_RX 17
#define wifi_TX 16

#define pie 8

#define Red 12
#define Blue 10
#define Green 11

LiquidCrystal lcd(22, 23, 28, 29, 30, 31); //lcd모듈 객체화 이름 : lcd
MFRC522 rfid(sda, rst); //rfid 모듈 객체화 이름 : rfid
MFRC522::MIFARE_Key key;

int menu = 0; //메뉴번호
int temlog = 0; 
int inMenu = 0; //메뉴에 들어간 상태
float stat_Voltage; //아두이노 출력 전압

boolean getIsConnected = false;

unsigned long lastConnectionTime = 0;         // 마지막 연결 시간
const unsigned long postingInterval = 2000L; // 업데이트주기
const int WIFI_Initial_time=1000;



IPAddress serv(59, 0, 121, 66); //서버와 연결
int PORT = 4885; //서버 포트 (맞춰서 수정하세요)

char ssid[] = "WIFI NAEM"; //와이파이 이름
char psw[] = "PASSWORD"; //비밀번호
char server[] = "SERVER ADRESS"; //서버 주소 맞춰서수정하세요
int status = WL_IDLE_STATUS;

WiFiEspClient client;






//특수문자 설정

byte emoji_Heart[8] = {
  B00000,
  B00000,
  B01010,
  B10101,
  B01010,
  B00100,
  B00000,
  B00000
};

byte cha_Kim[] = {
  B00000,
  B11101,
  B00101,
  B00101,
  B00000,
  B01111,
  B01001,
  B01111
};


byte cha_Dong[] = {
  B01110,
  B01000,
  B01110,
  B00100,
  B11111,
  B00100,
  B01010,
  B00100
};

byte cha_Hoon[] = {
  B00100,
  B11111,
  B01010,
  B00100,
  B11111,
  B00100,
  B10000,
  B11111
};


byte mat[] = {
  B00000,
  B11010,
  B11011,
  B11010,
  B00000,
  B01010,
  B10101,
  B00000
};

byte it[] = {
  B00000,
  B01001,
  B10101,
  B01001,
  B00001,
  B01010,
  B10101,
  B00000
};

byte ge[] = {
  B00000,
  B00000,
  B11011,
  B01111,
  B01011,
  B01011,
  B00011,
  B00000
};

byte du[] = {
  B00000,
  B00000,
  B11110,
  B10000,
  B11110,
  B00000,
  B11111,
  B00000
};

byte se[] = {
  B00000,
  B00011,
  B01011,
  B01111,
  B01011,
  B10111,
  B00011,
  B00000
};

byte yo[] = {
  B00000,
  B01110,
  B01010,
  B01110,
  B00000,
  B01010,
  B11111,
  B00000 //사실 쓸데 없음
};

//함수 모음


void print_buffer(byte buffer[]){
    Serial.print(String("buffer[] = {"));
    for(int i = 0; i < 16; i++){
        if(i != 0)
        Serial.print(',');

        Serial.print(buffer[i]);

    }
    Serial.print("}"); //버퍼 출력기 사용X
}

void Check(String strim){ //밥 먹는지 안먹느닞 감지
  tone(pie, 1479.978, 100); //카드가 대지면 소리 울리기

  if (client.connect(server, 4885)) { //서버 연결
    Serial.println("Connecting...");

    String jsondata = ""; //json 데이터 초기화
    StaticJsonBuffer<200> jsonBuffer; //json 데이터 크기 설저
    JsonObject& root = jsonBuffer.createObject(); //json데이터 만들기
    root["uid"] = strim; //json  데이터에서 uid : strim(uid값)을 넣기
    Serial.println(strim);

    root.printTo(jsondata); // printTo => String변수로 변환
    // Serial.print("jsondata: ");
    // Serial.println("{\"uid\":\"ardde\"}");
    // Serial.println(jsondata.length());

    client.print("POST /check");
    client.print(" HTTP/1.1\r\n");
    client.print("Host: 59.0.121.66:4885\r\n");
    client.print("User-Agent: Arduino\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: ");
    client.println(jsondata.length());
    client.println(); //데이터 전송 구분을 위한 줄 넘김
    client.print(jsondata); //json의 데이터 전송
    lastConnectionTime = millis();
    getIsConnected = true; //위는 post통신의 프로토콜

    String line = client.readString();
    int ind1 = line.indexOf('%'); //0,1,2 값 가져오기
    int ind2 = line.indexOf('^');
    int check_res = line.substring(ind1+1, ind2).toInt(); //결과 값 받기, 정상이면 1, 아니면 0

    if(check_res == 1){
      lcd.print("     ENJOY!     ");
      Success();
    }
    else if(check_res == 2){
      lcd.print("   Already USE   ");
      Fail();

    }
    else if(check_res == 0){
      lcd.print(" Not Registered ");
      Fail();
    }
    client.stop();

  }

  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
    lcd.print("CONNECTION LOST");
    Fail();
    getIsConnected = false; 
  }
  
  client.flush();
  client.stop(); // 클라이언트 접속 종료

}

void Fail(){
  analogWrite(Red, 0);
  analogWrite(Green, 0);
  analogWrite(Blue, 0);
  tone(pie, 1479.978, 150);

  delay(300);

  analogWrite(Red, 255);
  analogWrite(Green, 0);
  analogWrite(Blue, 0); //빨간색 출력
  tone(pie, 1479.978, 300);

}

void Success(){
  analogWrite(Red, 175);
  analogWrite(Green, 255);
  analogWrite(Blue, 186); //얘쁜 초록색 출력

  tone(pie, 1479.978, 300);
}



void initalize(){
  //가장 초기에 시작되는 코드(여러 세팅 및 디버깅)
  Serial.begin(9600);
  Serial2.begin(115200);
  SPI.begin(); //spi통신 초기화
  rfid.PCD_Init(); //rfid 초기화
  rfid.PCD_DumpVersionToSerial();
  Serial.println("rfid initalize compelete.");
  
  lcd.begin(16, 2); //lcd 켜기  
  lcd.setCursor(0, 0); //첫 줄에 커서 세팅
  lcd.print("abcdefghizklnmop"); //디버그 메세지 출력
  lcd.setCursor(0, 1); //두 번째 줄에 커서 세팅
  lcd.print("!@#$%^1234567890"); //디버그 메세지 출력
  delay(1000);
  lcd.clear(); //초기화

  
  WiFi.init(&Serial2);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("shield not present");
    while (true);
  }
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, psw);
  }

  Serial.println("Success");
  Serial.println();
  Serial.println("Connecting with server...");




  //메뉴 버튼
  pinMode(btnLeft, INPUT);
  pinMode(btnEnter, INPUT);
  pinMode(btnRight, INPUT);
  menuPreview(0);

  //피에조
  pinMode(pie, OUTPUT);
  //LED
  pinMode(Red, OUTPUT);
  pinMode(Blue, OUTPUT);
  pinMode(Green, OUTPUT);

  //특수기호, 이모지 설정
  lcd.createChar(0, cha_Kim); //0 : 김
  lcd.createChar(1, cha_Dong); //1 : 동
  lcd.createChar(2, cha_Hoon); //2 : 훈
  lcd.createChar(3, emoji_Heart); //3 : 하트(이모지)
  lcd.createChar(4, mat); //4: 맛
  lcd.createChar(5, it); //5: 있
  lcd.createChar(6, ge); //6: 게
  lcd.createChar(7, du); //7: 드
  lcd.createChar(8, se); //8: 세
  lcd.createChar(9, yo); //9: 요


  return;
}

void menuPreview(int mode){ //메뉴 화면 미리보기 함수
  switch(mode){
    case 0: //모드 선택 장려 화면 미리보기
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("SELCET A MODE");
      lcd.setCursor(5, 1);
      lcd.print("<1/4>");
      break;
      
     case 1: //결과 출력 화면 미리보기
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("RESULTING MODE");
      lcd.setCursor(5, 1);
      lcd.print("<2/4>");
      break;

      case 2:
        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print("DEBUGING MODE");
        lcd.setCursor(5, 1);
        lcd.print("<3/4>");
        break;
      case 3:
        lcd.clear();
        lcd.setCursor(6, 0);
        lcd.print("INFO");
        lcd.setCursor(5, 1);
        lcd.print("<4/4>");
        break;

  }
  return;
}

void Resulting(){ //결과값 메뉴

  analogWrite(Red, 255);
  analogWrite(Green, 0);
  analogWrite(Blue, 0); //빨간색 출력


  byte buffer[4];

  String strim = "";

  lcd.clear();
  if (!rfid.PICC_IsNewCardPresent()) {
        return;
    }

  if (!rfid.PICC_ReadCardSerial()) {
        return;
  }

  Serial.println(F("Scanned PICC's UID:"));
  for ( uint8_t i = 0; i < 4; i++) {
    buffer[i] = rfid.uid.uidByte[i];
    strim = strim + buffer[i];
  }

  Check(strim);

  delay(1000);

}

void Debuging(){ //디버그 메뉴(모듈 이상 확인)
  //LCD 상태
  lcd.clear();
  lcd.write("LCD:");
  //전압 상태
  lcd.setCursor(0, 1);
  lcd.write("Volt:");
  lcd.print(stat_Voltage, 1);
  lcd.print("V");

  //LCD 상태 결과
  if(digitalRead(22)){
    lcd.setCursor(4, 0);
    lcd.write("ok");
  }
  else{
    lcd.setCursor(4, 0);
    lcd.write("n/a");
  }


  return;
}

void Info(){ //정보 메뉴
  lcd.clear();
  lcd.print("Made by ");
  lcd.write(byte(0));
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.write(byte(3));
  lcd.setCursor(0, 1);
  lcd.print("gmail:beuetv123");
}




void setup() {
  initalize();
}

void loop() {
  //디버그
  stat_Voltage = analogRead(A0) * (5.0 / 1023.0); //아두이노 출력 전압 상태 확인

  if(inMenu == 0){ //메뉴 선택 화면
    analogWrite(Red, 0);
    analogWrite(Green, 0);
    analogWrite(Blue, 0); //끄기
    if(digitalRead(btnLeft)){ //왼쪽누르면 menu가 0이 아닐때만, -1 하기, 그리고 화면에 반영
      if(menu != 0){
        menu = menu - 1;
        menuPreview(menu);
      }
    }
    else if(digitalRead(btnRight)){ //오른쪽 누르면 menu가 최대인 4가 아닐때만, +1 하고 화면에 반영
      if(menu != 3){
        menu = menu + 1;
        menuPreview(menu);
      }
    }
    else if(digitalRead(btnEnter)){ //메뉴선택하면 선택한 메뉴로 이동
      switch(menu){
        case 1:
          Resulting();
          inMenu = 1;
          break;
        case 2:
          Debuging();
          inMenu = 1;
          break;
        default:
          break;
        case 3:
          Info();
          inMenu = 1;
          break;


      }
    }
  }
  else if(inMenu == 1){  //메뉴에서
    switch(menu){
        case 1:
          Resulting();
          inMenu = 1;
          break;
        case 2:
          Debuging();
          inMenu = 1;
          break;
        default:
          break;
        case 3:
          Info();
          inMenu = 1;
          break;
      }
    if(digitalRead(btnEnter)){ //메뉴에서 엔터 누르면 이전 메뉴 선택 화면으로 이동
      menuPreview(menu);
      inMenu = 0;
    }
  }

  // Serial.print("menu : ");
  // Serial.println(menu);
  // Serial.print("btnEnter : ");
  // Serial.println(digitalRead(btnEnter));
  // Serial.print("inMenu : ");
  // Serial.println(rst);



  delay(100);
}
