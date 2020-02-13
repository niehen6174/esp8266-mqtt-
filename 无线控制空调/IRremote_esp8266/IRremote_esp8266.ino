// 作者 ： 涅痕 
// 更多IOT开发 访问 niehen.cn
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* ssid = "niehen_pc";
const char* password = "#";
const char* mqtt_server = "*";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
String tmpTopic;
String strplayload;

#define IR_LED 4  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(IR_LED);  // Set the GPIO to be used to sending the message.
char*  order_infor[] ={"on","off","tema","tems","warm","cold","winda","winds"};  // 控制的温度
long enc[] ={0xFF30CF,0xFF18E7,0xFF7A85,0xFF10EF,0xFF38C7,0xFF5AA5,0xFF42BD,0xFF4AB5};//发射的红外 十六进制值  自定的 实际需要去获取到要遥控器的发送信号

void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  irsend.begin();
 // Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void callback(char* topic, byte* payload, unsigned int length) {

 tmpTopic = String((char *)topic);
 payload[length] = '\0';
 strplayload  = String((char*)payload);

  client.publish("mp3player/status", "get older ");

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
      int older_air = strplayload.toInt() ; // 转换为 int类型
        if(older_air >=1 && older_air<=8){
          irsend.sendSony(enc[older_air - 1], 12);
          client.publish("air_cond/status", "IR remote is running");
          client.publish("air_cond/status", order_infor[older_air -1]);
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

      client.publish("air_cond/status", "init...");
      // ... and resubscribe
      client.subscribe("air_cond/older");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

