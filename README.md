# InputCommandReader

Simple Arduino Lib for handling Serial input as commands

By using this library you will enable a commands parser on the main Serial interface of the arduino (the one used to debug while connected to PC through USB).

Each line on the Serial interface will be interpreted as one or more commands, each one compsed by:

```
<CommandOpCode> param1 param2 param3 ...
```

- **CommandOpCode**: string identifying the command. Command identifier is case sensitive and can contain any "small" amount of chars. **Small** means 19 or less chars.
- **params**: list of param values (limited to 5 per command), params can be any string, int or float value.
- **ParamsSeparator**: Whitespaces will be used as opCode and params separator.

If using multi command lines, each line will be interpreted as:

```
<CommandOpCode> param1 param2 param3 ... <SeparatorChar><CommandOpCode> param1 ...
```
- **CommandOpCode**: string identifying the command. Command identifier is case sensitive and can contain any "small" amount of chars. **Small** means 19 or less chars.
- **params**: list of param values (limited to 5 per command), params can be any string, int or float value.
- **ParamsSeparator**: Whitespaces will be used as opCode and params separator.
- **SeparatorChar**: A char used as commands separator (see ussage section to know how to enable multi command lines).

### Installation

1. Download the library: https://github.com/wyunreal/InputCommandLibrary/archive/master.zip
2. (In the Arduino IDE) Sketch > Include Library > Add .ZIP Library > select the downloaded file > Open

### Usage

First, you need to include the library and and create an instance:

```c++
#include <Input.h>

Input input;
```

Above instance will use a buffer of 20 chars, so, max length for command + params string will be 19 chars. If you need a longer buffer, just use pass desired size in constructor:

```c++
#include <Input.h>

Input input(42);
```

Above instance will use a buffer of 42 chars.

Each command should have a function with following signature:

```c++
void commandWithParams(CommandParams &params, Stream &response) {
	...
}
```

Parameters will be provided to the function on the **CommandParams\*** object. To read params, just call the following methods passing the index (starting at 0) of the required param:

```c++
params.getParamAsInt(byte paramIndex);
params.getParamAsLongInt(byte paramIndex);
params.getParamAsFloat(byte paramIndex);
params.getParamAsString(byte paramIndex);
```

String params should be provided surrounded by 'quotes' or "double quotes". If you need the param to include some of this chars, you can surround the param with the other. Take into account no escaping is supported.

Also, command functions will be able printing its response through the **response** parameter. This parameter is of type **Stream**, which have all **Serial** methods for printing.

Last, you need to start the library by calling:

```c++
input.begin(bauds, commandDefinitions);
```

where:

- **bauds** represents the baudRate you want to open Serial with.
- **commandDefinitions**: is an array of **InputCommand** instances, used to define each command. **InputCommand** constructor requires 3 params: - **commandOpCode**: op code of the param, used to identify the param received on the **Serial** interface. - **paramsCount**: number of parameters used by the command. - **commandFunction**: reference to the function containing the code for the param.

To enable **multi command lines**, just specify a commands separator char when calling **begin** method:

```c++
input.begin(bauds, commandsSeparatorChar, commandDefinitions);
```

- **commandsSeparatorChar**: a char used to sepatate commands on a multi commands line, take into account this char can not be used in command op codes nor params.

Commands should be defined using following macros:

```c++
defineCommands(...)
command(opCode, paramsCount, &commandFunction)
```

These macros can be used as follow:

```c++
const InputCommand commandDefinitions[] PROGMEM = defineCommands(
  command("com1", 3, &commandWithParams),
  command("com2", 0, &commandWithNoParams)
);
```

Take into account the library expects the commands definition be stored in program memory (to save precious RAM). Thats why the use of **const** and **PROGMEM** are mandatory in above code.

To get started, just copy the following example.

# Complete example:

```c++
#include <Input.h>

Input input(100);

void commandWithParams(CommandParams &params, Stream &response) {
  // do command business here and then fullfill the command response:
  response.print("command 1: ");
  response.print(params.getParamAsLongInt(0));
  response.print(" ");
  response.print(params.getParamAsFloat(1));
  response.print(" ");
  response.println(params.getParamAsString(2));
}

void commandWithNoParams(CommandParams &params, Stream &response) {
  // do command business here and then fullfill the command response:
  response.println("command 2");
}

const InputCommand commandDefinitions[] PROGMEM = defineCommands(
  command("com1", 3, &commandWithParams),
  command("com2", 0, &commandWithNoParams)
);

void setup() {
  // initialize input command reader with main Serial at 9600 bauds
  input.begin(9600, commandDefinitions);
  // use following call if you want to enable multi commands line, using '|' char, for example
  input.begin(9600, '|', commandDefinitions);
}

void loop() {
  // your sketch can do here its main tasks
}
```

# Contribute

If you want to contribute, just do a pull request here at Github !
