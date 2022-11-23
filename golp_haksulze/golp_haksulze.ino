#include <ArduinoJson.h>
#include "WiFiEsp.h"
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <SPI.h>
#include <sha256.h>
#include <String.h>

/*
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
#define btnLeft 3
#define btnEnter 4
#define btnRight 5 
 
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

int menu = 0; 
int temlog = 0;
int inMenu = 0; //메뉴에 들어간 상태
float stat_Voltage; //아두이노 출력 전압

boolean getIsConnected = false;

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 5000L; // delay between updates, in millisecondsunsigned long lastConnectionTime = 0;
const int WIFI_Initial_time=2000;



IPAddress serv(59, 0, 121, 66);
int PORT = 4885;

char ssid[] = "BongSoon_2.4GHz"; //와이파이 이름
char psw[] = "kimdong2006!"; //비밀번호
char server[] = "59.0.121.66"; //서버 주소
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



//함수 모음


void print_buffer(byte buffer[]){
    Serial.print(String("buffer[] = {"));
    for(int i = 0; i < 16; i++){
        if(i != 0)
        Serial.print(',');

        Serial.print(buffer[i]);

    }
    Serial.print("}");
}

void Check(String strim){
  tone(pie, 783.9909, 100);


  if (client.connect(server, 4885)) {
    Serial.println("Connecting...");

    String jsondata = "";
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["uid"] = strim;
    Serial.println(strim);

    root.printTo(jsondata); // printTo => String변수로 변환
    // Serial.print("jsondata: ");
    // Serial.println("{\"uid\":\"ardde\"}");
    // Serial.println(jsondata.length());

    client.print(F("POST /check"));
    client.print(F(" HTTP/1.1\r\n"));
    client.print(F("Host: 59.0.121.66:4885\r\n"));
    client.print(F("User-Agent: Arduino\r\n"));
    client.print(F("Content-Type: application/json\r\n"));
    client.print(F("Content-Length: "));
    client.println(jsondata.length());
    client.println(); //데이터 전송 구분을 위한 줄 넘김
    client.print(jsondata);
    lastConnectionTime = millis();
    getIsConnected = true;

    String line = client.readString();
    int ind1 = line.indexOf('%');
    int ind2 = line.indexOf('^');
    int check_res = line.substring(ind1+1, ind2).toInt(); //결과 값 받기, 정상이면 1, 아니면 0

    if(check_res == 1){
      lcd.print("PASS");
      Success();
    }
    else if(check_res == 2){
      lcd.print("Already USE");
      Fail();

    }
    else if(check_res == 0){
      lcd.print("Not Registered");
      Fail();
    }


  }

  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
    lcd.print("CONNECTION LOST");
    getIsConnected = false; 
  }
  
  client.flush();
  client.stop(); // 클라이언트 접속 종료

}

void Fail(){
  analogWrite(Red, 0);
  analogWrite(Green, 0);
  analogWrite(Blue, 0);
  tone(pie, 783.9909, 300);

  delay(300);

  analogWrite(Red, 255);
  analogWrite(Green, 0);
  analogWrite(Blue, 0); //빨간색 출력
  tone(pie, 783.9909, 300);

}

void Success(){
  analogWrite(Red, 175);
  analogWrite(Green, 255);
  analogWrite(Blue, 186); //얘쁜 초록색 출력

  tone(pie, 783.9909, 300);
}

String Crypto(String strim){ //uid 암호화  sha256 사용


  String temp;
  uint8_t *hash;
  Sha256.init();
  Sha256.print(strim);
  hash = Sha256.result();
  

  for (int i = 0; i < 32; i++) {
    temp = temp + hash[i] + " ";
  }


  return temp;
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
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, psw);
  }

  Serial.println("인터넷 연결 성공");
  Serial.println();
  Serial.println("서버와 연결 합니다...");




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
      case 4:
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Writing");
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


void Writing(){ //rfid카드 등록
  byte buffer[18];
  byte size = sizeof(buffer);

  byte buffer_uid[4];

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
    buffer_uid[i] = rfid.uid.uidByte[i];
    strim = strim + buffer_uid[i];
  }

  MFRC522::MIFARE_Key keyA, keyB; //keyA, keyB 선언

  for (byte i = 0; i<6; i++){
    keyA.keyByte[i] = 0xFF;
  }

  byte valueBlockA = 54; //13번 섹터 첫번째 블럭(4번 블럭)에 값을 지정 하기 위해 지정 해놓음
  byte valueBlockA_2 = 53;
  byte trailerblock = 55; //13번 섹터의 트레일러블럭인 7번 블럭을 지정 함.

  Serial.println("A키 인증 시작...");
  byte status; //status에 상태를 저장하기 위함.
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerblock, &keyA, &(rfid.uid)); //A키로 rfid인증 시작(&쓴이유 : 전역변수에 구애 받지 않기 위함.)
  if(status != MFRC522::STATUS_OK){
    Serial.print("PCD_Authenticate() 실패: ");
    // Serial.print(rfid.GetStatusCodeName(status));
    return;
  }

  Serial.println("1");
  char tem[64];
  // strcpy(tem, Crypto(strim).c_str());
  char *temp_cry = strtok(tem, " ");

  Serial.println("2");
  // byte value1Block_2[] = {
  // 0,0,0,0,  0,0,0,0,   0,0,0,0,   0,0,0,0, valueBlockA_2,~valueBlockA_2,valueBlockA_2,~valueBlockA_2
  // };

  byte value1Block[] = {
  0,0,0,0,  0,0,0,0,   0,0,0,0,   0,0,0,0, valueBlockA,~valueBlockA,valueBlockA,~valueBlockA
  };

  for(int i = 0; i<16; i++){
    value1Block[i] = temp_cry[i];
  }
  // for(int i = 15; i<32; i++){
  //   value1Block_2[i] = temp_cry[i];
  // }


  print_buffer(value1Block);
  
  
  Serial.println("기록중..");
  print_buffer(value1Block);
  status = rfid.MIFARE_Write(valueBlockA, value1Block, 16);
  // status = rfid.MIFARE_Write(valueBlockA_2, value1Block_2, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("MIFARE_Write() 실패 :");
    // Serial.println(rfid.GetStatusCodeName(status));
    return;
  }

  Serial.println("읽는 중...");
  size = sizeof(buffer);
  status = rfid.MIFARE_Read(valueBlockA, buffer, &size);

  print_buffer(buffer);

  // status = rfid.MIFARE_Read(valueBlockA_2, buffer, &size);

  print_buffer(buffer);

  Serial.println(Crypto(strim));

}








void setup() {
  initalize();
}

void loop() {
  //디버그
  stat_Voltage = analogRead(A0) * (5.0 / 1023.0); //아두이노 출력 전압 상태 확인
  


  if(inMenu == 0){ //메뉴 선택 화면
    if(digitalRead(btnLeft)){ //왼쪽누르면 menu가 0이 아닐때만, -1 하기, 그리고 화면에 반영
      if(menu != 0){
        menu = menu - 1;
        menuPreview(menu);
      }
    }
    else if(digitalRead(btnRight)){ //오른쪽 누르면 menu가 최대인 4가 아닐때만, +1 하고 화면에 반영
      if(menu != 4){
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

        case 4:
          Writing();
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
        case 4:
          Writing();
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
