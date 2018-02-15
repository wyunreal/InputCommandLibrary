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
void commandWithParams(CommandParam** params, Stream* response) {
	...
}
```

Parameters will be provided to the function on the **CommandParam*** array, each instance on this array will provide a method for the type required (int, float or string):

``` c++
params[x]->asInt();
params[x]->asFloat();
params[x]->asString();
```

where **x** is the index of required parameter.

Also, command functions will be able printing its response through the **response** parameter. This parameter is of type **Stream**, which have all **Serial** methods for printing.

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

In order to determine the number of defined commands, the library requires you to pass a NULL value as last element in the commands definition array. In order to simplify this, the library provides two macros:

``` c++
defineCommands(...)
command(opCode, paramsCount, &commandFunction)
```

These macros can be used as follow:

``` c++
InputCommand* commandDefinitions[] = defineCommands(
  command("com1", 3, &commandWithParams),
  command("com2", 0, &commandWithNoParams)
);
```

To get started, just copy the following example.

# Complete example:

``` c++
#include <Input.h>

Input input;

void commandWithParams(CommandParam** params, Stream* response) {
  // do command business here and then fullfill the command response:
  response->print("command 1: ");
  response->print(params[0]->asInt());
  response->print(" ");
  response->print(params[1]->asFloat());
  response->print(" ");
  response->println(params[2]->asString());
}

void commandWithNoParams(CommandParam** params, Stream* response) {
  // do command business here and then fullfill the command response:
  response->println("command 2");
}

InputCommand* commandDefinitions[] = defineCommands(
  command("com1", 3, &commandWithParams),
  command("com2", 0, &commandWithNoParams)
);

void setup() {
  // initialize input command reader with main Serial at 9600 bauds
  input.begin(9600, commandDefinitions);
}

void loop() {
  // your sketch can do here its main tasks
}
```

# Triggering commands manually

Sometimes, you need to manually trigger a command manually (in the code of you sketch), to do so, you can call the trigger method:

```
void Input::trigger (char * commandLine);
```

This method will behave like if the command represented by the provided command line get read from Serial.
**Warning:** This method can NOT be executed inside a command handler. If done, memory will leak.
# Contribute

If you want to contribute, just do a pull request here at Github !
