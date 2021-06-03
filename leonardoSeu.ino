




void setup()
{
  Serial.begin(115200); //This pipes to the serial monitor
  while (!Serial);
  Serial1.begin(115200); //This is the UART, pipes to sensors attached to board

  DynamicJsonDocument doc(2048);

  char json[] = "{\"hello\":\"world\"}";
  deserializeJson(doc, json);

  const char* world = doc["hello"];
}

void loop()
{
  if ( Serial.available() )
  {
    int inByte = Serial.read();
    Serial1.write( inByte );
  }
  if ( Serial1.available() )
  {
    int inByte = Serial1.read();
    Serial.write( inByte );
  }
}
