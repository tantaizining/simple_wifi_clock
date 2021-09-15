#include <ESP8266WiFi.h>                    //需要安装开发板文件
#include <TM1637.h>                         //需要下载库文件

/*这个TM1637的库有很多，但每个库的具体函数都不太一样，大家如果使用不同的库，编译很有可能会过不去，这个时候大家看看
 * 自己的库文件里封装的函数到底怎么写。举个例子，我之前的库设置亮度是setBrightness（）；现在的库是set（）；
 * 还请大家都耐心看看，多调试几次，区别大多都在函数名上面。
 * 作为小白，也是刚刚接触esp8266，有不足还请大佬指出，一定虚心听取
 */
#include <ESP8266HTTPClient.h>
#define CLK D3
#define DIO D4
TM1637 tm1637(CLK,DIO);


//在这里输入你家的WiFi名字和密码
const char* ssid     = "/*你家WiFi的名字*/"; 

const char* password = "/*你家WiFi的密码*/";   




HTTPClient http;

String GetUrl;

String response;





void setup() {

  tm1637.init();
  tm1637.set(3);
  

  // 连接到你家的WiFi

  delay(500);

  Serial.begin(115200);                    //串口监视器波特率也要调到115200



  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);



  Serial.println("");
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);

    Serial.print(".");

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


        if(response.substring(31,32).toInt()%2==1)
        tm1637.point(1);
        else tm1637.point(0);
        tm1637.display(0,response.substring(24,25).toInt());
        tm1637.display(1,response.substring(25,26).toInt());
        tm1637.display(2,response.substring(27,28).toInt());
        tm1637.display(3,response.substring(28,29).toInt());
        delay(100);
        

      }

  } else {

      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());

  }

  http.end();

  delay(900);

}
