// 作者 ： 涅痕 
// 更多IOT开发 访问 niehen.cn
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PIN 2
#define NUM_LEDS 8
#define BRIGHTNESS 50
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);//  RGBW 与RGB在创建灯带时不一样
const char* ssid = "niehen_pc";
const char* password = "*";
const char* mqtt_server = "*";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 1;
String tmpTopic;
String strplayload;
int bright = 0;
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

randomSeed(micros());
  Serial.println(WiFi.localIP());
}
void callback(char* topic, byte* payload, unsigned int length) {

 tmpTopic = String((char *)topic);
 payload[length] = '\0';
 strplayload  = String((char*)payload);


  client.publish("led_color/status", "get older ");
  if (strplayload == "off") {  // 判断是否是off 关闭指令
      value = 0;
      client.publish("led_color/status", "led_play stop");
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
      int older_led = strplayload.toInt() ; // 转换为 int类型
        if(older_led >=0 && older_led<=2){
          value = older_led;
           char* pub_status  = "led_play is running";
           pub_status[19] = '0' + value;
          client.publish("led_color/status",pub_status);

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

      client.publish("led_color/status", "init...");
      // ... and resubscribe
      client.subscribe("led_color/transform/older");
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
   Serial.begin(115200);
   setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  pixels.setBrightness(BRIGHTNESS);
  pixels.begin();
  pulseWhite(1,5);
  delay(10);
  pulseWhite(0,5);// 出现bug 开灯后 有一个灯亮 这是解决办法
}

void loop() {
  if (value == 0){
   pulseWhite(0,20); 
  }else if(value == 1)  
  rainbow(20);
  else if(value == 2)  
  rainbowCycle(20);

   if (!client.connected()) {
    reconnect();
  }
  client.loop();

}

void pulseWhite(uint8_t result,uint8_t wait) {  // 第一个参数为要变为的亮度值 第二个参数为变化间隔的时间单位为毫秒
      for(uint16_t i=0; i<pixels.numPixels(); i++) {
          pixels.setPixelColor(i, pixels.Color(result,result,result) );
        }
        delay(wait);
        pixels.show();

}
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}


