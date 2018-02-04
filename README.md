# InputCommandReader

Simple Arduino Lib for handling Serial input as commands

By using this library you will enable a basic command parser on the main Serial interface of the arduino (the one used to debug while connected to PC through USB).

Each line on the Serial interface will be interpreted as a command, comosed by:

```
CommandOpCode param1 param2 param3 ...
```

- **CommandOpCode**: string identifying the command. Command identifier is case sensitive and can contain any "small" amount of chars.
- **params**: list of param values, params can be any string, int or float value.
- **Separator**: Whitespaces will be used as opCode and params separator.

# Using the library:

To use this library, just download as ZIP, unzip and copy to your arduino's library folder.

Example:

``` c++
#include "InputCommandReader.h"

InputCommandReader inputCommandReader;

void commandWithParams(InputCommandParam** params) {
  Serial.print("command 1: ");
  Serial.print(params[0]->asInt());
  Serial.print(" ");
  Serial.print(params[1]->asFloat());
  Serial.print(" ");
  Serial.println(params[2]->asString());
}

void commandWithNoParams(InputCommandParam** params) {
  Serial.print("command 2: ");
}

InputCommand* commandDefinitions[] = {
  new InputCommand("mem", 0, &commandWithParams),
  new InputCommand("com1", 3, &commandWithNoParams),
  NULL
};

void setup() {
  // initialize input command reader with main Serial at 9600 bauds
  inputCommandReader.begin(9600, commandDefinitions);
}

void loop() {
  // your sketch can do here its main tasks
}
```

# Contribute

If you want to contribute, just do a pull request here at Github !
