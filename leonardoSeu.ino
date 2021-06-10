#define KEY_LEFT_CTRL  0x80
#define KEY_LEFT_SHIFT 0x81
#define KEY_LEFT_ALT   0x82
#define KEY_LEFT_GUI   0x83
#define KEY_RIGHT_CTRL 0x84
#define KEY_RIGHT_SHIFT    0x85
#define KEY_RIGHT_ALT  0x86
#define KEY_RIGHT_GUI  0x87

#define KEY_UP_ARROW   0xDA
#define KEY_DOWN_ARROW 0xD9
#define KEY_LEFT_ARROW 0xD8
#define KEY_RIGHT_ARROW    0xD7
#define KEY_BACKSPACE  0xB2
#define KEY_TAB        0xB3
#define KEY_RETURN 0xB0
#define KEY_ESC        0xB1
#define KEY_INSERT 0xD1
#define KEY_DELETE 0xD4
#define KEY_PAGE_UP    0xD3
#define KEY_PAGE_DOWN  0xD6
#define KEY_HOME   0xD2
#define KEY_END        0xD5
#define KEY_CAPS_LOCK  0xC1
#define KEY_F1     0xC2
#define KEY_F2     0xC3
#define KEY_F3     0xC4
#define KEY_F4     0xC5
#define KEY_F5     0xC6
#define KEY_F6     0xC7
#define KEY_F7     0xC8
#define KEY_F8     0xC9
#define KEY_F9     0xCA
#define KEY_F10        0xCB
#define KEY_F11        0xCC
#define KEY_F12        0xCD

int buttonPin = 2;  // Set a button to any pin
#include <Keyboard.h>
#include <Mouse.h>
#include <ArduinoJson.h>

const uint32_t MAX_INPUT_LENGTH = 128;

char input[MAX_INPUT_LENGTH];

StaticJsonDocument<96> doc;


String inputString = "";         // a String to hold incoming data
bool stringCompleted = false;  // whether the string is complete
bool stringStarted = false;  // whether the string is complete

volatile uint32_t i = 0;



bool processCommandKeyboard(char * command) {
  if ((unsigned)strlen(command) == 1) {
    Keyboard.write(command[0]);
    return true;
  }
  return false;
}

bool processActionMouse(int x, int y) {
  Mouse.move(x, y);
  return true;
}

bool processData(char * input, int inputLength) {

  DeserializationError error = deserializeJson(doc, input, inputLength);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return false;
  }
  Serial.println("todo OK");

  if (!(doc.containsKey("device") && doc.containsKey("type") && doc.containsKey("command"))) {
    return false;
  }

  const char* nodeName = doc["name"]; // "node0001"
  const char* confType = doc["type"]; // "k"
  const char* command = doc["command"]; // "w"

  int data_0 = doc["data"][0]; // 1.29876
  int data_1 = doc["data"][1]; // 2.34567
  if (confType[0] == 'k') {
    return processCommandKeyboard(command);
  }
  if (confType[0] == 'm') {
    return processActionMouse(data_0, data_1);
  }
  return true;
  

}


void setup()
{
  Serial.begin(115200); //This pipes to the serial monitor

  Serial1.begin(115200); //This is the UART, pipes to sensors attached to board

  pinMode(buttonPin, INPUT);  // Set the button as an input
  digitalWrite(buttonPin, HIGH);  // Pull the button high
  Mouse.begin();
  Keyboard.begin();
}

void loop()
{

  if ( Serial.available() )
  {
    Serial.println("Sending...");
    int inByte = Serial.read();
    Serial1.write( inByte );
  }

  if (!stringStarted && Serial1.available() ) {
    char inChar = (char)Serial1.read();
    // add it to the inputString:
    input[i] = inChar;
    if (inChar == '{') {
      stringStarted = true;
      i += 1;
    }
  }
  if (stringStarted && !stringCompleted && Serial1.available() ) {
    char inChar = (char)Serial1.read();
    // add it to the inputString:
    input[i] = inChar;
    if (inChar == '}') {
      stringCompleted = true;
    }
    i += 1;
  }
  if (stringCompleted) {
    Serial.print("Completed! What do we have: ");
    Serial.println(input);
    if (processData(input, i)) {
      Serial1.write("ok");
    }
    i = 0;
    stringCompleted = false;
    stringStarted = false;
  }
}
