#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>//软串口头文件名
#include <DFPlayer_Mini_Mp3.h>//MP3头文件名

const char* ssid = "niehen_pc";
const char* password = "*";
const char* mqtt_server = "*";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
String tmpTopic;
String strplayload;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
//  Serial.println();
//  Serial.print("Connecting to ");
//  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
   // Serial.print(".");
  }

randomSeed(micros());
  //Serial.println(WiFi.localIP());
}
void callback(char* topic, byte* payload, unsigned int length) {
  
 tmpTopic = String((char *)topic);
 payload[length] = '\0';
 strplayload  = String((char*)payload);
  Serial.println("get the order");
  client.publish("itgzs/mp3player/status", "get older ");
  if (strplayload == "off") {  // 判断是否是off 关闭指令
      mp3_stop ();
     
      client.publish("itgzs/mp3player/status", "mp3 stop");
  }else {
    int isint = 0;// 如果有字母 就为1 全数字就为0
    for (int i=0; i< strplayload.length();i++)  // 检测接收到的命令里面是否含有 非数字的字符
  {
    if (!isDigit(strplayload[i]))
    {
      isint = 1;
      break;
      }
    }
    if(isint == 0){
      int older_mp3 = strplayload.toInt() ; // 转换为 int类型
        if(older_mp3 >=1 && older_mp3<=20){
          mp3_play (older_mp3);
           Serial.println("mp3 running");
           Serial.println(older_mp3);
          client.publish("itgzs/mp3player/status", "mp3 is running");
          delay(2000);
        }
    }
    }
  }

                                                                                                                                                                                                                                                                                                                                                                                                                                                    

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
     Serial.println("connected");
      
      client.publish("itgzs/mp3player/status", "init...");
      // ... and resubscribe
      client.subscribe("itgzs/mp3player/older");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
   Serial.begin(9600);
   setup_wifi();
   mp3_set_serial(Serial);//定义MP3串口为Arduino串口。
  
   mp3_set_volume(30);//设置音量。
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}

void loop() {
//  if (value == 4){
//    client.publish("mp3/status", "music one");
//    mp3_play (1);//播放第一首。
//        delay (2000);//播放6秒。
//        value = 5;
//  }

   if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

