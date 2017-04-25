# IoTTalkArduinoWithESP8266 - Arduino_ESP8266_csmapi.ino

This is a case using Adruino + ESP8266 to communicate to IoTTalk.<BR>
In  Arduino_ESP8266_csmapi.ino, there are 4 function to communicate to IoTTalk on HTTP:<BR>

boolean ECregister(String MAC,String Profile)<BR>
String ECpull(String MAC,String Feature)<BR>
boolean ECpush(String pushData,String MAC,String Feature)<BR>
boolean ECderegister(String MAC)<BR>

each function can tranfer data use IoTTalk's define HTTP format.Like IoTTalk's csmapi.py or csmapi.java.<BR>



Reference:<BR>
[1]The ESP8266 UART Serial communicate with Arduino is Reference from " http://allaboutee.com/2014/12/30/esp8266-and-arduino-webserver/ ".<BR>
[2]IoTTalk(EasyConnect) : 戴嘉駿,EasyConnect物聯網系統,國立交通大學碩士論文,2015<BR>
