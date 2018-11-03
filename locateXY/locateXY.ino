#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial mySerial(10, 2);

int start_button = 3;

String json = "";

int app_num = 1;
float app_x[20];
float app_y[20];
float plt_x, plt_y;

float tol_hei = -7.5;




void setup() {
  Serial.begin(9600);
  mySerial.begin(115200);
  prepare();
}

int s = 0;

void loop() {

  prepare();

  //start working
  if (digitalRead(start_button) == HIGH)
  {
    //read app_data
    //    Serial.println(1);

    if (Serial.available() > 0)
    {
      json = Serial.readString();
    }
    if (json.length() > 0) {
      StaticJsonBuffer<500> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(json);
      //      if (!root.success()) {
      //        Serial.println("failed");
      //        return;
      app_num = root[0]-48;
      Serial.println(app_num);
      for (int i = 0; i < app_num; i++) {
        app_x[i] = root[1][i][0];
        app_y[i] = root[1][i][1];
      }
      plt_x = root[2][0][0];
      plt_y = root[2][0][1];

      json = "";
    }

  }

  //doing
  //    for (int i = 0; i < app_num; i++)
  //    {
  //      precatch(atan(app_y[i] / app_x[i]));
  //      locateXY(app_x[i],  app_y[i]);
  //      catched();
  //      precatch(atan(app_y[i] / float(app_x[i])));
  //
  //      locateXY(plt_x, plt_y);
  //      released();
  //    }
}



void locateXY(float x, float y) {
  float ra = sqrt(pow(x, 2) + pow(y , 2)) + 1.8, de =  atan(y / float(x));
  locate(ra, tol_hei, de);
}

void prepare() {
  //弯曲
  mySerial.println("{#004P1500T0250!#005P1500T0250!}");
  delay(250);
  locate(15, 10, 0);
}

void precatch(float beta) {
  locate(20, 0, beta);
  mySerial.println("{#004P1500T1000!#005P0800T1000!}");
  delay(1000);
}
void catched() {
  mySerial.println("#005P1700T1000!");
  delay(1000);
}

void locate(float distance, float height, float alpha) {
  //将夹子对准某位置
  //三个参数分别为距离，高度，角度，前两个定性（就是需要尝试），最后一个定量
  float len = sqrt(pow(distance, 2) + pow(height, 2));
  float jiaodu = atan(height / distance);
  int pwn0, pwn1, pwn2, pwn3;

  pwn0 = thetaToPwn((alpha) );
  pwn1 = thetaToPwn(PI / float(2) - jiaodu - acos((25 * pow(len, 2) - 6196) / float(40 * (13 * len + 200))));
  pwn2 = thetaToPwn(acos((25 * pow(len, 2) + 421) / float(50 * (15 * len + 104))));
  pwn3 = thetaToPwn(acos((25 * pow(len, 2) - 6196) / float(40 * (13 * len + 200)))  + 20 / float(180) * PI) ;

  String temp0 = "0000";
  String temp1 = "0000";
  String temp2 = "0000";
  String temp3 = "0000";


  temp0 += pwn0;
  temp1 += pwn1;
  temp2 += pwn2;
  temp3 += pwn3;


  temp0 = temp0.substring(temp0.length() - 4);
  temp1 = temp1.substring(temp1.length() - 4);
  temp2 = temp2.substring(temp2.length() - 4);
  temp3 = temp3.substring(temp3.length() - 4);

  mySerial.println("{#000P" + temp0 + "T1000!#001P" + temp1 + "T1000!#002P" + temp2 + "T1000!#003P" + temp3 + "T1000!}");
  delay(1000);

}
void reset() {
  //复位
  mySerial.println("{#001P1500T1000!#002P1500T1000!#003P1500T1000!#004P1500T1000!#005P1500T1000!}");
  delay(1000);
}
void released() {
  mySerial.println("{#005P1000T1000!}");
  delay(1000);
}


int thetaToPwn(float jiaodu) {
  return (int(1500 + 2000 * jiaodu / (PI * 3 / 2)));
}




