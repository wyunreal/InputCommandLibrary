# InputCommandReader

Simple Arduino Lib for handling Serial input as commands

By using this library you will enable a commands parser on the different Serial interfaces of the arduino, including the USB connection used to debug while connected to PC and also Serial1, Serial2 and Serial3 if available.

Each line on the Serial interface will be interpreted as one or more commands, each one compsed by:

```
<CommandOpCode> address param1 param2 param3 ...
```

-   **CommandOpCode**: string identifying the command. Command identifier is case sensitive and can contain any "small" amount of chars. **Small** means 19 or less chars.
-   **address**: required only if the Input instance is not Slave and an address is configured.
-   **params**: list of param values (limited to 5 per command), params can be any string, int or float value.
-   **ParamsSeparator**: Whitespaces will be used as opCode and params separator.

If using multi command lines, each line will be interpreted as:

```
<CommandOpCode> address param1 param2 param3 ... <SeparatorChar><CommandOpCode> address param1 ...
```

-   **CommandOpCode**: string identifying the command. Command identifier is case sensitive and can contain any "small" amount of chars. **Small** means 19 or less chars.
-   **address**: required only if the Input instance is not Slave and an address is configured.
-   **params**: list of param values (limited to 5 per command), params can be any string, int or float value.
-   **ParamsSeparator**: Whitespaces will be used as opCode and params separator.
-   **SeparatorChar**: A char used as commands separator (see ussage section to know how to enable multi command lines).

### Installation

1. Download the library: https://github.com/wyunreal/InputCommandLibrary/archive/master.zip
2. (In the Arduino IDE) Sketch > Include Library > Add .ZIP Library > select the downloaded file > Open

### Usage

First, you need to include the library and and create an instance:

```c++
#include <Input.h>

char buffer[50];
Input input(buffer, 50);
```

The library uses an external buffer to store input chars, you need to pass the buffer and its available lenght.

Each command should have a function with following signature:

```c++
void commandWithParams(CommandParams &params, ResponseWriter &response) {
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

Also, command functions will be able printing its response through the **response** parameter. This parameter is of type **ResponseWriter**, which have all **Serial** methods for printing.

**Setting custom response writer**

The above **ResponseWriter &response** parameter can be overrided using **responseWriter**:

```c++

class CustomWriter : public ResponseWriter
{
public:
    // You can override the print or println method you need
    virtual size_t print(const char value[])
    {
    	// Do custom stuff and print using ResponseWriter::print or ResponseWriter::println methods:
        size_t size = ResponseWriter::print(value);
        size += ResponseWriter::print("some custom data");
        return size;
    }
};

CustomWriter customWriter;

input.responseWriter(&customWriter);
```

**Selecting a port**

Before starting to listen for input commands, you should specify the port, this can be done calling the port(serialId) function:

```c++
input.port(SERIAL_ID_1);
```

where the param represents the hardware serial interface to be used, accepted values are:

-   SERIAL_ID_0
-   SERIAL_ID_1
-   SERIAL_ID_2
-   SERIAL_ID_3

SERIAL_ID_0 is the port connected to the USB in most boards.

If you don't call this method, the used serial will be **SERIAL_ID_O**

**Configuring an address**

You can filter input serial commands by address, that is, if you call the **address(char\*)** method, providing a valid address (any non empty C string), the library will execute commands only if provided address matches the address specified by the command.

```c++
input.address(ADDRESS);
```

If an address is configured, all incoming commands should have an address string before parameters.

**Specifying if you will need request id handling**

```c++
input.withRequestId();
```
By calling above function, you will make explicit that all input commands will specify a request id (integer), right after the command op code. This request Id will be injected to the response writer on every command execution and also will be available in the params object (by calling the getRequestId() method).

**Starting the library by calling begin**

```c++
input.begin(bauds, commandDefinitions);
```

where:

-   **bauds** represents the baudRate you want to open Serial with.
-   **commandDefinitions**: is an array of **InputCommand** instances, used to define each command. **InputCommand** constructor requires 3 params: - **commandOpCode**: op code of the param, used to identify the param received on the **Serial** interface. - **paramsCount**: number of parameters used by the command. - **commandFunction**: reference to the function containing the code for the param.

To enable **multi command lines**, just specify a commands separator char when calling **begin** method:

```c++
input.begin(bauds, commandsSeparatorChar, commandDefinitions);
```

-   **commandsSeparatorChar**: a char used to sepatate commands on a multi commands line, take into account this char can not be used in command op codes nor params.

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

**Providing NOT FOUND or FORBIDDEN handler**

A **Not found** handler can be configured by adding a command deffinition containing an empty opcode and 0 params count at the end of commandDefinitions array:

```c++
const InputCommand commandDefinitions[] PROGMEM = defineCommands(
  command("com1", 3, &commandWithParams),
  command("com2", 0, &commandWithNoParams),
  command("", 0, &notFoundHandler)
);
```

The library will call the **NOT FOUND** handler when a command not found or invalid address, passing, as param 0, the complete command string.

To get started, just copy the following example.

# Complete example:

```c++
#include <Input.h>

Input input(100);

void commandWithParams(CommandParams &params, ResponseWriter &response) {
  // do command business here and then fullfill the command response:
  response.print("command 1: ");
  response.print(params.getParamAsLongInt(0));
  response.print(" ");
  response.print(params.getParamAsFloat(1));
  response.print(" ");
  response.println(params.getParamAsString(2));
}

void commandWithNoParams(CommandParams &params, ResponseWriter &response) {
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
  // input.begin(9600, '|', commandDefinitions);
}

void loop() {
  // your sketch can do here its main tasks
}
```

# Contribute

If you want to contribute, just do a pull request here at Github !
