#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9
int mSwitch = 2 ; int LEDRed = 3 ; int LEDGreen = 4 ;//设定电磁阀开关及红绿灯Pin值
MFRC522 mfrc522 ( SS_PIN , RST_PIN ); //建立MFRC522.<br />
void setup () {
pinMode ( LEDRed , OUTPUT );
pinMode ( LEDGreen , OUTPUT );
pinMode ( mSwitch , OUTPUT );
Serial . begin ( 9600 ); //设定串列通讯速率
SPI . begin (); //设定SPI bus
mfrc522 . PCD_Init (); //设定MFRC522
Serial . println ( " Put your card close to the reader… " );
Serial . println ();
}
void loop () {
digitalWrite ( LEDGreen , LOW );
digitalWrite ( LEDRed , LOW );
digitalWrite ( mSwitch , LOW );
if ( ! mfrc522 . PICC_IsNewCardPresent ()) { return ; } //
if ( ! mfrc522 . PICC_ReadCardSerial ()) { return ; } //Show UID on serial monitor
Serial.print ( "Card No. :"
) ;
String content = "" ;
byte letter ;
for ( byte i = 0 ; i < mfrc522 . uid . size ; i ++)
{
Serial . print ( mfrc522 . uid . uidByte [ i ] < 0x10 ? " 0" : " " );
Serial . print ( mfrc522 . uid . uidByte [ i ], HEX );
content . concat ( String ( mfrc522 . uid . uidByte [ i ] < 0x10 ? " 0" : " " ));
content . concat ( String ( mfrc522 . uid . uidByte [ i ], HEX ));
}
Serial . println ();
Serial . print ( " Message : " );
content . toUpperCase ();<br />
//需要改成可通行的卡号
if ( content . substring ( 1 ) == "FA 91 83 08" ) {
Serial . println ( " Pass… " );
Serial . println (); //亮绿灯,电磁阀开
digitalWrite ( LEDGreen , HIGH );
digitalWrite ( LEDRed , LOW );
digitalWrite ( mSwitch , HIGH );
delay ( 3000 );
}
else { Serial . println ( " Stop…" ); //亮红灯,电磁阀关
digitalWrite ( LEDGreen , LOW );
digitalWrite ( LEDRed , HIGH );
digitalWrite ( mSwitch , LOW );
delay ( 3000 );
}
}
