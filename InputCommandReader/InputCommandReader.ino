#include "InputCommandReader.h"
#include <MemoryFree.h>

InputCommandReader inputCommandReader;

void memCommand(InputCommandParam** params) {
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
}

void com1Command(InputCommandParam** params) {
  Serial.print("command 1: ");
  Serial.print(params[0]->asInt());
  Serial.print(" ");
  Serial.print(params[1]->asFloat());
  Serial.print(" ");
  Serial.println(params[2]->asString());
}

InputCommandDefinition* commandDefinitions[] = {
  new InputCommandDefinition("mem", 0, &memCommand),
  new InputCommandDefinition("com1", 3, &com1Command),
  NULL
};

void setup() {
  // initialize input command reader with main Serial at 9600 bauds
  inputCommandReader.begin(9600, commandDefinitions);
}

void loop() {
  // your sketch can do here its main tasks
}


