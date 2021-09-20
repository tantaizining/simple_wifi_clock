#include <ESP8266WiFi.h>                    //需要安装开发板文件
#include <TM1637.h>                         //需要下载库文件
#include <ArduinoJson.h>

/*这个TM1637的库有很多，但每个库的具体函数都不太一样，大家如果使用不同的库，编译很有可能会过不去，这个时候大家看看
 * 自己的库文件里封装的函数到底怎么写。举个例子，我之前的库设置亮度是setBrightness（）；现在的库是set（）；
 * 还请大家都耐心看看，多调试几次，区别大多都在函数名上面。
 * 作为小白，也是刚刚接触esp8266，有不足还请大佬指出
 */
#include <ESP8266HTTPClient.h>
#define CLK D2
#define DIO D4
TM1637 tm1637(CLK,DIO);

void smartConfig();



//在这里输入你家的WiFi名字和密码
const char* ssid     = "li"; 

const char* password = "wdyllxy";   




HTTPClient http;

String GetUrl;

String response;





void setup() {

  bool WiFi_status= true;
  tm1637.init();
  tm1637.set(3);
  tm1637.point(1);
  tm1637.display(0,8);
  tm1637.display(1,8);
  tm1637.display(2,8);
  tm1637.display(3,8);                  //自检
  delay(1500);
  
  
  

  // 连接到你家的WiFi

  delay(500);

  Serial.begin(115200);                    //串口监视器波特率也要调到115200



  WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);



  Serial.println("");
  Serial.print("connecting");
  int i=0;
  while (WiFi.status() != WL_CONNECTED) {
    if(WiFi_status)
    {
    delay(500);
    Serial.print(".");
    i=i+1;            
    if(i>20){
      WiFi_status=false;
      Serial.println("\nplease turn to smartconfig");
      }
    }

    else{smartConfig();}

  }



  Serial.println("");

  Serial.println("WiFi connected");

  Serial.println("IP address: ");

  Serial.println(WiFi.localIP());                                                           



  // 连接苏宁网站的授时网页

  GetUrl = "http://quan.suning.com/getSysTime.do";

  http.setTimeout(5000);

  http.begin(GetUrl);


}



void loop() {

  // 从网站获得网页内容

  int httpCode = http.GET();

  if (httpCode > 0) {

      //Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {

        //读取响应内容

        response = http.getString();

        //Serial.println(response);

        Serial.println(response.substring(13,23));
        Serial.println(response.substring(24,32));
        Serial.println("");


        if(digitalRead(D3)==1){
        if(response.substring(31,32).toInt()%2==1)
        tm1637.point(1);
        else tm1637.point(0);
        int time1;
        if(response.substring(24,25).toInt()==0)
        time1=20;
        else
        time1=response.substring(24,25).toInt();
        tm1637.display(0,time1);
        tm1637.display(1,response.substring(25,26).toInt());
        tm1637.display(2,response.substring(27,28).toInt());
        tm1637.display(3,response.substring(28,29).toInt());
        int t=10*response.substring(24,25).toInt()+response.substring(25,26).toInt();
        int brightness=analogRead(A0);
        Serial.println(analogRead(A0));
        /*if(brightness>160)
        {tm1637.set(7);}
        else if(brightness>130||brightness==0)
        {tm1637.set(3);}
        else 
        {tm1637.set(1);}                                  */
        delay(50);                                          
        }
        else{                                                //日期
        tm1637.point(0);
        if(response.substring(18,19).toInt()==0){
          tm1637.display(0,20);                             //不显示
          tm1637.display(1,response.substring(19,20).toInt());
        }
        else{
          tm1637.display(0,response.substring(18,19).toInt());
          tm1637.display(1,response.substring(19,20).toInt());
        }
        tm1637.display(2,response.substring(21,22).toInt());
        tm1637.display(3,response.substring(22,23).toInt());
        delay(3000);
        }

      }

  } else {

      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      tm1637.point(0);
      tm1637.display(0,14);
      tm1637.display(1,36);
      tm1637.display(2,36);                  //96,上圈
      tm1637.display(3,96);

  }

  http.end();

  delay(900);

}


void smartConfig()
{
     WiFi.mode(WIFI_STA);
     Serial.println("waiting for smartConfig");
     WiFi.beginSmartConfig();
     Serial.println("");
     while(1){
      digitalWrite(D4,1);
      delay(500);
      digitalWrite(D4,0);
      delay(500);
      if(WiFi.smartConfigDone()){
        Serial.println("smartConfig completed");
        Serial.println(WiFi.localIP());
        Serial.printf("SSID:%s\r\n",WiFi.SSID().c_str());
        Serial.printf("PASSWORD:%s\r\n",WiFi.psk().c_str());
        break;
        }
      }
}
