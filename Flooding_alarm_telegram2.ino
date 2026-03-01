#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP8266WebServer.h>
#include <time.h>

/* ================= PIN ================= */
#define TRIG_PIN   D1
#define ECHO_PIN   D2
#define BUZZER_PIN D5

#define BUZZER_ON  LOW
#define BUZZER_OFF HIGH

/* ================= WIFI ================= */
const char* ssid = "Surin_2.4G";
const char* pass = "0862674344";

/* ================= TELEGRAM ================= */
#define BOT_TOKEN "8479743852:AAHA_2kuN3qo5agSAWlLyXiRuqj5FqD8BKs"
#define CHAT_ID   "7808527597"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

ESP8266WebServer server(80);

/* ================= SYSTEM ================= */
#define SENSOR_DISTANCE 60
#define MAX_HISTORY 10

int waterHeight = 0;
uint8_t level = 0;
uint8_t lastLevel = 255;

String historyLog[MAX_HISTORY];
uint8_t historyCount = 0;

unsigned long tSensor = 0;
const long gmtOffset_sec = 7 * 3600;

/* ================= FUNCTION ================= */

void addHistory(String msg){
  if(historyCount < MAX_HISTORY){
    historyLog[historyCount++] = msg;
  } else {
    for(int i=1;i<MAX_HISTORY;i++)
      historyLog[i-1]=historyLog[i];
    historyLog[MAX_HISTORY-1]=msg;
  }
}

String nowString(){
  time_t now = time(nullptr);
  if(now < 100000) return "--";
  struct tm *t = localtime(&now);
  char buf[25];
  sprintf(buf,"%02d/%02d %02d:%02d:%02d",
          t->tm_mday,t->tm_mon+1,
          t->tm_hour,t->tm_min,t->tm_sec);
  return String(buf);
}

long readDistance(){
  digitalWrite(TRIG_PIN,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN,LOW);
  long duration=pulseIn(ECHO_PIN,HIGH,12000);
  return duration/29/2;
}

uint8_t calcLevel(int h){
  if(h < 20) return 0;
  if(h < 30) return 1;
  if(h < 40) return 2;
  return 3;
}

/* ================= WEB PAGE ================= */
const char PAGE[] PROGMEM = R"====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta charset="utf-8">
<title>ระบบตรวจจับระดับน้ำ</title>
<style>
body{
  margin:0;
  font-family: 'Segoe UI', sans-serif;
  background: linear-gradient(135deg,#0f172a,#1e293b);
  color:white;
  text-align:center;
}
.card{
  margin:40px auto;
  padding:30px;
  max-width:420px;
  background: rgba(255,255,255,0.05);
  border-radius:20px;
  backdrop-filter: blur(10px);
  box-shadow:0 10px 30px rgba(0,0,0,0.5);
}
.value{
  font-size:80px;
  font-weight:bold;
  margin:20px 0;
}
.status{
  font-size:28px;
  margin-bottom:10px;
}
.time{
  font-size:16px;
  opacity:0.7;
}
.history{
  margin-top:20px;
  text-align:left;
  font-size:14px;
  background:rgba(0,0,0,0.3);
  padding:15px;
  border-radius:12px;
  max-height:150px;
  overflow:auto;
}
.normal{color:#22c55e;}
.watch{color:#facc15;}
.high{color:#fb923c;}
.danger{color:#ef4444;}
</style>
</head>
<body>

<div class="card">
  <h2>🌊 ระบบตรวจจับระดับน้ำ</h2>
  <div id="water" class="value">-- cm</div>
  <div id="status" class="status"></div>
  <div id="time" class="time"></div>
  <div class="history">
    <b>ประวัติการแจ้งเตือน</b>
    <div id="history"></div>
  </div>
</div>

<script>
function update(){
 fetch('/data').then(r=>r.json()).then(d=>{
   let water=document.getElementById('water');
   let status=document.getElementById('status');
   let time=document.getElementById('time');

   water.className="value";
   status.className="status";

   water.innerHTML=d.height+" cm";
   time.innerHTML="เวลา: "+d.time;

   if(d.height < 20){
     status.innerHTML="สถานะ: ปกติ";
     water.classList.add("normal");
     status.classList.add("normal");
   }
   else if(d.height < 30){
     status.innerHTML="สถานะ: เฝ้าระวัง";
     water.classList.add("watch");
     status.classList.add("watch");
   }
   else if(d.height < 40){
     status.innerHTML="สถานะ: ระดับสูง";
     water.classList.add("high");
     status.classList.add("high");
   }
   else{
     status.innerHTML="สถานะ: อันตราย";
     water.classList.add("danger");
     status.classList.add("danger");
   }

   let h="";
   d.history.slice().reverse().forEach(i=>{
     h+="<div>"+i+"</div>";
   });
   document.getElementById("history").innerHTML=h;
 });
}
setInterval(update,1000);
update();
</script>

</body>
</html>
)====";

/* ================= API ================= */
void handleData(){
  String json="{";
  json+="\"height\":"+String(waterHeight)+",";
  json+="\"time\":\""+nowString()+"\",";
  json+="\"history\":[";
  for(int i=0;i<historyCount;i++){
    json+="\""+historyLog[i]+"\"";
    if(i<historyCount-1) json+=",";
  }
  json+="]}";
  server.send(200,"application/json",json);
}

/* ================= SETUP ================= */
void setup(){
  Serial.begin(115200);

  pinMode(TRIG_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  pinMode(BUZZER_PIN,OUTPUT);
  digitalWrite(BUZZER_PIN,BUZZER_OFF);

  WiFi.begin(ssid,pass);
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
  }

  client.setInsecure();
  configTime(gmtOffset_sec,0,"pool.ntp.org");

  server.on("/", [](){ server.send_P(200,"text/html",PAGE); });
  server.on("/data", handleData);
  server.begin();

  bot.sendMessage(CHAT_ID,"ระบบออนไลน์\nIP: "+WiFi.localIP().toString(),"");
}

/* ================= LOOP ================= */
void loop(){

  server.handleClient();

  if(millis()-tSensor>=300){
    tSensor=millis();

    long distance=readDistance();
    int h=SENSOR_DISTANCE-distance;
    if(h<0) h=0;

    waterHeight=h;
    level=calcLevel(waterHeight);

    // 🔔 Buzzer ดังเมื่อ ≥ 40 cm
    if(waterHeight >= 40)
      digitalWrite(BUZZER_PIN,BUZZER_ON);
    else
      digitalWrite(BUZZER_PIN,BUZZER_OFF);

    if(level!=lastLevel){
      String msg="ระดับน้ำ: "+String(waterHeight)+" cm";
      bot.sendMessage(CHAT_ID,msg,"");
      addHistory(nowString()+" | "+String(waterHeight)+" cm");
      lastLevel=level;
    }
  }
}
