#include <Arduino.h>
#include <SoftwareSerial.h>
#include <String.h>
#include "BlockNot.h"
#include <EEPROM.h>
#define ledp 5
#define dltbtn 12
#define rin A0
#define yin A1
#define bin A2
#define rinp A3
#define yinp A4
#define binp A5
#define motorpin LED_BUILTIN

BlockNot motorOffTimer(1, SECONDS);
BlockNot deleteMsg(120, SECONDS);

int IHV = 455,ILV = 360;

//Create software serial object to communicate with SIM800L
SoftwareSerial gsm(3, 2); 

//SIM800L Tx & Rx is connected to Arduino #3 & #2

String data;
String pass = "1443";
String phone1, phone2, phone3, phone4, phone5;
bool numbersFull;
char* phonetmp;
int num1[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int num2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int num3[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int num4[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int num5[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int rVoltage, yVoltage, bVoltage;
int VCALRI, VCALYI, VCALBI;
String rcvNum;
bool motor;
float RY, YB, BR;
bool bypass = true;

char ry1[4];
char yb1[4];
char br1[4];
char hour[3];
char minute[3];


//Constant Messages:
const char* delmsgcmd = "AT+CMGDA=DEL ALL";
const char* numdeletemsg = "Numbers Deleted";
const char* gsmconmsg = "CON2GSM";
const char* atcmd = "AT";
const char* textmodecmd = "AT+CMGF=1";
const char* gettextmodecmd = "AT+CMGF?";
const char* motorontext = "Motor ON";
const char* motorofftext = "Motor Off";
const char* stateprefix = "@STATE";
const char* ridprefix = "#RID";
const char* dataprefix = "&DATA=";
const char* motorstateprefix = "%MOTORSTATE:";
const char* pwrstateprefix = "%PWRSTATE:";
const char* ryprefix = "%RY:";
const char* ybprefix = "%YB:";
const char* brprefix = "%BR:";
const char* phaseprefix = "%PHASE:";
const char* timeprefix = "%TIME:";
const char* alarmprefix = "%ALARM:";
const char* endprefix = "%OK";
const char* donemsg = "Done";
const char* motortimeroffmsg = "Motor Stopped, Timeout.";
const char* runprefix = "RUN";
const char* stopprefix = "STP";
const char* errprefix = "ERR";
const char* errresponse = "%ERR:";
const char* errmsg = "Error Occured";
const char* smsnotifprefix = "+CMTI";
const char* smsreadcmd = "AT+CMGR=";
const char* smsreadresponse = "+CMGR";
const char* linkcmdprefix = "@LK";
const char* connectedresponse = "%CONNECTED";
const char* memfulmsg = "MEMFUL";
const char* getdataprefix = "GETDATA";
const char* setdataprefix = "SETDATA";
const char* smstocmd = "AT+CMGS=\"+91";





void readParameters() {
  VCALRI = 4 * EEPROM.read(0);
  VCALYI = 4 * EEPROM.read(1);
  VCALBI = 4 * EEPROM.read(2);
}

void setIN(uint8_t pin){
  pinMode(pin, INPUT);
}

void setOUT(uint8_t pin){
  pinMode(pin, OUTPUT);
}

void configIO() {
  //setOUT(5);
  setOUT(motorpin);
  setIN(12);
  setIN(rin);
  setIN(yin);
  setIN(bin);
  setIN(rinp);
  setIN(yinp);
  setIN(binp);
  pinMode(dltbtn, INPUT_PULLUP);

}

void deleteAllNums() {
  for(int i = 10; i < 60; i++){
    EEPROM.update(i, 0);
  }
  Serial.println(numdeletemsg);
}

uint8_t getSavedNumbersLoc() {
  if(num1[0] == 0)
  {
    numbersFull = false;
    return 1;
  } else if (num2[0] == 0)
  {
    numbersFull = false;
    return 2;
  }
   else if (num3[0] == 0)
  {
    numbersFull = false;
    return 3;
  }
   else if (num4[0] == 0)
  {
    numbersFull = false;
    return 4;
  }
   else if (num5[0] == 0)
  {
    numbersFull = false;
    return 5;
  } else {
    numbersFull = true;
    return 5;
  }
  
}

void readNumbers() {
  phone1 = "";
  phone2 = "";
  phone3 = "";
  phone4 = "";
  phone5 = "";

  for(int x = 0; x < getSavedNumbersLoc(); x++){
    switch(x) {
      case 0:
        for(int i = 0; i<10; i++){
          num1[i] = EEPROM.read(i + 10);
        }
        for(int i = 0; i<10; i++){
          if(num1[i] > 9){
          num1[i] = 0;
        }
        }
        for(int i = 0; i<10; i++) {
          phone1.concat(String(num1[i]));
        }
        break;
      case 1:
        for(int i = 0; i<10; i++){
          num2[i] = EEPROM.read(i + 20);
        }
        for(int i = 0; i<10; i++){
          if(num2[i] > 9){
          num2[i] = 0;
        }
        }
        for(int i = 0; i<10; i++) {
          phone2.concat(String(num2[i]));
        }
        break;
      case 2:
        for(int i = 0; i<10; i++){
          num3[i] = EEPROM.read(i + 30);
        }
        for(int i = 0; i<10; i++){
          if(num3[i] > 9){
          num3[i] = 0;
        }
        }
        for(int i = 0; i<10; i++) {
          phone3.concat(String(num3[i]));
        }
        break;
      case 3:
        for(int i = 0; i<10; i++){
          num4[i] = EEPROM.read(i + 40);
        }
        for(int i = 0; i<10; i++){
          if(num4[i] > 9){
          num4[i] = 0;
        }
        }
        for(int i = 0; i<10; i++) {
          phone4.concat(String(num4[i]));
        }
        break;
      case 4:
        for(int i = 0; i<10; i++){
          num5[i] = EEPROM.read(i + 50);
        }
        for(int i = 0; i<10; i++){
          if(num5[i] > 9){
          num5[i] = 0;
        }
        }
        for(int i = 0; i<10; i++) {
          phone5.concat(String(num5[i]));
        }
        break;
      default:
        break;

  }
    }
  }

void setup()
{
  motorOffTimer.stop();
  configIO();
  //digitalWrite(lede, HIGH);
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(19200);
  
  //Begin serial communication with Arduino and SIM800L
  gsm.begin(19200);

  Serial.println(gsmconmsg);
  gsm.println(atcmd);
  delay(300);
  gsm.println(textmodecmd);
  delay(300);
  gsm.println(gettextmodecmd);
  gsm.println(delmsgcmd);
  readNumbers();
  //Serial.println("Saved Numbers: \nNum1: " + phone1 + "\nNum2: " + phone2 + "\nNum3: " + phone3 + "\nNum4: " + phone4 + "\nNum5: " + phone5);
  Serial.println("Total Saved: " + (String)getSavedNumbersLoc());
  Serial.println(motor ? motorontext:motorofftext);
}
/*
float checkhz() {
  long ontime,offtime;
  //calculates the pulse width in milliseconds //timesout after 60ms to avoid blocking other code if no input is detected!
  ontime = pulseIn(hz, HIGH);
  offtime = pulseIn(hz, LOW);
  //Calculates the Frequency by dividing the ontime and offtime with 1 sec time (or 1000000 ms)
  return 1000000.0 / (ontime + offtime);//(ontime + offtime);
}
*/
void IVo() {

float rin1, yin1, bin1;
  for(int i=0; i<1; i++) {
    if(((VCALRI/1000.0) * analogRead(rin)) > rin1){
      rin1 = (VCALRI/1000.0) * analogRead(rin);
      }
    rVoltage = (int)rin1;
    if(((VCALYI/1000.0) * analogRead(yin)) > yin1){
      yin1 = (VCALYI/1000.0) * analogRead(yin);
      }
    yVoltage = (int)yin1;
    if(((VCALBI/1000.0) * analogRead(bin)) > bin1){
      bin1 = (VCALBI/1000.0) * analogRead(bin);
      }
    bVoltage = (int)bin1;
    }
  RY = (rVoltage + yVoltage) * 0.866;
  YB = (yVoltage + bVoltage) * 0.866;
  BR = (bVoltage + rVoltage) * 0.866;
  sprintf(ry1, "%03d", (int)RY);
  sprintf(yb1, "%03d", (int)YB);
  sprintf(br1, "%03d", (int)BR);
}

void updateParameters() {
  EEPROM.update(0, VCALRI/4);
  EEPROM.update(1, VCALYI/4);
  EEPROM.update(2, VCALBI/4);
}

bool phaseSeq() {
 int r = analogRead(rinp);
  if (r > 300 && r < 750){
    delay (6.66666666);
    int y = analogRead(yinp);    
    int b = analogRead(binp);    
    if (y > 300)
    {
      return true;
    } else if ( b > 300) 
    {
      return false;
    } else {
      return false;
    }
  }
}

String getPhaseSeq() {
  if(phaseSeq()){
    return "RYB";
  } else {
    return "RBY";
  }
}

bool isNumberAllowed (String msg) {
    if(msg.indexOf(phone1) != -1){
      if(phone1 != ""){
        rcvNum = phone1;
        return true;
      } 
    } else if(msg.indexOf(phone2) != -1){
      if(phone2 != ""){
        rcvNum = phone2;
        return true;
      }
    } else if(msg.indexOf(phone3) != -1){
      if(phone3 != ""){
        rcvNum = phone3;
        return true;
      }
    } else if(msg.indexOf(phone4) != -1){
      if(phone4 != ""){
        rcvNum = phone4;
        return true;
      }
    } else if(msg.indexOf(phone5) != -1){
      if(phone5 != ""){
        rcvNum = phone5;
        return true;
      }
    } else {
      return false;
    }
}

String getRemainingTime() {
  long second;
  second = motorOffTimer.convert(motorOffTimer.getTimeUntilTrigger(), SECONDS);
  int hr, min;
  hr = second / 3600;
  min = (second % 3600) / 60;
  sprintf(hour, "%02d", hr);
  sprintf(minute, "%02d", min);
  return (String)hour + ":" + (String)minute;
}


String getPwrState() {
  IVo();
  if(!bypass){
    if(RY < 30 && YB < 30 && BR < 30)
    {
      return "NPR";
    } else 
    {
      return "PWR"; 
    }
  } else {
    return "PWR";
  }
}

String getAlarm() {
  IVo();
  if(!bypass){
    if(getPwrState() == "NPR"){
      return "NPR";
    } else {
      if((RY < ILV || RY > IHV) && 
      (YB < ILV || YB > IHV) && 
      (BR < ILV || BR > IHV)){
        return "VAL";
      } else {
        if(RY < ILV || RY > IHV)
        {
          return "VRY";
        } else if (YB < ILV || YB > IHV)
        {
          return "VYB";
        } else if(BR < ILV || BR > IHV)
        {
          return "VBR";
        } else if (phaseSeq())
        {
          return "PHQ";
        } else {
          return "AOK";
        }
      }
    }
  } else {
    return "AOK";
  }
}

String getMotorState() {
  if(getAlarm() == "AOK"){
    if(motor){
      return "RUN";
    } else {
      return "RDY";
    }
  } else {
    return "ALM";
  }
}

String requestID(String request) {
  if(request.indexOf(ridprefix) != -1){
    String rid = request.substring(request.indexOf(ridprefix) + 4, request.indexOf(ridprefix) + 8);
    return rid;
  } else {
    return "NoRID";
  }
}

void sendSMS(String phone, String msg1){
  /*
  String sendmsg;
  sendmsg = "AT+CMGS=\"+91";
  sendmsg += phone;
  sendmsg += "\"";
  */
  Serial.println(smstocmd + rcvNum + "\"");
  delay(300);
  gsm.println(smstocmd + rcvNum + "\"");
  delay(300);
  gsm.print(msg1);
  delay(300);
  gsm.write(26);
  delay(300);
  gsm.println();
  delay(300);
  Serial.print(msg1);
  delay(300);
  Serial.write(26);
  delay(300);
  Serial.println();
  delay(300);
  Serial.println(msg1);
  data = "";
}

void getData(String tempdata, String rid) {
  /*
  String TMSG, TRID, TMTRSTATE, TPWRSTATE, TPHQ, TTIME, TALM;
  TRID = requestID(tempdata); Serial.println("RequestID: " + TRID);
  TMTRSTATE = getMotorState(); Serial.println("MotorState: " + TMTRSTATE);
  TPWRSTATE = getPwrState(); Serial.println("PowerState: " + TPWRSTATE);
  TPHQ = getPhaseSeq(); Serial.println("PhaseSeq: " + TPHQ);
  TTIME = getRemainingTime(); Serial.println("TIME: " + TTIME);
  TALM = getAlarm(); Serial.println("AlarmState: " + TALM);
  TMSG += "@STATE#RID" + TRID;
  TMSG += "&DATA=";
  TMSG +="%MOTORSTATE:" + TMTRSTATE;
  TMSG +="%PWRSTATE:" + TPWRSTATE;
  TMSG +="%RY:" + (String)((int)RY);
  TMSG +="%YB:" + (String)((int)YB);
  TMSG +="%BR:" + (String)((int)BR);
  TMSG +="%PHASE:" + TPHQ;
  TMSG +="%TIME:" + TTIME;
  TMSG +="%ALARM:" + TALM;
  TMSG +="%END";
  */
  /*
  String sendmsg;
  sendmsg = "AT+CMGS=\"+91";
  sendmsg += rcvNum;
  sendmsg += "\"";
  */
  gsm.println(smstocmd + rcvNum + "\"");
  delay(300);
  gsm.print(stateprefix + (String)ridprefix + rid
  + dataprefix
  + motorstateprefix + getMotorState()
  + pwrstateprefix + getPwrState()
  + ryprefix + (String)ry1
  + ybprefix + (String)yb1
  + brprefix + (String)br1
  + phaseprefix + getPhaseSeq()
  + timeprefix + getRemainingTime()
  + alarmprefix + getAlarm()
  + endprefix);
  delay(300);
  gsm.write(26);
  delay(300);
  gsm.println();
  delay(300);
  Serial.println(donemsg);
  data = "";
  //sendSMS(rcvNum);

}

void startTime(String time) {
  int hr, min;
  long second;
  if(time != "") {
    // TIME:  HH:MM
    hr = time.substring(1, 2).toInt();
    min = time.substring(4, 5).toInt();
    second = (hr * 3600) + (min * 60);
    motorOffTimer.setDuration(second, SECONDS);
    motorOffTimer.start();
    motorOffTimer.reset();
    motor = true;
  }
}

void timeUp() {
  motor = false;
  motorOffTimer.stop();
  sendSMS(rcvNum, motortimeroffmsg);
}

void setData(String tempdata, String rid) {
  String temptime, tempmotor;
  if(tempdata.indexOf(motorstateprefix) != -1){
    tempmotor = tempdata.substring(tempdata.indexOf(motorstateprefix) + 12, tempdata.indexOf(motorstateprefix) + 15);
    if(tempmotor == runprefix){
      if(tempdata.indexOf(timeprefix) != -1){
        temptime = tempdata.substring(tempdata.indexOf(timeprefix) + 6, tempdata.indexOf(timeprefix) + 11);
        startTime(temptime);
      } else {
        motor = true;
      }
    }
    else if (tempmotor == stopprefix)
    {
      motor = false;
    } else 
    {
      Serial.println("ErMTRCMD");
    }
    /*
    String sendmsg;
    sendmsg = "AT+CMGS=\"+91";
    sendmsg += rcvNum;
    sendmsg += "\"";
    */
    delay(300);
    gsm.println(smstocmd + rcvNum + "\"");
    delay(300);
    gsm.print(stateprefix + (String)ridprefix + rid
    + dataprefix + "REOK"
    + motorstateprefix + getMotorState()
    + pwrstateprefix + getPwrState()
    + ryprefix + ry1
    + ybprefix + yb1
    + brprefix + br1
    + phaseprefix + getPhaseSeq()
    + timeprefix + getRemainingTime()
    + alarmprefix + getAlarm()
    + endprefix);
    delay(300);
    gsm.write(26);
    delay(300);
    gsm.println();
    delay(300);
    Serial.println("\nOK");
    data = "";
  }
}

void saveNumbers() {
  for(int x = 0; x < getSavedNumbersLoc(); x++){
    switch(x) {
      case 0:
        for(int i = 0; i<10; i++) {
          EEPROM.update(i + 10, num1[i]);
        }
        break;
      case 1:
        for(int i = 0; i<10; i++) {
          EEPROM.update(i + 20, num2[i]);
        }
        break;
      case 2:
        for(int i = 0; i<10; i++) {
          EEPROM.update(i + 30, num3[i]);
        }
        break;
      case 3:
        for(int i = 0; i<10; i++) {
          EEPROM.update(i + 40, num4[i]);
        }
        break;
      case 4:
        for(int i = 0; i<10; i++) {
          EEPROM.update(i + 50, num5[i]);
        }
        break;
      default :
        break;
    }
  }
  readNumbers();
  //Serial.println("Saved Numbers: \nNum1: " + phone1 + "\nNum2: " + phone2 + "\nNum3: " + phone3 + "\nNum4: " + phone4 + "\nNum5: " + phone5);
}

void loop()
{

  if(deleteMsg.triggered(true)){
    gsm.println(delmsgcmd);
  }

  if(!digitalRead(dltbtn)){
    deleteAllNums();
  }
  if(motorOffTimer.triggered(false)){
    timeUp();
  }

  if(motor){
    digitalWrite(motorpin, HIGH);
  } else {
    digitalWrite(motorpin, LOW);
  }

  /*
  while(Serial.available() > 0){
    gsm.write(Serial.read());
  }
  */
  //updateSerial();
  if(gsm.available()) {
    data = gsm.readString();
    Serial.println(data);
  }
  if(data.indexOf(errprefix) != -1){
    Serial.println(errmsg);
    //digitalWrite(lede, HIGH);
    data = "";
  }
  if(data.indexOf(smsnotifprefix) != -1) {
    String msgindex;
    msgindex = data.substring(data.indexOf(",")+1, data.indexOf(",")+2);
    data = "";
    delay(300);
    gsm.println(smsreadcmd + msgindex);
  }
  if(data.indexOf("OK") != -1){
      Serial.println("GSM OK");
      //digitalWrite(lede, LOW);
      if(data.length() > 5){
        if(data.indexOf(smsreadresponse) != -1) {
            Serial.println("Cmd " + data);
          if(data.indexOf(linkcmdprefix) != -1)
            {
              String num, password;
              password = data.substring((data.indexOf(linkcmdprefix) + 3), (data.indexOf(linkcmdprefix) + 7));
            //numloc = data.substring((data.indexOf("#")) + 1, (data.indexOf("#")) + 2);
              num = data.substring((data.indexOf("&") + 1), (data.indexOf("&") + 11));
              rcvNum = num;
              if(password == pass) {
            //delay(500);
              const char* tempnum = num.c_str();
            if(!numbersFull){
              if(getSavedNumbersLoc() == 1)
              {
                for(int i = 0; i < 10; i++){
                  num1[i]= tempnum[i] - '0';
                }
              }
              else if (getSavedNumbersLoc() == 2)
              {
                for(int i = 0; i < 10; i++){
                  num2[i]= tempnum[i] - '0';
                }
              }
              else if (getSavedNumbersLoc() == 3)
               {
                for(int i = 0; i < 10; i++){
                  num3[i]= tempnum[i] - '0';
                }
              }
              else if (getSavedNumbersLoc() == 4)
               {
                for(int i = 0; i < 10; i++){
                  num4[i]= tempnum[i] - '0';
                }
              } 
              else if (getSavedNumbersLoc() == 5)
              {
                for(int i = 0; i < 10; i++){
                  num5[i]= tempnum[i] - '0';
                }
              }
              /*
              String sendmsg;
              sendmsg = "AT+CMGS=\"+91";
              sendmsg = sendmsg + rcvNum;
              sendmsg = sendmsg + "\"";
              */
              Serial.println(smstocmd + rcvNum + "\"");
              gsm.println(smstocmd + rcvNum + "\"");
              delay(300);
              gsm.print(ridprefix + requestID(data) + connectedresponse);
              delay(300);
              gsm.write(26);
              data = "";
            } else {
              /*
              String sendmsg;
              sendmsg = "AT+CMGS=\"+91";
              sendmsg = sendmsg + rcvNum;
              sendmsg = sendmsg + "\"";
              */
              Serial.println("memful");
              Serial.println(smstocmd + rcvNum + "\"");
              gsm.println(smstocmd + rcvNum + "\"");
              //delay(200);
              gsm.print(ridprefix + requestID(data) + errresponse + memfulmsg);
              //delay(200);
              gsm.write(26);
              //delay(200);
            
            }
              saveNumbers();
              data = "";
            } else {
              Serial.println("ErrPaswrd " + password);
              data = "";
            }
          } 
          else if(isNumberAllowed(data))
          {

            Serial.println("AGRNT");
            if(data.indexOf(stateprefix) != 1)
            {
              if(data.indexOf(getdataprefix) != -1)
              {
                getData(data, requestID(data));
              } 
              else if(data.indexOf(setdataprefix) != -1)
              {
                setData(data, requestID(data));
              }
            }
          } else
          {
            Serial.println("UCmd");
            Serial.println(data);
            data = "";
          }
            data = "";
          } 
          data = "";
        } else {
        data = "";
        }
      data = "";
      //clearGSM();
  }
  data = "";
  
}
