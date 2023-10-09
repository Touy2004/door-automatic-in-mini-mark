#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

//Setup Servo
Servo myServo;

//Declared Valuable for Servo and Ultasonic pin
int servoPin = 5; // Servo Motor's Data pin
const int TRIG_PIN = 23; // Ultrasonic Sensor's TRIG pin
const int ECHO_PIN = 22; // Ultrasonic Sensor's ECHO pin

//Declared Valuable
float duration_us, distance_cm;
int functionMode = 1;

//Declared Valuable for Wifi and MQTT connection
const char *ssid = "Touynoy";
const char *password = "SK799514";
const char *mqtt_server = "broker.hivemq.com";
const char *mqtt_topic = "soundtouy";

//Setup Wifi and MQTT connection
WiFiClient espClient;
PubSubClient client(espClient);

//function
float ultasonic()
{
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration_us = pulseIn(ECHO_PIN, HIGH);
  distance_cm = 0.017 * duration_us;
  return distance_cm;
}

void setup_wifi()
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  if ((char)payload[0] == '0')
  {
    functionMode = 1;
    Serial.println("on");
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);                     //Initial Serial with bus 9600
	myServo.attach(servoPin, 1000, 2000);   //Initial Servo
	myServo.write(0);                       //Set Degree of Servo to 0
  pinMode(TRIG_PIN, OUTPUT);              //Make Trig pin to be output signal
  pinMode(ECHO_PIN, INPUT);               //Make ECHO pin to be input signal
  setup_wifi();                           //Setup Wifi Connection
  client.setServer(mqtt_server, 1883);    //Setup MQTT Broker and Port 
  client.setCallback(callback);           //Setup Callback function
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  if (functionMode == 1)
  {
    distance_cm = ultasonic();
    Serial.print("distance: ");
    Serial.print(distance_cm);
    Serial.println(" cm");
    delay(1000);
  }
  
  if (functionMode == 1 && distance_cm <= 30.0)
  {
    functionMode = 0;
    Serial.println("off");
    client.publish(mqtt_topic, "1");
    client.publish(mqtt_topic, "2");
    Serial.println("sent!!");
    myServo.write(180);
    delay(3000);
    myServo.write(0);  
  }
}
