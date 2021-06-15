#include "HX711.h"					//调用24bitAD HX711库
#include <SoftwareSerial.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>
//定义称重装置
HX711 HX711_CH0(2, 3, 106.5); //SCK,DT,GapValue
//SCK引脚用于arduino和HX711模块通讯的时序提供
//DT引脚用于从HX711读取AD的数据
//GapValue用于校准输出的重量值，如果数值偏大就加大该值，如果数据偏小就减小该值

//定义时钟DS1302
ThreeWire myWire(6,7,5); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
String comdata;
int getsize;//手机传来的字符串长度
float getTime;//手机传来的时间
int getWeight = 0;//手机传来的重量
int flag = 0;//是否达到过重量
float t1 = 0;//系统时间的整形表示
int a = 8;
int b =9;
int c =10;
int d = 11;
int sudu = 2;
String time_weight;
long Weight = 0;		//定义一个变量用于存放承重的重量，单位为g
SoftwareSerial mySerial(13, 12); // RX, TX
 
void setup()
{ 
  //Serial.begin(115200);
  Serial.begin(57600);
  while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only 
  }
  mySerial.begin(115200);
  pinMode(a,OUTPUT);
  pinMode(b,OUTPUT);
  pinMode(c,OUTPUT);
  pinMode(d,OUTPUT);
  Rtc.Begin();
  

  HX711_CH0.begin();					//读取传感器支架毛重  
  delay(3000);								//延时3s用于传感器稳定
  HX711_CH0.begin();					//重新读取传感器支架毛重用于后续计算

  
}
void steeper(){
   pin_a();
   delay(sudu);
   pin_b();
   delay(sudu);
   pin_c();
   delay(sudu);
   pin_d();
   delay(sudu);
   pin_stop();
   delay(sudu);
}
void loop()
{
  while (mySerial.available() > 0)  
    {
        comdata += char(mySerial.read());
        delay(2);
    }
    if (comdata.length() >= 43)
    {
        comdata.trim();
        getWeight = 0;
        //获得重量
        for(int temp = 0;temp<comdata.length();temp++){
          if(comdata[temp] == '_'){
            for(int j = temp+1;j<comdata.length();j++){
              getWeight *= 10;
              getWeight += comdata[j]-'0';
            }
            break;
          }
        }
        //获得时间
        for(int temp =0;temp<comdata.length();temp++){
          if(comdata[temp] == ':'){
            getTime = ((comdata[temp+2]-'0')*10+comdata[temp+3]-'0')*3600+((comdata[temp+7]-'0')*10+comdata[temp+8]-'0')*60+((comdata[temp+12]-'0')*10+comdata[temp+13]-'0');
            break;
          }
        }
        RtcDateTime now = Rtc.GetDateTime();
        getDateTime(now);
        Serial.println(t1);
        Serial.println(getTime);
        Serial.println(comdata);
        Serial.println(getWeight);
        while(t1<getTime){
          RtcDateTime now = Rtc.GetDateTime();
          getDateTime(now);
          Weight = HX711_CH0.Get_Weight();
          Serial.println(Weight);
        }
        if(t1 >= getTime){
            Weight = HX711_CH0.Get_Weight();    //采样当前传感器重量，该重量已经自动去皮，去皮值根据初始化程序中采样的值计算。
            while(getWeight >= Weight){
              for(int i=0;i<10;i++){
                steeper();
              }
              Weight = HX711_CH0.Get_Weight(); 
            }
            Serial.println(Weight);
         }
    }
    comdata = "";
}
void pin_a(){
  digitalWrite(a,HIGH);
  digitalWrite(b,LOW);
  digitalWrite(c,LOW);
  digitalWrite(d,LOW);
}

void pin_b(){
  digitalWrite(a,LOW);
  digitalWrite(b,HIGH);
  digitalWrite(c,LOW);
  digitalWrite(d,LOW);
}
void pin_c(){
  digitalWrite(a,LOW);
  digitalWrite(b,LOW);
  digitalWrite(c,HIGH);
  digitalWrite(d,LOW);
}
void pin_d(){
  digitalWrite(a,LOW);
  digitalWrite(b,LOW);
  digitalWrite(c,LOW);
  digitalWrite(d,HIGH);
}
void pin_stop(){
  digitalWrite(a,LOW);
  digitalWrite(b,LOW);
  digitalWrite(c,LOW);
  digitalWrite(d,LOW);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))
void getDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.println(datestring);
    t1 = ((datestring[11]-'0')*10+datestring[12]-'0')*3600+((datestring[14]-'0')*10+datestring[15]-'0')*60+((datestring[17]-'0')*10+datestring[18]-'0');
    //Serial.println(t1);
}
