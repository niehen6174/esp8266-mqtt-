// 作者 ： 涅痕 
// 更多IOT开发 访问 niehen.cn
#include <Stepper.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const int STEPS_PER_ROTOR_REV = 32; 
const int GEAR_REDUCTION = 64;
const float STEPS_PER_OUT_REV = 400;// 电机外部输出轴旋转一周步数 （2048）
const int stepsPerRevolution = 200;
//Stepper steppermotor(STEPS_PER_OUT_REV,  D1, D2, D5, D6); 
Stepper steppermotor(STEPS_PER_OUT_REV,  5, 4, 14, 12); 

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
bool start_on = false;

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
 int isint = 0;
String inString = "";
int older_circle = 0;
client.publish("curtain/status", "curtain get the older");

for (int i=1; i< strplayload.length();i++)  // 检测接收到的命令是否符合
  {
    if (strplayload[0] == '-') {
      inString += strplayload[i];
    }
    if (!isDigit(strplayload[i]))
    {
      isint = 1;
      break;
      }    
    }
if(isint == 0){
if (strplayload[0] == '-') {
  // 反转
  older_circle = inString.toInt();
  value = (-1) * older_circle; 
  start_on = true;
}else{
  // 正转
  older_circle = strplayload.toInt();
  value =  older_circle; 
  start_on = true;
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

      client.publish("curtain/status", "init...");
      // ... and resubscribe
      client.subscribe("curtain/older");
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
 steppermotor.setSpeed(80);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
if(start_on ){

  if(value >0)
  {
  steppermotor.step((value)*STEPS_PER_OUT_REV);
  client.publish("curtain/status", "curtain runing");
    }else if(value <0){ 
    steppermotor.step((value)*STEPS_PER_OUT_REV);
    client.publish("curtain/status", "curtain runing");
      }
  start_on = false;
  }
}

