#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#define GEARNUM = 5;

SoftwareSerial mySerial(10, 2);

int startButton = 3;

String json = "";

int appleNum = 1;
float appleX[20];
float appleY[20];
float plateX, plateY;

float armHeight = -7.5;




void setup() {
  setupSerial();
  robotPrepare();
}


void loop() {

  robotPrepare();

  if (digitalRead(startButton) == HIGH)
  {
    readJson();
    if (json.length > 0)
      deconstructJson();
  }

  for (int i = 0; i < appleNum; i++)
  {
    catchPrepare(atan(appleY[i], appleX[i]));
    locateXY(appleX[i],  appleY[i]);
    catches();
    cathcPrepare(atan(appleY[i] / float(appleX[i])));
    locateByXY(plateX, plateY);
    releases();
  }
}

void setupSerial() {
  Serial.begin(9600);
  mySerial.begin(115200);
}


void readJson() {
  if (Serial.available() > 0)
  {
    json = Serial.readString();
  }
}
void deconstructJson() {
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  //      if (!root.success()) {
  //        Serial.println("failed");
  //        return;
  app_num = root[0] - 48;
  Serial.println(appleNum);
  for (int i = 0; i < appleNum; i++) {
    appleX[i] = root[1][i][0];
    appleY[i] = root[1][i][1];
  }
  plateX = root[2][0][0];
  plateY = root[2][0][1];

  json = "";
}


void armPrepare() {
  //弯曲
  mySerial.println("{#004P1500T0250!#005P1500T0250!}");
  delay(250);
  locate(15, 10, 0);
}
void armReset() {
  //复位
  mySerial.println("{#001P1500T1000!#002P1500T1000!#003P1500T1000!#004P1500T1000!#005P1500T1000!}");
  delay(1000);
}

void catchPrepare(float beta) {
  locate(20, 0, beta);
  mySerial.println("{#004P1500T1000!#005P0800T1000!}");
  delay(1000);
}
void catches() {
  mySerial.println("#005P1700T1000!");
  delay(1000);
}
void releases() {
  mySerial.println("{#005P1000T1000!}");
  delay(1000);
}

void locateXY(float x, float y) {
  float radius = distanceFromOrigin(x, y) + 1.8, facingTheta =  atan(y / float(x));
  locate(radius, armHeight, facingTheta);
}
void locate(float distance, float height, float alpha) {
  float len = distanceFromOrigin(distance, height);
  float jiaodu = atan(height / distance);
  int[4] pwn;

  getEveryGearPwn();
  String[4] temp;
  for (int i=0;i<4;i++){
    temp[i] = getStirngByPwn(pwn[i]);
  }
  sendGearMessage();
}
void getEveryGearPwn() {
  pwn[0] = thetaToPwn((alpha) );
  pwn[1] = thetaToPwn(PI / float(2) - jiaodu - acos((25 * pow(len, 2) - 6196) / float(40 * (13 * len + 200))));
  pwn[2] = thetaToPwn(acos((25 * pow(len, 2) + 421) / float(50 * (15 * len + 104))));
  pwn[3] = thetaToPwn(acos((25 * pow(len, 2) - 6196) / float(40 * (13 * len + 200)))  + 20 / float(180) * PI) ;
}
String getStringByPwn(int pwn) {
  String string = "0000";
  string += pwn;
  return(string.substring(temp0.length() - 4));
}
void sendGearMessage() {
  mySerial.println("{#000P" + temp[0] + "T1000!#001P" + temp[1] + "T1000!#002P" + temp[2] + "T1000!#003P" + temp[3] + "T1000!}");
  delay(1000);
}

float distanceFromOrigin(float x, float y) {
  return sqrt(pow(x, 2) + pow(y , 2));
}
int thetaToPwn(float jiaodu) {
  return (int(1500 + 2000 * jiaodu / (PI * 3 / 2)));
}




