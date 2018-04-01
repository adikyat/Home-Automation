#include <NTPtimeESP.h>

#define DEBUG_ON

#define BLYNK_PRINT Serial

#include <LiquidCrystal_I2C.h>



#include <ArduinoJson.h>

#include <ESP8266WebServer.h>

#include <WiFiClientSecure.h>
#include <WiFiClient.h>


#include <Wire.h>                             
                          

LiquidCrystal_I2C lcd(0x3F, 20, 4);                       


String APIKEY = "2903270bf4fa84e6b41b19cccb1a19eb";
String CityID = "1254661";

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "c2f46eb3ba5448e78d07ec372ae307d3";
String IftttMakerKey = "ksbK2QwkEb2OAQEUQPrjZWuDM88FP-1RkLRxwZnNhQ6"; // IFTTT account
const char *hostIftttMaker = "maker.ifttt.com";
String IftttMakerString = "";
// Your WiFi credentials.
// Set password to "" for open networks.
NTPtime NTPin("in.pool.ntp.org");   // Choose server pool as required
char ssid[] = "AdityaKyatham";
char pass[] = "Kyatham@2013";
const int LED_PIN= D8;
int LDRInPin = D7;
 

 
WiFiClient client;
char servername[]="api.openweathermap.org";  // remote server we will connect to
String result;

int  counter = 60;

String weatherDescription ="";
String weatherLocation = "";
String Country;
float Temperature;
float Humidity;
float Pressure;
strDateTime dateTime;

void setup()
{
  // Debug console
  pinMode(LED_PIN, OUTPUT);

pinMode(LDRInPin, INPUT);
pinMode(D0, OUTPUT);
  Serial.begin(115200);
   delay(10);
   lcd.init();                                           // Initialize the lcd 
  lcd.backlight();                                          // Clear LCD display  
  lcd.setCursor(0,0);                                   // Set cursor position
  lcd.print("    Pce Project");  
  lcd.setCursor(0,1);                                   // Set cursor position
  lcd.print("    RAIT");    
  delay(1500);  
 
  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid,pass);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
   Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
   Blynk.begin(auth, ssid, pass);
  lcd.clear();                                          // Clear LCD    
  lcd.setCursor(0,0);  
  lcd.print(WiFi.localIP());// Set cursor position
  lcd.setCursor(0,1);
  lcd.print("   Connected!");
  
  delay(3000);
  
}

void loop()
{
   Blynk.run();
 
 
 
  

if(digitalRead(LDRInPin)==LOW){
  SendIftttMaker("Someone",IftttMakerKey,"");
  Serial.println("Someone has come");
}

  dateTime = NTPin.getNTPtime(4.5, 1);
  if(dateTime.valid){
    NTPin.printDateTime(dateTime);

    byte actualHour = dateTime.hour;
    byte actualMinute = dateTime.minute;
    byte actualsecond = dateTime.second;
    int actualyear = dateTime.year;
    byte actualMonth = dateTime.month;
    byte actualday =dateTime.day;
    byte actualdayofWeek = dateTime.dayofWeek;
    if(actualHour==15 && actualMinute==00){
      digitalWrite(D0,HIGH);
      delay(5000);
      Serial.print("alarm ");
    }
    else {
      digitalWrite(D0,LOW);
    }
    if(actualHour==15 && actualMinute==1){
      digitalWrite(D0,HIGH);
      delay(5000);
      Serial.print("alarm ");
    }
    else {
      digitalWrite(D0,LOW);
    }
    if(actualHour < 12){
    displayTime( actualHour,actualMinute,actualsecond);
    Serial.println( actualHour);
    
    displayDate(actualday,actualMonth,actualyear);
    
    
    delay(5000);
    }
    else if(actualHour >= 12)
    {
      displayTime1(actualHour,actualMinute,actualsecond);
    Serial.println( actualHour);
    displayDate(actualday,actualMonth,actualyear);
    delay(5000);
    
    }
  }
  if(counter == 60 ) //Get new data every 10 minutes
    {
      counter = 0;
      displayGettingData();
      delay(1000);
      getWeatherData();
    }else 
    {
      counter++;

      displayWeather(weatherLocation,weatherDescription);
      delay(5000);
      displayConditions(Temperature,Humidity,Pressure);
      delay(5000);
    }
    
   
  
  if (digitalRead(LED_PIN) == HIGH) {
SendIftttMaker("Lights",IftttMakerKey,"on");
  
  
}
else{
  digitalWrite(LED_PIN,LOW);
}

}


void send2web(String thehost, String urlstring) {
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(thehost.c_str(), httpPort)) {
    Serial.println(F("connection failed"));
    return;
  }

  // We now create a URI for the request
  //String url = urlstring;

  // This will send the request to the server
  client.print(String("GET ") + urlstring + " HTTP/1.1\r\n" +
               "Host: " + thehost + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(4000);
  client.flush();
  client.stop();
  delay(100);
}

void SendIftttMaker(String inIftttEvent, String inIftttMakerKey, String instatus) {
  IftttMakerString = "/trigger/" + inIftttEvent + "/with/key/" + inIftttMakerKey + "?value1=" + instatus;
  send2web(hostIftttMaker, IftttMakerString);
}
void getWeatherData() //client function to send/receive GET request data.
{
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println("GET /data/2.5/weather?id="+CityID+"&units=metric&APPID="+APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }

  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
      result = result+c;
    }

  client.stop(); //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
  

char jsonArray [result.length()+1];
result.toCharArray(jsonArray,sizeof(jsonArray));
jsonArray[result.length() + 1] = '\0';

StaticJsonBuffer<1024> json_buf;
JsonObject &root = json_buf.parseObject(jsonArray);
if (!root.success())
{
  Serial.println("parseObject() failed");
}

String location = root["name"];
String country = root["sys"]["country"];
float temperature = root["main"]["temp"];
float humidity = root["main"]["humidity"];
String weather = root["weather"]["main"];
String description = root["weather"]["description"];
float pressure = root["main"]["pressure"];

weatherDescription = description;
weatherLocation = location;
Country = country;
Temperature = temperature;
Humidity = humidity;
Pressure = pressure;

}

void displayWeather(String location,String description)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(location);
  lcd.print(", ");
  lcd.print(Country);
  lcd.setCursor(0,1);
  lcd.print(description);
}

void displayConditions(float Temperature,float Humidity, float Pressure)
{
  lcd.clear();
  lcd.print("T:"); 
 lcd.print(Temperature,1);
 lcd.print((char)223);
 lcd.print("C ");
 
 //Printing Humidity
 lcd.print(" H:");
 lcd.print(Humidity,0);
 lcd.print(" %");
 
 //Printing Pressure
 lcd.setCursor(0,1);
 lcd.print("P: ");
 lcd.print(Pressure,1);
 lcd.print(" hPa");

}

void displayGettingData()
{
  lcd.clear();
  lcd.print("Getting data");
}
void displayTime(int actualHour,int actualMinute,int actualsecond)
{
  lcd.clear();
  lcd.setCursor(0,0);
  int a;
  
  if(actualHour==00)
  {
    a=12;
    lcd.print(a);
    lcd.print(":");
  lcd.print(actualMinute);
  lcd.print(":");
  lcd.print(actualsecond);
    lcd.print("  AM");
  }
  else{
   
  lcd.print(actualHour);
  lcd.print(":");
  lcd.print(actualMinute);
  lcd.print(":");
  lcd.print(actualsecond);
  lcd.print("  AM");
  }
  
  
  
}
void displayDate(int actualday,int actualMonth,int actualyear)
{
  lcd.setCursor(0,1);
  lcd.print(actualday);
  lcd.print("/");
  lcd.print(actualMonth);
  lcd.print("/");
  lcd.print(actualyear);
 
}
void  displayTime1( int actualHour,int actualMinute,int actualsecond)
{
  int actualHour1;
   lcd.clear();
  lcd.setCursor(0,0);
  if(actualHour==12)
  {
    actualHour1=12;
    lcd.print(actualHour1);
  lcd.print(":");
  lcd.print(actualMinute);
  lcd.print(":");
  lcd.print(actualsecond);
  lcd.print("  PM");
    
  }
  if(actualHour==13)
  {
    actualHour1=1;
    lcd.print(actualHour1);
  lcd.print(":");
  lcd.print(actualMinute);
  lcd.print(":");
  lcd.print(actualsecond);
  lcd.print("  PM");
    
  }
  if(actualHour==14)
  {
    actualHour1=2;
    lcd.print(actualHour1);
  lcd.print(":");
  lcd.print(actualMinute);
  lcd.print(":");
  lcd.print(actualsecond);
  lcd.print("  PM");
  }
  if(actualHour==15)
  {
    actualHour1=3;
    lcd.print(actualHour1);
  lcd.print(":");
  lcd.print(actualMinute);
  lcd.print(":");
  lcd.print(actualsecond);
  lcd.print("  PM");
  }
  if(actualHour==16)
  {
    actualHour1=4;
    lcd.print(actualHour1);
  lcd.print(":");
  lcd.print(actualMinute);
  lcd.print(":");
  lcd.print(actualsecond);
  lcd.print("  PM");
  }
  if(actualHour==17)
  {
    actualHour1=5;
    lcd.print(actualHour1);
  lcd.print(":");
  lcd.print(actualMinute);
  lcd.print(":");
  lcd.print(actualsecond);
  lcd.print("  PM");
  }
  if(actualHour==18)
  {
    actualHour1=6;
    lcd.print(actualHour1);
  lcd.print(":");
  lcd.print(actualMinute);
  lcd.print(":");
  lcd.print(actualsecond);
  lcd.print("  PM");
  }
  if(actualHour==19)
  {
    actualHour1=7;
    lcd.print(actualHour1);
  lcd.print(":");
  lcd.print(actualMinute);
  lcd.print(":");
  lcd.print(actualsecond);
  lcd.print("  PM");
  }if(actualHour==20)
  {
    actualHour1=8;
    lcd.print(actualHour1);
  lcd.print(":");
  lcd.print(actualMinute);
  lcd.print(":");
  lcd.print(actualsecond);
  lcd.print("  PM");
  }if(actualHour==21)
  {
    actualHour1=9;
    lcd.print(actualHour1);
  lcd.print(":");
  lcd.print(actualMinute);
  lcd.print(":");
  lcd.print(actualsecond);
  lcd.print("  PM");
  }if(actualHour==22)
  {
    actualHour1=10;
    lcd.print(actualHour1);
  lcd.print(":");
  lcd.print(actualMinute);
  lcd.print(":");
  lcd.print(actualsecond);
  lcd.print("  PM");
  }if(actualHour==23)
  {
    actualHour1=11;
    lcd.print(actualHour1);
  lcd.print(":");
  lcd.print(actualMinute);
  lcd.print(":");
  lcd.print(actualsecond);
  lcd.print("  PM");
  }
  
  }


