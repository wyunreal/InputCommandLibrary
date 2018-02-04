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

To use this library, just download as ZIP, unzip and copy the **Input** folder to your arduino's library folder.

First, you need to include the library and and create an instance:

``` c++
#include <Input.h>

Input input;
```

Each command should have a function with following signature:

``` c++
void commandWithParams(InputCommandParam** params) {
	...
}
```

Parameters will be provided to the function on the **InputCommandParam*** array, each instance on this array will provide a method for the type required (int, float or string):

``` c++
params[x]->asInt();
params[x]->asFloat();
params[x]->asString();
```

where **x** is the index of required parameter.

Last, you need to start the library by calling:

``` c++
input.begin(bauds, commandDefinitions);
```

where:
- **bauds** represents the baudRate you want to open Serial with.
- **commandDefinitions**: is an array of **InputCommand** instances, used to define each command. **InputCommand** constructor requires 3 params:
- - **commandOpCode**: op code of the param, used to identify the param received on the **Serial** interface.
- - **paramsCount**: number of parameters used by the command.
- - **commandFunction**: reference to the function containing the code for the param.

To get started, just copy the following example.

# Complete example:

``` c++
#include <Input.h>

Input input;

void commandWithParams(InputCommandParam** params) {
  Serial.print("command 1: ");
  Serial.print(params[0]->asInt());
  Serial.print(" ");
  Serial.print(params[1]->asFloat());
  Serial.print(" ");
  Serial.println(params[2]->asString());
}

void commandWithNoParams(InputCommandParam** params) {
  Serial.println("command 2");
}

InputCommand* commandDefinitions[] = {
  new InputCommand("com1", 3, &commandWithParams),
  new InputCommand("com2", 0, &commandWithNoParams),
  NULL
};

void setup() {
  // initialize input command reader with main Serial at 9600 bauds
  input.begin(9600, commandDefinitions);
}

void loop() {
  // your sketch can do here its main tasks
}
```

# Contribute

If you want to contribute, just do a pull request here at Github !
