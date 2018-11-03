#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial mySerial(10, 2);

//float h = -1, j = 0;
//float rs = 20, rb = 25;
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
float x = 20, y = 0;
float distance = sqrt(pow(x, 2) + pow(y, 2)), height = -10, theta = atan(y / x);




void setup() {
  for (i = 0; i < 7; i++) {
    ha[i] = dha[i] + sth;
  }
  Serial.begin(9600);
  mySerial.begin(115200);
  mySerial.println("#001PMOD2!");
  reset();
  prepare();
  precatch(theta);
  locate(distance, height, theta);
}

void loop() {


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

void locate(float distance, float height, float omiga) {
  //将夹子对准某位置
  //三个参数分别为距离，高度，角度，前两个定性（就是需要尝试），最后一个定量
  float len = sqrt(pow((distance + 1.8), 2) + pow(height, 2));
  float theta = 0, alpha = 0, beta = 0;
  alpha = acos (0.0000300481 * (1320 * len - sqrt(78400 * pow(len, 2) + 114216960)));
  beta = asin (4.8 / 10 * sin(alpha));


  int pwn0, pwn1, pwn2, pwn3;
  //  for (int i=0;i<1000;i++){
  //    alpha = 10*cos(beta)+25.4*cos(alpha)-len+alpha;
  //    beta = 4.6*sin(alpha)-10*sin(beta)+beta;
  //  }
  pwn0 = thetaToPwn(omiga);
  pwn1 = thetaToPwn(PI / 2 - theta - alpha);
  pwn2 = thetaToPwn(alpha - beta);
  pwn3 = thetaToPwn(alpha + beta + float(12) / 180 * PI);

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

    mySerial.println("{#000P" + temp0 + "T" + temp4 + "!#001P" + temp1 + "T" + temp4 +  "!#002P" + temp2 + "T" + temp4 + "!#003P" + temp3  + "T" + temp4 + "!}");
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




