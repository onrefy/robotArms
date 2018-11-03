#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial mySerial(10, 2);

float h = -1, j = 0;
float rs = 20, rb = 25;
int mode = 4, value = 0, tvalue = 0;
int premode = 0 , prevalue = 0, nextmode = 4;
String json = "";
int change = 1;
int times = 10, i = 0;
bool judge = true;
bool readmode = false;


//position data



//shape
//catch position
float l0 = 23, h0 = 0.5;




//circular cylinder
float l1 = 10, h1 = 4, th1 = 90;
//triangle
float l2 = 15, h2 = 4, th2 = 70;
//rectangle
float l3 = 23, h3 = 3, th3 = 45;
float th0 = -115;





//color
float la[] = {22, 22, 24.5, 27.5, 30, 33, 34.5};
float sth = -0.9;
float dha[] = { 1, 1, 0.7, 1, 0.3, -0.5, 0};
float ha[7];
float tha1 = 0;






void setup() {
  for (i = 0; i < 7; i++) {
    ha[i] = dha[i] + sth;
  }
  Serial.begin(9600);
  mySerial.begin(115200);
  mySerial.println("#001PMOD2!");
  reset();
  prepare();
  locate(20,0,0);
}

void loop() {
  //  Serial.println(mode);/
  if (!readmode) {
    //判断是否变化

    //识别模式的变化
    mode = nextmode;
    while (Serial.available() > 0)
    {
      Serial.read();
    }
    Serial.println(mode);
    readmode = true;
  }

  int buttonState = digitalRead(3);
  //  Serial.println(buttonState);
  if (buttonState == HIGH) {
    if (mode >= 1) {nextmode = 0;prepare();}
    else nextmode = mode + 1;
    switch (nextmode) {
      case 0:
        value = 3;
        precatch(th0);
        break;
      case 1:
        value = 7;
        preknock();
        break;
        //      case 2:
        //        locate(15, 10, 90);
        //        released();
        //        mySerial.println("{#004P0834T0250!#005P1500T0250!}");
        //        delay(250);
        //        locate(len, gao, -20);
    }
    premode = mode;
    prevalue = value;
    return;
  }

  if (readmode) {
    while (Serial.available() > 0)
    {
      char c = Serial.read();
      json += c;
      delay(2);
      judge = true;
    }
    if (!json.equals(""))  readmode = false;
  }
  //接收数据
  if (!readmode) {
    if (json.length() > 0) {
      //json=json.substring(json.length() - 23);
      StaticJsonBuffer<500> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(json);


      String temp1 = root["mode"];
      String temp2 = root["value"];
      //      mode = temp1.toInt();
      value = temp2.toInt();
      json = "";
    }
    json = "";

    //开始动作
    if (change == 1) {
      switch (mode) {
        case 0:
          switch (value) {
            case 0:
              locate(l0, h0, th0);
              catching();
              catched();
              locate(l1, h1, th1);
              released();
              delay(3000);
              precatch(th0);
              waitForRotate();
              break;
            case 1:
              locate(l0, h0, th0);
              catching();
              catched();
              locate(l2, h2, th2);
              released();
              delay(3000);
              precatch(th0);
              break;
            case 2:
              locate(l0, h0, th0);
              catching();
              catched();
              locate(l3, h3, th3);
              released();
              delay(3000);
              precatch(th0);
              waitForRotate();
              break;
            case 3:
              break;

          }


          break;
        case 1:
          if (value == 7) break;
          else {
            tlocate(la[value], ha[value], tha1, PI / 2, 250*abs(tvalue-value));
            tvalue = value;
            knock();
            waitForRotate();

          }
          //          switch (value) {
          //            case 0:
          //              tlocate(24, q - 1, 90, PI / 2, 250);
          //              knock();
          //              waitForRotate();
          //              break;
          //            case 1:
          //              tlocate(24, q, 90, PI / 2, 250);
          //              knock();
          //              waitForRotate();
          //              break;
          //
          //            case 2:
          //              tlocate(26.5, q, 90, PI / 2, 250);
          //              knock();
          //              waitForRotate();
          //              break;
          //
          //            case 3:
          //              tlocate(28, q, 90, PI / 2, 250);
          //              knock();
          //              waitForRotate();
          //              break;
          //
          //            case 4:
          //              tlocate(30, q-0.7, 90, PI / 2, 250);
          //              knock();
          //              waitForRotate();
          //              break;
          //
          //            case 5:
          //              tlocate(33, q-1.5, 90, PI / 2, 250);
          //              knock();
          //              waitForRotate();
          //              break;
          //
          //            case 6:
          //              tlocate(34.5, q-0.8 , 90, PI / 2, 250);
          //              knock();
          //              waitForRotate();
          //              break;
          //
          //            case 7:
          //
          //              break;



      }
      //        case 2:
      //          if (value == 1) {
      //
      //            dig();
      //            locate(15, 10, -20);
      //            waitForRotate();
      //          }
    }

  }

}











void waitForRotate() {
  delay(2500);
}

void reset() {
  //复位
  mySerial.println("{#001P1500T1000!#002P1500T1000!#003P1500T1000!#004P1500T1000!#005P1500T1000!}");
  delay(1000);
}

void prepare() {
  //弯曲
  mySerial.println("{#004P1500T0250!#005P1500T0250!}");
  delay(250);
  locate(15, 10, 0);
}

void locate(float distance, float height, float alpha) {
  //将夹子对准某位置
  //三个参数分别为距离，高度，角度，前两个定性（就是需要尝试），最后一个定量
  float len = sqrt(pow(distance, 2) + pow(height, 2));
  float theta, jiaodu = atan(height / distance);
  int pwn0, pwn1, pwn2, pwn3;

  pwn0 = int(alpha / 270 * 2000) + 1500;
  pwn2 = thetaToPwn(acos((len - 10) / 2 / 10));
  pwn1 = thetaToPwn(0 - acos((len - 10) / 2 / 10) - jiaodu + PI / 2);

  String temp0 = "0000";
  String temp1 = "0000";
  String temp2 = "0000";
  temp0 += pwn0;
  temp1 += pwn1;
  temp2 += pwn2;


  temp0 = temp0.substring(temp0.length() - 4);
  temp1 = temp1.substring(temp1.length() - 4);
  temp2 = temp2.substring(temp2.length() - 4);

  mySerial.println("{#000P" + temp0 + "T1000!#001P" + temp1 + "T1000!#002P" + temp2 + "T1000!#003P" + temp2 + "T1000!}");
  delay(1000);

}

void tlocate(float distance, float height, float alpha, float zitai, int t) {
  float x = distance - 15 * sin(zitai);
  float y = height + 15 * cos(zitai);
  float len = sqrt(pow(x, 2) + pow(y, 2));
  float jiao = atan(y / x);
  int pwn0, pwn1, pwn2, pwn3;
  if (len <= 35) {
    pwn0 = int(alpha / 270 * 2000) + 1500;
    pwn2 = thetaToPwn(2 * acos(len / 2 / 10));
    pwn1 = thetaToPwn(0 - acos(len / 2 / 10) - jiao + PI / 2);
    pwn3 = thetaToPwn(PI - zitai - 2 * acos(len / 2 / 10) - (0 - acos(len / 2 / 10) - jiao + PI / 2));

    String temp0 = "0000";
    String temp1 = "0000";
    String temp2 = "0000";
    String temp3 = "0000";
    String temp4 = "0000";
    String temp5 = "0000";
    temp0 += pwn0;
    temp1 += pwn1;
    temp2 += pwn2;
    temp3 += pwn3;
    temp4 += t;
    temp5 += t / 2;

    temp0 = temp0.substring(temp0.length() - 4);
    temp1 = temp1.substring(temp1.length() - 4);
    temp2 = temp2.substring(temp2.length() - 4);
    temp3 = temp3.substring(temp3.length() - 4);
    temp4 = temp4.substring(temp4.length() - 4);
    temp5 =  temp5.substring(temp5.length() - 4);

     mySerial.println("{#000P" + temp0 + "T" + temp4 +"!#001P" + temp1 + "T" + temp4 +  "!#002P" + temp2 + "T" + temp4 + "!#003P" + temp3  + "T" + temp4 + "!}");
    //mySerial.println("{#000P" + temp0 + "T" + temp4 +"!#003P" + temp3 + "T" + temp4 +  "!#002P" + temp2 + "T" + temp4 + "!#001P" + temp1  + "T" + temp4 + "!}");
    delay(t);
  }
}

//void dig() {
//  //扒
//  locate( len, 2, -15);
//}

void preknock() {
  //敲击之前的姿态调整
  mySerial.println("{#004P0834T0250!#005P1000T0250!}");
  delay(3000);
  mySerial.println("{#004P0800T0250!#005P2000T0250!}");
  delay(250);
  tlocate(la[0], ha[0], tha1, PI / 2, 2000);

}

void knock() {
  mySerial.println("#004P2500T0150!");
  delay(150);
  mySerial.println("#004P0800T0250!");
  delay(250);
}

void precatch(int beta) {
  locate(15, 10, beta);
  mySerial.println("{#004P1500T1000!#005P0800T1000!}");
  delay(1000);

}

void catching() {
  mySerial.println("#005P1700T1000!");
  delay(1000);
}
void catched() {
  locate(15, 10, th0);
}

void released() {
  mySerial.println("{#005P1000T1000!}");
  delay(1000);
}
int thetaToPwn(float jiaodu) {
  return (int(1500 + 2000 * jiaodu / (PI * 3 / 2)));
}




