 
#include <SoftwareSerial.h>//软串口头文件名
#include <DFPlayer_Mini_Mp3.h>//MP3头文件名
 
void setup () {
        Serial.begin (9600);//串口波特率，9600
        mp3_set_serial(Serial);//定义MP3串口为Arduino串口。
        mp3_set_volume(30);//设置音量。
}
 
void loop () {        
        mp3_play (1);//播放第一首。
        delay (6000);//播放6秒。
        mp3_next ();//播放下一首。
        delay (6000);//播放6秒。
        mp3_prev ();//上一曲
        delay (6000);//播放6秒。
        mp3_play (3);//播放第3首。
        delay (6000);//播放6秒。
}
