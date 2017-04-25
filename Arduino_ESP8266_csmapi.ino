//by NTCU-CS-PRJ-106-04
#include <SoftwareSerial.h>

//SoftwareSerial esp8266(50,51);
//esp8266 on Serial1

#define SerialBaud 115200



String MAC = "TestMAC";
String ECipaddress = "";

#define DEBUG true

void setup() {
  //sendData("AT+CWJAP=\"SSID\",\"PreShareKey\"\r\n",10000,DEBUG);  //esp8266 connect wifi ap
  Serial.begin(SerialBaud);
  Serial1.begin(SerialBaud);
  Serial.print("setup");

  delay(2000);
  sendData("AT+CWMODE=1\r\n",50,DEBUG);
  
  MAC=FindMac();
  connectEC();//find local iottalk server's broadcast package, if do not use this function,you should assign iottalk ip address on ECipaddress String.
  //the iottalk have already define device model named "Arduino" , and have many input device feature like MQ-2,Fire,Light,Temperature,Volume.
  String Profile = "{\"profile\":{\"d_name\":\"Arduino_ESP12\" ,\"dm_name\":\"Arduino\",\"df_list\":[\"MQ-2\",\"MQ-5\",\"Fire\",\"Light\",\"Temperature\",\"Volume\"],\"u_name\":\"yb\",\"is_sim\":false}}";
  ECregister(MAC,Profile);
  ECpush("20,30,40",MAC,"Acceleration");//you should read data from real sensor,this just a example that the value form Acceleration sensor is 20,30,40 
  ECpull(MAC,"Acceleration");
  ECderegister(MAC);
  
}

void loop() {
  //you can read sensor value and reuse ECpush() to resend sensor value to iottalk.
}


String readDataFromSerial(int timeout){
    long int time = millis();
    String response ="";  
    while( (time+timeout) > millis())
    {
      while(Serial.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = Serial.read(); // read the next character.
        response+=c;
      }  
    }
    return response;
}

String sendData(String command, const int timeout, boolean debug)
{
    String response = "Send:"+command;
    //Serial1.end();
    //Serial1.begin(SerialBaud);
    
    Serial1.print(command); // send the read character to the esp8266
    //delay(5000);
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(Serial1.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = Serial1.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}

String readDataFromESP8266Serial(int timeout){
    long int time = millis();
    String response ="";
    
    while( (time+timeout) > millis())
    {
      while(Serial1.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = Serial1.read(); // read the next character.
        response+=c;
      }  
    }
    return response;
}


String FindMac(){
    String msgs=sendData("AT+CIFSR\r\n",100,DEBUG);
    int macIndex=stringFind(msgs,"STAMAC,");
    String MACaddr=msgs.substring(macIndex+8,macIndex+25);
    int i;
    String macParse="";
    for(i=0;i<MACaddr.length();i++){
      if(MACaddr.charAt(i)==':'){
      }
      else{
        macParse=macParse+MACaddr.charAt(i);
      }
    }
    return macParse;
}

boolean connectEC(){
  boolean conn = false;
  sendData("AT+CIPCLOSE\r\n",100,DEBUG);
  sendData("AT+CIPSTART=\"UDP\",\"0\",0,17000,2\r\n",100,DEBUG);
  
  while(!conn){
    if(Serial1.available()){
      String ecS=readDataFromESP8266Serial(100);
      if(stringFind(ecS,"easyconnect")){
        String CIPSTATUS = sendData("AT+CIPSTATUS\r\n",200,DEBUG);
        String fi = "+CIPSTATUS:0,\"UDP\"";
        String tempec="";
        tempec=CIPSTATUS.substring(stringFind(CIPSTATUS,fi)+fi.length()+2,stringFind(CIPSTATUS,fi)+fi.length()+15+2);
        int i;
        for(i=0;i<tempec.length();i++){
          if(tempec.charAt(i)=='\"'){
            break;  
          }
          else{
            ECipaddress+=tempec.charAt(i);
          }
        }
        
        Serial.print("Find EC IP : ");
        Serial.println(ECipaddress);
        //Serial.println(stringFind(CIPSTATUS,fi)+fi.length());
        //Serial.println(stringFind(CIPSTATUS,fi)+fi.length()+15);
        if(ECipaddress.length()>=2){
         conn=true;
         sendData("AT+CIPCLOSE\r\n",100,DEBUG);
        }
      }
    }
  }
}
boolean ECregister(String MAC,String Profile){
   sendData("AT+CIPSTART=\"TCP\",\""+ECipaddress+"\",9999\r\n",100,DEBUG);
   //ex: String Profile = "{\"profile\":{\"d_name\":\"MyArduino\" ,\"dm_name\":\"Arduino\",\"df_list\":[\"Temperature\"],\"u_name\":\"yb\",\"is_sim\":false}}";
   String httpHeader;
   String httpPOST;
   // HTTP Header
   httpHeader = "POST /"+MAC+" HTTP/1.1\r\nContent-Type: application/json\r\n"; 
   httpHeader += "Content-Length: ";
   httpHeader += Profile.length();
   httpHeader += "\r\n";
   httpHeader +="Connection: keep-alive\r\n\r\n";
   httpPOST = httpHeader + Profile + " ";

   String cipSend = "AT+CIPSEND=";
   cipSend += httpPOST.length();
   cipSend +="\r\n";
   sendData(cipSend,100,DEBUG);
   sendData( httpPOST,500,DEBUG);        
}


boolean ECderegister(String MAC){
   sendData("AT+CIPSTART=\"TCP\",\""+ECipaddress+"\",9999\r\n",100,DEBUG);
   String jsonData = "";
   String httpHeader;
   String httpPOST;
   // HTTP Header
   httpHeader = "DELETE /"+MAC+" HTTP/1.1\r\nContent-Type: application/x-www-form-urlencoded\r\n"; 
   httpHeader += "Content-Length: ";
   httpHeader += jsonData.length();
   httpHeader += "\r\n";
   httpHeader +="Connection: keep-alive\r\n\r\n";
   httpPOST = httpHeader + jsonData + " ";

   String cipSend = "AT+CIPSEND=";
   cipSend += httpPOST.length();
   cipSend +="\r\n";
   sendData(cipSend,100,DEBUG);
   sendData( httpPOST,500,DEBUG);        
}

String ECpull(String MAC,String Feature){
   sendData("AT+CIPSTART=\"TCP\",\""+ECipaddress+"\",9999\r\n",100,DEBUG);
   String jsonData = "";
   String httpHeader;
   String httpPOST;
   // HTTP Header
   httpHeader = "GET /"+MAC+"/"+Feature+" HTTP/1.1\r\nContent-Type: application/x-www-form-urlencoded\r\n"; 
   httpHeader += "Content-Length: ";
   httpHeader += jsonData.length();
   httpHeader += "\r\n";
   httpHeader +="Connection: keep-alive\r\n\r\n";
   httpPOST = httpHeader + jsonData + " ";

   String cipSend = "AT+CIPSEND=";
   cipSend += httpPOST.length();
   cipSend +="\r\n";
   sendData(cipSend,50,DEBUG);
   String response = sendData( httpPOST,500,DEBUG);

   return response;
}

boolean ECpush(String pushData,String MAC,String Feature){
   sendData("AT+CIPSTART=\"TCP\",\""+ECipaddress+"\",9999\r\n",100,DEBUG);
   String jsonData = "{\"data\":["+pushData+"]}";
   String httpHeader;
   String httpPOST;
   // HTTP Header
   httpHeader = "PUT /"+MAC+"/"+Feature+" HTTP/1.1\r\nContent-Type: application/json\r\n"; 
   httpHeader += "Content-Length: ";
   httpHeader += jsonData.length();
   httpHeader += "\r\n";
   httpHeader +="Connection: keep-alive\r\n\r\n";
   httpPOST = httpHeader + jsonData + " ";

   String cipSend = "AT+CIPSEND=";
   cipSend += httpPOST.length();
   cipSend +="\r\n";
   sendData(cipSend,30,DEBUG);
   sendData(httpPOST,50,DEBUG);     

   return true;
}


static int stringFind(String Fi,String Se){
  int i,j,have=0;
  //Serial.println("call find data");
  //Serial.println(Fi);
  //Serial.println(Se);
  //Serial.println("call find data end");
  
  for(i=0;i<Fi.length()-Se.length()+1;i++){
    have=0;
    for(j=0;j<Se.length();j++){
      //Serial.println(Fi.charAt(i+j));
      //Serial.println(Se.charAt(j));
      if(Fi.charAt(i+j) == Se.charAt(j)){
        have++;
        
      }
    }
    if(have==Se.length()){
      return i;
    }
  }
  return -1;
}
