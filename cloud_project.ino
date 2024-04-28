#include <DHT.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <SSD1306Wire.h>
#include <ThingSpeak.h>

int smokeA0 = A0;

SSD1306Wire display(0x3c, D2, D1); 


// Replace with your network credentials
const char* ssid = "sk";
const char* password = "12344321";

// Replace with your ThingSpeak channel details
unsigned long channelID = 2226903;
const char* apiKey = "0D3IV6V650WQO14G";

// DHT sensor setup
#define RESET LED_BUILTIN
#define DHTPIN D5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Initialize client to send data to ThingSpeak
WiFiClient client;

void setup() {
  pinMode(smokeA0, INPUT);
  Serial.begin(9600);
  delay(100);
  display.init();
  display.flipScreenVertically();

  display.setColor(WHITE);
  display.setFont(ArialMT_Plain_10);


  display.clear();
  display.drawString(0, 0, "Connecting to WiFi");
  display.display();


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");

  }
  Serial.println("Connected to WiFi.");
  display.clear();

  

  // Initialize DHT sensor
  dht.begin();
}

void loop() {

  int analogSensor = analogRead(smokeA0);


  // Read temperature and humidity from DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  int tempf = (temperature * 9/5) + 32 ;

  // Print the readings to the Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  
  display.clear();
  display.drawString(5, 0, "Humidity");
  display.drawString(60, 0, String(humidity) + "%");
  display.drawString(5, 30, "Temperature");
  display.drawString(80, 30, String(temperature) + "C");
  display.drawString(80, 45, String(tempf) + "F");
  display.display(); 
 

  
 
  display.display(); 






  Serial.print("Pin A0: ");
  Serial.println(analogSensor);

  // Update ThingSpeak with the readings
  updateThingSpeak(temperature, humidity, analogSensor, channelID, apiKey);

  // Wait for 20 seconds before sending data to ThingSpeak again
  delay(15000);
}




void updateThingSpeak(float temp, float hum, int analogSensor, unsigned long channel, const char* api_key) {
  ThingSpeak.begin(client);
  
  int f1 = 0;
  int f2 = 0;

  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, hum);
  ThingSpeak.setField(3, analogSensor);
  ThingSpeak.setField(4, f1);
  ThingSpeak.setField(5, f2);
  
  int x = ThingSpeak.writeFields(channel, api_key);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  
}
