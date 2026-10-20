// Add header of the library to the source files.
#include "InkplatePeripheralMode.h"

// Include file describing the settings of the peripheral mode.
#include "settings.h"

// Include the list with all available commands.
#include "commandList.h"

// Define the static member.
PeripheralMode* PeripheralMode::_instancePtr = nullptr;

bool PeripheralMode::begin(HardwareSerial *_serialPtr, Inkplate *_inkplatePtr, uint32_t _baud, uint8_t _rxPin, uint8_t _txPin, uint32_t _size)
{
    // Store every parameter locally and check them.
    if ((_serialPtr == nullptr) || (_inkplatePtr == nullptr) || (_size == 0)) return false;
    _serial = _serialPtr;
    _display = _inkplatePtr;
    _bufferSize = _size;

    // Init the serial communication @ desired communication speed. Also set the pins.
    _serial->begin(_baud, SERIAL_8N1, _rxPin, _txPin);

    // Allocate memory for the serial buffer in the PSRAM.
    _serialBuffer = (char*)ps_malloc(_bufferSize * sizeof(char));

    // Check if allocation is successful.
    if (_serialBuffer == NULL)
    {
        return false;
    }

    // If everything is ok, return true.
    return true;
}

bool PeripheralMode::getDataFromSerial(unsigned long _timeout)
{
    // Variable used for timeouting incoming serial data.
    unsigned long _timestamp;

    // Variables for parsing.
    char *_commandEndPosition = NULL;
  
    if (_serial->available())
    {
        // You got something on the serial? Cool!
        // First capture the current time (needed for timeouting!).
        _timestamp = millis();

        // Capture the timestamp for automatic buffer cleaning.
        _bufferAutoCleanTimestamp = millis();

        // Try to capture incoming serial data until the timeout or until the buffer is full!
        while (((unsigned long)(millis() - _timestamp) < _timeout) && (_serialBufferIndex < (_bufferSize - 2)))
        {
            if (_serial->available())
            {
                _serialBuffer[_serialBufferIndex++] = _serial->read();
                _timestamp = millis();
            }
        }

        // Add a nul-terminating char.
        _serialBuffer[_serialBufferIndex] = '\0';

        // Go though all commands that are received.
        _commandEndPosition = _serialBuffer;
        while (_commandEndPosition != NULL)
        {
            // Variables for parsing.
            int _command = 0;
            int _repeatable = 0;
            int _payloadSize = 0;
            char *_payload = NULL;

            // Check received for validity. If everything went ok, parse the command.
            if (checkPacket(_commandEndPosition,  _bufferSize - (uint32_t)(_commandEndPosition - _serialBuffer), &_command, &_repeatable, &_payloadSize, &_payload, &_commandEndPosition))
            {
                // If is a valid command, try to parse it.
                parseCommand(_command, _repeatable, _payloadSize, _payload);

                // reset the counter.
                _serialBufferIndex = 0;

                // Go to the next command by advancing to the next index.
                _commandEndPosition++;
            }
        }
    }

    // Periodically clear the buffer from the remained invalid data.
    if (((unsigned long)(millis() - _bufferAutoCleanTimestamp) >= SERIAL_BUFFER_CLEAN_TIME_MS) && _serialBufferIndex)
    {
        // Clear the buffer.
        memset(_serialBuffer, 0, _bufferSize);
        
        // reset the index variable.
        _serialBufferIndex = 0;

        Serial.println("Cleaned");
    }

    // If data is not valid, return false;
    return false;
}

bool PeripheralMode::checkPacket(char *_buffer, uint32_t _maxSize, int *_command, int *_repeatableFlag, int *_payloadSize, char **_payloadDataStart, char **_commandLastPosition)
{
    // Prepare the command end position pointer - set it to null. Later in the code it will be set to some address that corresponds end position of the command.
    // If not found, it will stay on null.
    *_commandLastPosition = NULL;

    // -------------------PARSING START AND STOP FLAGS-------------------
    // First check if there is start and stop flags.
    char *_startFlag = strstr(_buffer, "TS");

    // Check start flag. If the start flag is wrong, return error.
    if (_startFlag == NULL) return false;

    // Update the end of processing flag;
    *_commandLastPosition = _startFlag;

    Serial.printf("Passed 1st check!\n");
    Serial.flush();

    // -------------------PARSING COMMAND-------------------
    // Check the command and payload size.
    // First find the first semicolon.
    char *_cmd = strchr(_startFlag + 1, TEMR_CHAR);

    // If it's NULL, something is wrong, return NULL.
    if (_cmd == NULL) return false;

    // Update the end of processing flag;
    *_commandLastPosition = _cmd;

    Serial.printf("Passed 2nd check!\n");
    Serial.flush();

    // Check if there is a command. Move pointer by one place to the right (skip the semicolon).
    int _cmdDataInt = strtol(_cmd + 1, NULL, 16);

    // If command if equal to zero, something is wrong.
    if (_cmdDataInt == 0) return false;

    Serial.printf("Passed 3rd check!\n");
    Serial.flush();

    // -------------------PARSING REPEATABLE FLAG-------------------
    // Get the repeatable flag.
    char *_repeatable = strchr(_cmd + 1, TEMR_CHAR);

    // If it's NULL, something is wrong, return NULL.
    if (_repeatable == NULL) return false;

    // Update the end of processing flag;
    *_commandLastPosition = _repeatable;

    Serial.printf("Passed 4th check!\n");
    Serial.flush();

    // Get the repeatable flag data from HEX. Move one place to the right (skip the semicolon).
    int _repeatableFlagInt = strtol(_repeatable + 1, NULL, 16);

    // Repeatable flag can only be one or zero. Discard otherwise.
    if (_repeatableFlagInt > 1 || _repeatableFlagInt < 0) return false; 

    Serial.printf("Passed 5th check!\n");
    Serial.flush();

    // -------------------PARSING PAYLOAD SIZE-------------------
    // Go to the next semicolon (payload size).
    char *_payload = strchr(_repeatable + 1, TEMR_CHAR);

    // If it's NULL, something is wrong, return NULL.
    if (_payload == NULL) return false;

    // Update the end of processing flag;
    *_commandLastPosition = _payload;

    Serial.printf("Passed 6th check!\n");
    Serial.flush();

    // Get the number!
    int _payloadSizeInt = strtol(_payload + 1, NULL, 16);

    // Go to the start of the payload by finding next semicolon.
    char *_payloadDataInt = strchr(_payload + 1, TEMR_CHAR);

    if (_payloadDataInt == NULL) return false;

    // Update the end of processing flag;
    *_commandLastPosition = _payloadDataInt;

    // -------------------CHECKING THE STOP FLAG-------------------
    // Go to the next semicolon, which is for the stop flag.
    // For the stop flag is a little bit more complicated. Since this can be found in the payload, we need to find the last one 0xaa in the buffer.
    // For that we are gonna try to go though the whole command and check if the stop flag is at the end. If not, command is considered invalid.
    char *_stopFlag = strchr(_payloadDataInt + 1 + _payloadSizeInt, TEMR_CHAR);

    if (_stopFlag == NULL) return false;

    // Update the end of processing flag;
    *_commandLastPosition = _stopFlag;

    // Check if the stop flag is really there. If not, return with fail.
    if (strstr(_stopFlag, "TE") != (_stopFlag + 1)) return false;

    // Update the stop flag position by one (move after the semicolon).
    _stopFlag++;

    Serial.printf("Passed 7th check!\n");
    Serial.flush();

    // -------------------PARSING PAYLOAD DATA-------------------
    // Increment the pointer. Now we're at the first byte of the payload.
    _payloadDataInt++;
    
    // Check for the size. If the size is ok, fill the command, payload size and payload data start pointers.
    if ((_stopFlag - _payloadDataInt - 1) == _payloadSizeInt)
    {
        *_command = _cmdDataInt;
        *_repeatableFlag = _repeatableFlagInt;
        *_payloadSize = _payloadSizeInt;
        *_payloadDataStart = _payloadDataInt;
        return true;
    }

    Serial.printf("ERROR: stopFlag: %d payloadDataInt: %d payloadSizeInt %d\n", _stopFlag, _payloadDataInt, _payloadSizeInt);
    Serial.flush();

    // Otherwise, return null, indicating an error.
    return false;
}

void PeripheralMode::sendResponse(int _command, int _payloadSize, char *_payload)
{
    _serial->printf("TS;%04X;%04X;%s;TE\r\n", _command, _payloadSize, _payload);
}

char* PeripheralMode::getArgument(char *_inBuffer, uint32_t _payloadSize, int _noOfArgs, int _n, uint32_t *_size)
{
    // Count already found arguments.
    int _currentArgument = 0;

    // If is selected argument that does not exist, return NULL pointer as error;
    if (_n >= _noOfArgs || _n < 0) return NULL;
    
    // Try get the wanted arguments.
    // First declare two pinters; one represents start of the argument, second end.
    // In initial state, start of the first argument is, well, start of the payload buffer.
    // End of the first argument is start of the second argument -> semicolon.
    char *_start = _inBuffer;
    char *_stop = (char*)memchr((char*)(_inBuffer + 1), TEMR_CHAR, _payloadSize);
    
    // Run trough the loop until you hit the right argument.
    while ((_currentArgument != _n) && (_start != NULL))
    {
        // Copy end of the last argument and move the pointer by one place (to skip the semicolon).
        _start = _stop + 1;
        
        // Try to find, next semicolon (or start of the next argument)
        _stop = (char*)memchr((char*)(_stop + 1), TEMR_CHAR, _payloadSize - (_stop - _inBuffer) + 1);
        
        // Increment argument counter.
        _currentArgument++;
    }
    
    // Return what has been found and also return the size of the argument.
    if (_size != NULL) *_size = _stop - _start;
    return _start;
}

int PeripheralMode::getNumberOfArgs(char *_inBuffer, uint32_t _payloadSize)
{
    // Find the total number of arguments.
    int _noOfArgs = 0;

    // Count every semicolon (add also one at the end of the payload).
    for (int i = 0; i < _payloadSize + 1; i++)
    {
        if (_inBuffer[i] == TEMR_CHAR) _noOfArgs++;
    }

    // Return the number.
    return _noOfArgs;
}

void PeripheralMode::parseCommand(int _command, int _repeat, int _payloadSize, char* _payload)
{
    // Parse the command. You can add command if needed.
    // First find the number of sent arguments.
    int _numberOfArgs = getNumberOfArgs(_payload, _payloadSize);

    switch(_command)
    {
        case CMD_PING:
        {
            // Payload doesn't matter here, just send the response.
            sendResponse(CMD_PING, strlen((char*)_okResponseString), (char*)_okResponseString);
            break;
        }
        case CMD_DRAW_PIXEL:
        {
            // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 3, _repeat);

            // Arguments can be repeated for faster transfer, so multiple pixels can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=3)
            {
                // Get the arguments and their values.
                char *_argX;
                char *_argY;
                char *_argColor;
                _argX = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argY = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);

                // Check if they are vaild.
                if (_argX == NULL || _argY == NULL || _argColor == NULL) return;

                // Write the pixels.
                _display->drawPixel(atol(_argX), atol(_argY), atol(_argColor));
            }
            break;
        }
        case CMD_DRAW_LINE:
        {
             // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 5, _repeat);

            // Arguments can be repeated for faster transfer, so multiple lines can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=5)
            {
                // Get the arguments and their values.
                char *_argX1;
                char *_argY1;
                char *_argX2;
                char *_argY2;
                char *_argColor;
                _argX1 = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argY1 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argX2 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);
                _argY2 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 3, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 4, NULL);

                // Check if they are valid.
                if (_argX1 == NULL || _argY1 == NULL || _argX2 == NULL || _argY2 == NULL || _argColor == NULL) return;

                // Write the lines.
                _display->drawLine(atol(_argX1), atol(_argY1), atol(_argX2), atol(_argY2), atol(_argColor));
            }
            break;
        }
        case CMD_DRAW_FASTVLINE:
        {
             // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 4, _repeat);

            // Arguments can be repeated for faster transfer, so multiple lines can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=4)
            {
                // Get the arguments and their values.
                char *_argX;
                char *_argY;
                char *_argH;
                char *_argColor;
                _argX = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argY = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argH = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 3, NULL);

                // Check if they are valid.
                if (_argX == NULL || _argY == NULL || _argH == NULL || _argColor == NULL) return;

                // Write the lines.
                _display->drawFastVLine(atol(_argX), atol(_argY), atol(_argH), atol(_argColor));
            }
            break;
        }
        case CMD_DRAW_FASTHLINE:
        {
             // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 4, _repeat);

            // Arguments can be repeated for faster transfer, so multiple lines can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=4)
            {
                // Get the arguments and their values.
                char *_argX;
                char *_argY;
                char *_argW;
                char *_argColor;
                _argX = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argY = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argW = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 3, NULL);

                // Check if they are vaild.
                if (_argX == NULL || _argY == NULL || _argW == NULL || _argColor == NULL) return;

                // Write the lines.
                _display->drawFastHLine(atol(_argX), atol(_argY), atol(_argW), atol(_argColor));
            }
            break;
        }
        case CMD_DRAW_RECT:
        {
             // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 5, _repeat);

            // Arguments can be repeated for faster transfer, so multiple rectangles can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=5)
            {
                // Get the arguments and their values.
                char *_argX;
                char *_argY;
                char *_argW;
                char *_argH;
                char *_argColor;
                _argX = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argY = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argW = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);
                _argH = getArgument(_payload, _payloadSize, _numberOfArgs, i + 3, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 4, NULL);

                // Check if they are vaild.
                if (_argX == NULL || _argY == NULL || _argW == NULL || _argH == NULL || _argColor == NULL) return;

                // Write the lines.
                _display->drawRect(atol(_argX), atol(_argY), atol(_argW), atol(_argH), atol(_argColor));
            }
            break;
        }
        case CMD_DRAW_CIRCLE:
        {
             // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 4, _repeat);

            // Arguments can be repeated for faster transfer, so multiple circles can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=4)
            {
                // Get the arguments and their values.
                char *_argX;
                char *_argY;
                char *_argR;
                char *_argColor;
                _argX = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argY = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argR = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 3, NULL);

                // Check if they are vaild.
                if (_argX == NULL || _argY == NULL || _argR == NULL || _argColor == NULL) return;

                // Write the lines.
                _display->drawCircle(atol(_argX), atol(_argY), atol(_argR), atol(_argColor));
            }
            break;
        }
        case CMD_DRAW_TRIANGLE:
        {
             // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 7, _repeat);

            // Arguments can be repeated for faster transfer, so multiple triangles can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=7)
            {
                // Get the arguments and their values.
                char *_argX1;
                char *_argY1;
                char *_argX2;
                char *_argY2;
                char *_argX3;
                char *_argY3;
                char *_argColor;
                _argX1 = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argY1 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argX2 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);
                _argY2 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 3, NULL);
                _argX3 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 4, NULL);
                _argY3 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 5, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 6, NULL);

                // Check if they are vaild.
                if (_argX1 == NULL || _argY1 == NULL || _argX2 == NULL || _argY2 == NULL  || _argX3 == NULL || _argY3 == NULL || _argColor == NULL) return;

                // Write the lines.
                _display->drawTriangle(atol(_argX1), atol(_argY1), atol(_argX2), atol(_argY2), atol(_argX3), atol(_argY3), atol(_argColor));
            }
            break;
        }
        case CMD_DRAW_ROUND_RECT:
        {
             // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 6, _repeat);

            // Arguments can be repeated for faster transfer, so multiple rounded rectangles can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=6)
            {
                // Get the arguments and their values.
                char *_argX;
                char *_argY;
                char *_argW;
                char *_argH;
                char *_argR;
                char *_argColor;
                _argX = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argY = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argW = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);
                _argH = getArgument(_payload, _payloadSize, _numberOfArgs, i + 3, NULL);
                _argR = getArgument(_payload, _payloadSize, _numberOfArgs, i + 4, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 5, NULL);

                // Check if they are vaild.
                if (_argX == NULL || _argY == NULL || _argW == NULL || _argH == NULL  || _argR == NULL || _argColor == NULL) return;

                // Write the lines.
                _display->drawRoundRect(atol(_argX), atol(_argY), atol(_argW), atol(_argH), atol(_argR), atol(_argColor));
            }
            break;
        }
        case CMD_DRAW_THICK_LINE:
        {
             // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 6, _repeat);

            // Arguments can be repeated for faster transfer, so multiple rounded rectangles can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=6)
            {
                // Get the arguments and their values.
                char *_argX1;
                char *_argY1;
                char *_argX2;
                char *_argY2;
                char *_argColor;
                char *_argThick;
                _argX1 = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argY1 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argX2 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);
                _argY2 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 3, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 4, NULL);
                _argThick = getArgument(_payload, _payloadSize, _numberOfArgs, i + 5, NULL);

                // Check if they are vaild.
                if (_argX1 == NULL || _argY1 == NULL || _argX2 == NULL || _argY2 == NULL  || _argColor == NULL || _argThick == NULL) return;

                // Write the lines.
                _display->drawThickLine(atol(_argX1), atol(_argY1), atol(_argX2), atol(_argY2), atol(_argColor), atol(_argThick));
            }
            break;
        }
        case CMD_DRAW_ELIPSE:
        {
             // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 5, _repeat);

            // Arguments can be repeated for faster transfer, so multiple rounded rectangles can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=5)
            {
                // Get the arguments and their values.
                char *_argRx;
                char *_argRy;
                char *_argXc;
                char *_argYc;
                char *_argColor;
                _argRx = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argRy = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argXc = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);
                _argYc = getArgument(_payload, _payloadSize, _numberOfArgs, i + 3, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 4, NULL);

                // Check if they are vaild.
                if (_argRx == NULL || _argRy == NULL || _argXc == NULL || _argYc == NULL  || _argColor == NULL) return;

                // Write the lines.
                _display->drawElipse(atol(_argRx), atol(_argRy), atol(_argXc), atol(_argYc), atol(_argColor));
            }
            break;
        }
        case CMD_FILL_RECT:
        {
             // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 5, _repeat);

            // Arguments can be repeated for faster transfer, so multiple rectangles can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=5)
            {
                // Get the arguments and their values.
                char *_argX;
                char *_argY;
                char *_argW;
                char *_argH;
                char *_argColor;
                _argX = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argY = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argW = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);
                _argH = getArgument(_payload, _payloadSize, _numberOfArgs, i + 3, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 4, NULL);

                // Check if they are valid.
                if (_argX == NULL || _argY == NULL || _argW == NULL || _argH == NULL || _argColor == NULL) return;

                // Write the lines.
                _display->fillRect(atol(_argX), atol(_argY), atol(_argW), atol(_argH), atol(_argColor));
            }
            break;
        }
        case CMD_FILL_CIRCLE:
        {
             // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 4, _repeat);

            // Arguments can be repeated for faster transfer, so multiple circles can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=4)
            {
                // Get the arguments and their values.
                char *_argX;
                char *_argY;
                char *_argR;
                char *_argColor;
                _argX = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argY = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argR = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 3, NULL);

                // Check if they are valid.
                if (_argX == NULL || _argY == NULL || _argR == NULL || _argColor == NULL) return;

                // Write the lines.
                _display->fillCircle(atol(_argX), atol(_argY), atol(_argR), atol(_argColor));
            }
            break;
        }
        case CMD_FILL_TRIANGLE:
        {
             // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 7, _repeat);

            // Arguments can be repeated for faster transfer, so multiple triangles can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=7)
            {
                // Get the arguments and their values.
                char *_argX1;
                char *_argY1;
                char *_argX2;
                char *_argY2;
                char *_argX3;
                char *_argY3;
                char *_argColor;
                _argX1 = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argY1 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argX2 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);
                _argY2 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 3, NULL);
                _argX3 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 4, NULL);
                _argY3 = getArgument(_payload, _payloadSize, _numberOfArgs, i + 5, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 6, NULL);

                // Check if they are valid.
                if (_argX1 == NULL || _argY1 == NULL || _argX2 == NULL || _argY2 == NULL  || _argX3 == NULL || _argY3 == NULL || _argColor == NULL) return;

                // Write the lines.
                _display->fillTriangle(atol(_argX1), atol(_argY1), atol(_argX2), atol(_argY2), atol(_argX3), atol(_argY3), atol(_argColor));
            }
            break;
        }
        case CMD_FILL_ROUND_RECT:
        {
             // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 6, _repeat);

            // Arguments can be repeated for faster transfer, so multiple rounded rectangles can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=6)
            {
                // Get the arguments and their values.
                char *_argX;
                char *_argY;
                char *_argW;
                char *_argH;
                char *_argR;
                char *_argColor;
                _argX = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argY = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argW = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);
                _argH = getArgument(_payload, _payloadSize, _numberOfArgs, i + 3, NULL);
                _argR = getArgument(_payload, _payloadSize, _numberOfArgs, i + 4, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 5, NULL);

                // Check if they are valid.
                if (_argX == NULL || _argY == NULL || _argW == NULL || _argH == NULL  || _argR == NULL || _argColor == NULL) return;

                // Write the lines.
                _display->fillRoundRect(atol(_argX), atol(_argY), atol(_argW), atol(_argH), atol(_argR), atol(_argColor));
            }
            break;
        }
        case CMD_FILL_ELIPSE:
        {
             // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 5, _repeat);

            // Arguments can be repeated for faster transfer, so multiple rounded rectangles can be written at once.
            for (int i = 0; i < _numberOfArgs; i+=5)
            {
                // Get the arguments and their values.
                char *_argRx;
                char *_argRy;
                char *_argXc;
                char *_argYc;
                char *_argColor;
                _argRx = getArgument(_payload, _payloadSize, _numberOfArgs, i, NULL);
                _argRy = getArgument(_payload, _payloadSize, _numberOfArgs, i + 1, NULL);
                _argXc = getArgument(_payload, _payloadSize, _numberOfArgs, i + 2, NULL);
                _argYc = getArgument(_payload, _payloadSize, _numberOfArgs, i + 3, NULL);
                _argColor = getArgument(_payload, _payloadSize, _numberOfArgs, i + 4, NULL);

                // Check if they are valid.
                if (_argRx == NULL || _argRy == NULL || _argXc == NULL || _argYc == NULL  || _argColor == NULL) return;

                // Write the lines.
                _display->fillElipse(atol(_argRx), atol(_argRy), atol(_argXc), atol(_argYc), atol(_argColor));
            }
            break;
        }
        case CMD_PRINT:
        {
            // Check the number of arguments, it can only be one and do not allow repeated payload.
            if (_numberOfArgs != 1 || _repeat) return;

            // Get the arguments ant it's size.
            char *_arg;
            uint32_t _argSize;
            _arg = getArgument(_payload, _payloadSize, _numberOfArgs, 0, &_argSize);

            // Convert them from HEX ASCII to ASCII of the fly.
            hexAsciiToAscii(_arg, _argSize);

            // Add nul-terminating char at the end of the argument (text)
            _arg[_argSize / 2] = '\0';

            // Print it on display.
            _display->print(_arg);
            break;
        }
        case CMD_SET_TEXT_SIZE:
        {
            // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 1, 0);

            // Get the arguments and their values.
            char *_textSize;

            // Get the arguments ant it's size.
            _textSize = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);

            // Check if they are valid.
            if (_textSize == NULL) return;

            // Set new text size.
            _display->setTextSize(atol(_textSize));
            break;
        }
        case CMD_SET_TEXT_COLOR:
        {
            // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 2, 0);

            // Get the arguments and their values.
            char *_textColor;
            char *_bgColor;

            // Get the arguments ant it's size.
            _textColor = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            _bgColor = getArgument(_payload, _payloadSize, _numberOfArgs, 1, NULL);

            // Check if they are valid.
            if (_textColor == NULL || _bgColor == NULL) return;

            // Set new text and background color.
            _display->setTextColor(atol(_textColor), atol(_bgColor));
            break;
        }
        case CMD_SET_CURSOR:
        {
            // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 2, 0);

            // Get the arguments and their values.
            char *_xCursor;
            char *_yCursor;

            // Get the arguments ant it's size.
            _xCursor = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            _yCursor = getArgument(_payload, _payloadSize, _numberOfArgs, 1, NULL);

            // Check if they are valid.
            if (_xCursor == NULL || _yCursor == NULL) return;

            // Set new text cursor position.
            _display->setCursor(atol(_xCursor), atol(_yCursor));
            break;
        }
        case CMD_SET_TEXT_WRAP:
        {
            // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 1, 0);

            // Get the arguments and their values.
            char *_textWrap;

            // Get the arguments ant it's size.
            _textWrap = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            
            // Check if they are valid.
            if (_textWrap == NULL) return;

            // Set text wrapping..
            _display->setTextWrap(atol(_textWrap));
            break;
        }
        case CMD_SET_ROTATION:
        {
            // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 1, 0);

            // Get the arguments and their values.
            char *_rotation;

            // Get the arguments ant it's size.
            _rotation = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            
            // Check if they are valid.
            if (_rotation == NULL) return;

            // Set text wrapping..
            _display->setRotation(atol(_rotation));
            break;
        }
        case CMD_SD_CARD_INIT:
        {
            // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 1, 0);

            // Get the arguments and their values.
            char *_init;

            // Get the arguments ant it's size.
            _init = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            
            // Check if they are valid.
            if (_init == NULL && atol(_init) != 1) return;

            // Try to init microSD card
            if (_display->sdCardInit())
            {
                sendResponse(CMD_SD_CARD_INIT, strlen((char*)_okResponseString), (char*)_okResponseString);
            }
            else
            {
                sendResponse(CMD_SD_CARD_INIT, strlen((char*)_failResponseString), (char*)_failResponseString);
            }
            break;
        }
        case CMD_SD_CARD_SLEEP:
        {
            // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 1, 0);

            // Get the arguments and their values.
            char *_uSDSleep;

            // Get the arguments ant it's size.
            _uSDSleep = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            
            // Check if they are valid.
            if (_uSDSleep == NULL && atol(_uSDSleep) != 1) return;

            // Set microSD card to sleep.
            _display->sdCardSleep();
            break;
        }
        case CMD_DRAW_BITMAP:
        {
            // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 6, 0);

            // Get the arguments and their values.
            char *_xPos;
            char *_yPos;
            char *_w;
            char *_h;
            char *_color;
            char *_hexData;
            uint32_t _argSize;

            // Get the arguments ant it's size.
            _xPos = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            _yPos = getArgument(_payload, _payloadSize, _numberOfArgs, 1, NULL);
            _w = getArgument(_payload, _payloadSize, _numberOfArgs, 2, NULL);
            _h = getArgument(_payload, _payloadSize, _numberOfArgs, 3, NULL);
            _color = getArgument(_payload, _payloadSize, _numberOfArgs, 4, NULL);
            _hexData = getArgument(_payload, _payloadSize, _numberOfArgs, 5, &_argSize);

            // Check if they are valid.
            if (_xPos == NULL || _yPos == NULL || _w == NULL || _h == NULL || _color == NULL || _hexData == NULL) return;

            // Extract the data from the HEX on the fly. This can be done since the decoded data is two times smaller then original ASCII HEX data.
            hexAsciiToAscii(_hexData, _argSize);

            // Display the bitmap.
            _display->drawBitmap(atol(_xPos), atol(_yPos), (uint8_t*)_hexData, atol(_w), atol(_h), atol(_color));

            break;
        }
        case CMD_DRAW_IMAGE:
        {
            // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 7, 0);

            // Get the arguments and their values.
            char *_xPos;
            char *_yPos;
            char *_dither;
            char *_invert;
            char *_path;
            char *_pathType;
            uint32_t _argSize;

            // Get the arguments ant it's size.
            _xPos = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            _yPos = getArgument(_payload, _payloadSize, _numberOfArgs, 1, NULL);
            _dither = getArgument(_payload, _payloadSize, _numberOfArgs, 2, NULL);
            _invert = getArgument(_payload, _payloadSize, _numberOfArgs, 3, NULL);
            _pathType = getArgument(_payload, _payloadSize, _numberOfArgs, 4, NULL);
            _path = getArgument(_payload, _payloadSize, _numberOfArgs, 5, &_argSize);
            
            // Check if they are valid.
            if (_xPos == NULL || _yPos == NULL || _dither == NULL || _invert == NULL || _pathType==NULL ||_path == NULL) return;

            if(_pathType == 0)
            {
                // Convert it on the fly from HEX ASCII to standard ASCII. This can be done directly in the buffer since ASCII will take up half the size of the original _path size.
                hexAsciiToAscii(_path, _argSize);
                
                // Add null-termination at the end of the argument.
                _path[_argSize / 2] = '\0';

                // Try to open an image from microSD card. Send reponse back if the opening was successful or not.
                if (_display->image.draw((const char*)_path, atol(_xPos), atol(_yPos), atol(_dither) & 1, atol(_invert)  &1))
                {
                    sendResponse(CMD_DRAW_IMAGE, strlen((char*)_okResponseString), (char*)_okResponseString);
                }
                else
                {
                    sendResponse(CMD_DRAW_IMAGE, strlen((char*)_failResponseString), (char*)_failResponseString);
                }

            }
            else
            {
                char _pathNew[_argSize+1];
                strncpy(_pathNew, _path, _argSize);
                _pathNew[_argSize]='\0';
                // Try to open an image from microSD card. Send reponse back if the opening was successful or not.
                if (_display->image.draw((const char*)_pathNew, atol(_xPos), atol(_yPos), atol(_dither) & 1, atol(_invert)  &1))
                {
                    sendResponse(CMD_DRAW_IMAGE, strlen((char*)_okResponseString), (char*)_okResponseString);
                }
                else
                {
                    sendResponse(CMD_DRAW_IMAGE, strlen((char*)_failResponseString), (char*)_failResponseString);
                }
            }


            break;

        }
        case CMD_DRAW_IMAGE_BUFFER:
        {
            // Check the number of the arguments and check it's payload repeatable.
            checkArguments(&_numberOfArgs, 5, 0);

            // Get the arguments and their values.
            char *_xPos;
            char *_yPos;
            char *_w;
            char *_h;
            char *_hexData;
            uint32_t _argSize;

            // Get the arguments ant it's size.
            _xPos = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            _yPos = getArgument(_payload, _payloadSize, _numberOfArgs, 1, NULL);
            _w = getArgument(_payload, _payloadSize, _numberOfArgs, 2, NULL);
            _h = getArgument(_payload, _payloadSize, _numberOfArgs, 3, NULL);
            _hexData = getArgument(_payload, _payloadSize, _numberOfArgs, 4, &_argSize);

            // Check if they are valid.
            if (_xPos == NULL || _yPos == NULL || _w == NULL || _h == NULL || _hexData == NULL) return;

            // Extract the data from the HEX on the fly.
            hexAsciiToAscii(_hexData, _argSize);

            // Display the image from the buffer.
            _display->image.draw((const uint8_t*)_hexData, atol(_xPos), atol(_yPos), atol(_w), atol(_h)); 
            break;
        }
        case CMD_SET_MODE:
        {
            break;
        }
        case CMD_GET_MODE:
        {
            break;
        }
        case CMD_CLEAR_DISPLAY:
        {
            // No payload check needed, just clear the display
            _display->clearDisplay();

            break;
        }
        case CMD_DISPLAY:
        {
            // Check the number of arguments.
            if (_numberOfArgs != 1) return;

            // Get the argument.
            char *_arg1;
            _arg1 = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            
            // Check if they are valid.
            if (_arg1 == NULL) return;

            // Execute the command!
            _display->display(atol(_arg1));
            break;
        }
                
        case CMD_READ_BATTERY:
        {
            Serial.println(_display->readBattery());
            char response[50];
            sprintf(response,"%lf", _display->readBattery());
            sendResponse(CMD_READ_BATTERY, strlen(response), response);
            break;
        }
        case CMD_EINK_PMIC:
        {

            break;
        }
        case CMD_RTC_SET_TIME:
        {
            // Check the number of arguments.
            if (_numberOfArgs != 3) return;

            // Get the argument.
            char *_arg1; // H
            char *_arg2; // M
            char *_arg3; // S


            _arg1 = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            _arg2 = getArgument(_payload, _payloadSize, _numberOfArgs, 1, NULL);
            _arg3 = getArgument(_payload, _payloadSize, _numberOfArgs, 2, NULL);

            // Check if they are valid.
            if (_arg1 == NULL || _arg2 == NULL || _arg3 == NULL) return;



            _display->rtc.setTime(atoi(_arg1), atoi(_arg2), atoi(_arg3));

            break;
        }
        case CMD_RTC_SET_DATE:
        {
            // Check the number of arguments.
            if (_numberOfArgs != 4) return;

            // Get the argument.
            char *_arg1; // Weekday
            char *_arg2; // Day
            char *_arg3; // Month
            char *_arg4; // Year


            _arg1 = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            _arg2 = getArgument(_payload, _payloadSize, _numberOfArgs, 1, NULL);
            _arg3 = getArgument(_payload, _payloadSize, _numberOfArgs, 2, NULL);
            _arg4 = getArgument(_payload, _payloadSize, _numberOfArgs, 3, NULL);

            // Check if they are valid.
            if (_arg1 == NULL || _arg2 == NULL || _arg3 == NULL || _arg4== NULL) return;


            _display->rtc.setDate(atoi(_arg1), atoi(_arg2), atoi(_arg3), atoi(_arg4));
            break;
        }
        case CMD_RTC_SET_EPOCH:
        {
            // Check the number of arguments.
            if (_numberOfArgs != 1) return;

            // Get the argument.
            char *_arg1; // Epoch


            _arg1 = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);

            // Check if they are valid.
            if (_arg1 == NULL) return;

            _display->rtc.setEpoch(atoll(_arg1));
            break;
        }
        case CMD_RTC_UPDATE:
        {
            _display->rtc.getRtcData();
            break;
        }
        case CMD_RTC_GET_DATA_ALL:
        {
            char response[100];
            sprintf(response,"%d:%d:%d %d %d/%d/%d", _display->rtc.getHour(),_display->rtc.getMinute(), _display->rtc.getSecond(), 
            _display->rtc.getWeekday(), _display->rtc.getDay(), _display->rtc.getMonth(), _display->rtc.getYear());
            sendResponse(CMD_RTC_GET_ALARM_ALL, strlen(response), response);
            break;
        }
        case CMD_RTC_SET_ALARM:
        {
            // Check the number of arguments.
            if (_numberOfArgs != 5) return;

            // Get the argument.
            char *_arg1; // sec
            char *_arg2; // min
            char *_arg3; // hour
            char *_arg4; // day
            char *_arg5; // weekday



            _arg1 = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            _arg2 = getArgument(_payload, _payloadSize, _numberOfArgs, 1, NULL);
            _arg3 = getArgument(_payload, _payloadSize, _numberOfArgs, 2, NULL);
            _arg4 = getArgument(_payload, _payloadSize, _numberOfArgs, 3, NULL);
            _arg5 = getArgument(_payload, _payloadSize, _numberOfArgs, 4, NULL);

            // Check if they are valid.
            if (_arg1 == NULL || _arg2 == NULL || _arg3 == NULL || _arg4== NULL || _arg5== NULL) return;

            _display->rtc.enableAlarm();

            _display->rtc.setAlarm(atoi(_arg1),atoi(_arg2),atoi(_arg3),atoi(_arg4),atoi(_arg5));

            break;
        }
        case CMD_RTC_SET_ALARM_EPOCH:
        {
            // Check the number of arguments.
            if (_numberOfArgs != 2) return;

            // Get the argument.
            char *_arg1; // epoch
            char *_arg2; // match

            _arg1 = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            _arg2 = getArgument(_payload, _payloadSize, _numberOfArgs, 1, NULL);

            // Check if they are valid.
            if (_arg1 == NULL || _arg2 == NULL) return;

            _display->rtc.setAlarmEpoch(atoll(_arg1),atoi(_arg2));

            break;
        }
        case CMD_RTC_CLEAR_AL_FLAG:
        {
            _display->rtc.clearAlarmFlag();
            break;
        }
        case CMD_RTC_GET_ALARM_ALL:
        {
            char response[100];
            sprintf(response,"%d:%d:%d %d-%d", _display->rtc.getAlarmHour(),_display->rtc.getAlarmMinute(), _display->rtc.getAlarmSecond(), _display->rtc.getAlarmDay(), _display->rtc.getAlarmWeekday());
            sendResponse(CMD_RTC_GET_ALARM_ALL, strlen(response), response);
            break;
        }
        case CMD_RTC_SET_TIMER:
        {
            // Check the number of arguments.
            if (_numberOfArgs != 4) return;

            // Get the argument.
            char *_arg1; // Source clock
            char *_arg2; // 
            char *_arg3; // Month
            char *_arg4; // Year


            _arg1 = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            _arg2 = getArgument(_payload, _payloadSize, _numberOfArgs, 1, NULL);
            _arg3 = getArgument(_payload, _payloadSize, _numberOfArgs, 2, NULL);
            _arg4 = getArgument(_payload, _payloadSize, _numberOfArgs, 3, NULL);

            // Check if they are valid.
            if (_arg1 == NULL || _arg2 == NULL || _arg3 == NULL || _arg4== NULL) return;

            RTC::rtcCountdownSrcClock clock;

            switch(atoi(_arg1))
            {
                case 0:
                    clock=RTC::TIMER_CLOCK_4096HZ;
                    break;
                case 1:
                    clock=RTC::TIMER_CLOCK_64HZ;
                    break;
                case 2:
                    clock=RTC::TIMER_CLOCK_1HZ;
                    break;
                case 3:
                    clock=RTC::TIMER_CLOCK_1PER60HZ;
                    break;
                default:
                    return;

            }

            _display->rtc.timerSet(clock, atoi(_arg2), atoi(_arg3), atoi(_arg4));

            break;
        }
        case CMD_RTC_GET_TIMER_FLAG:
        {
            char response[1];
            itoa(_display->rtc.checkTimerFlag(), response, 10);
            sendResponse(CMD_RTC_GET_TIMER_FLAG, strlen(response), response);

            break;
        }
        case CMD_RTC_CLEAR_TIMER_FLAG:
        {
            _display->rtc.clearTimerFlag();
            break;
        }
        case CMD_RTC_DISABLE_TIMER:
        {
            _display->rtc.disableTimer();
            break;
        }
        case CMD_RTC_IS_SET:
        {
            char response[1];
            itoa(_display->rtc.isSet(), response, 10);
            sendResponse(CMD_RTC_IS_SET, strlen(response), response);
            break;
        }
        case CMD_RTC_RESET:
        {
            _display->rtc.reset();
            break;
        }
        case CMD_ESP32_DEEPSLEEP:
        {
            // Check the number of arguments.
            if (_numberOfArgs < 2) return;

            // Get the argument.
            char *_arg1; 
            char *_arg2; 

            _arg1 = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            _arg2 = getArgument(_payload, _payloadSize, _numberOfArgs, 1, NULL);

            // Check if they are valid.
            if (_arg1 == NULL || _arg2 == NULL) return;

            if(atoi(_arg1) == 0)
            {
                esp_sleep_enable_timer_wakeup(atoll(_arg2));
            }
            else
            {
                char *_arg3;
                _arg3 = getArgument(_payload, _payloadSize, _numberOfArgs, 2, NULL);
                if(_arg3 == NULL) return;

                esp_sleep_enable_ext0_wakeup((gpio_num_t)atoi(_arg2), atoi(_arg3));
            }
            sendResponse(CMD_ESP32_DEEPSLEEP, strlen((char*)_okResponseString), (char*)_okResponseString);
            esp_deep_sleep_start();
            break;
        }
        case CMD_ESP32_LIGHTSLEEP:
        {

            // Check the number of arguments.
            if (_numberOfArgs < 2) return;

            // Get the argument.
            char *_arg1; 
            char *_arg2; 

            _arg1 = getArgument(_payload, _payloadSize, _numberOfArgs, 0, NULL);
            _arg2 = getArgument(_payload, _payloadSize, _numberOfArgs, 1, NULL);

            // Check if they are valid.
            if (_arg1 == NULL || _arg2 == NULL) return;

            if(atoi(_arg1) == 0)
            {
                esp_sleep_enable_timer_wakeup(atoll(_arg2));
            }
            else
            {
                char *_arg3;
                _arg3 = getArgument(_payload, _payloadSize, _numberOfArgs, 2, NULL);
                if(_arg3 == NULL) return;

                esp_sleep_enable_ext0_wakeup((gpio_num_t)atoi(_arg2), atoi(_arg3));
            }
            sendResponse(CMD_ESP32_DEEPSLEEP, strlen((char*)_okResponseString), (char*)_okResponseString);
            delay(10);
            esp_light_sleep_start();
            break;
        }
        case CMD_TOUCH_INIT:
        {
            // Not initialized as Inkplate 5V2 doesnt have a touchscreen
            break;
        }
        case CMD_TOUCH_AVAILABLE:
        {
            // Not initialized as Inkplate 5V2 doesnt have a touchscreen
            break;
        }
        case CMD_TOUCH_GET_DATA:
        {
            // Not initialized as Inkplate 5V2 doesnt have a touchscreen
            break;
        }
        case CMD_CONNECT_WIFI:
        {
            // Check the number of arguments.
            if (_numberOfArgs != 2) return;


            // Get the arguments.
            char *_ssid; 
            char *_password;

            uint32_t ssidSize, passwordSize;

            _ssid = getArgument(_payload, _payloadSize, _numberOfArgs, 0, &ssidSize);
            _password = getArgument(_payload, _payloadSize, _numberOfArgs, 1, &passwordSize);

            // Check if they are valid.
            if (_ssid == NULL || _password == NULL) return;

            char ssid[ssidSize + 1];
            char password[passwordSize + 1];

            strncpy(ssid, _ssid, ssidSize);
            ssid[ssidSize] = '\0';          

            strncpy(password, _password, passwordSize);
            password[passwordSize] = '\0'; 


            Serial.println(ssid);
            Serial.println(password);


            if(_display->connectWiFi(ssid, password))
            {
                sendResponse(CMD_CONNECT_WIFI, strlen((char*)_okResponseString), (char*)_okResponseString);
            }
            else
            {
                sendResponse(CMD_CONNECT_WIFI, strlen((char*)_failResponseString), (char*)_failResponseString);
            }

            break;
        }
        case CMD_DISCONNECT_WIFI:
        {
            _display->disconnect();
            break;
        }
        case CMD_GET_REQUEST:
        {
            // Check the number of arguments.
            if (_numberOfArgs != 1) return;


            // Get the arguments.
            char *_server; 

            uint32_t serverSize;

            _server = getArgument(_payload, _payloadSize, _numberOfArgs, 0, &serverSize);

            // Check if they are valid.
            if (_server == NULL ) return;

            char server[serverSize+1];

            strncpy(server,_server, serverSize);

            server[serverSize]='\0';

            if(_display->isConnected())
            {
                HTTPClient http;

                
                // Your Domain name with URL path or IP address with path
                http.begin(server);
                
                // Send HTTP GET request
                int httpResponseCode = http.GET();
                
                if (httpResponseCode>0) {
                    String payload = http.getString();
                    sendResponse(CMD_GET_REQUEST, strlen(payload.c_str()), (char*)payload.c_str());
                }
                else {
                    sendResponse(CMD_GET_REQUEST, strlen((char*)_failResponseString), (char*)_failResponseString);
                }
                // Free resources
                http.end();
            }
            else
            {
                sendResponse(CMD_GET_REQUEST, strlen((char*)_failResponseString), (char*)_failResponseString);
            }

            break;
        }

        case CMD_POST_REQUEST:
        {
            // Check the number of arguments.
            if (_numberOfArgs != 2) return;


            // Get the arguments.
            char *_server; 
            char *_postMessage;

            uint32_t serverSize;
            uint32_t postSize;

            _server = getArgument(_payload, _payloadSize, _numberOfArgs, 0, &serverSize);
            _postMessage = getArgument(_payload, _payloadSize, _numberOfArgs, 1, &postSize);

            // Check if they are valid.
            if (_server == NULL ) return;

            char server[serverSize+1];
            char postMessage[postSize+1];

            strncpy(server,_server, serverSize);
            strncpy(postMessage, _postMessage, postSize);

            server[serverSize]='\0';
            postMessage[postSize]='\0';

            if(_display->isConnected())
            {
                HTTPClient http;

                // Your Domain name with URL path or IP address with path
                http.begin(server);

                http.addHeader("Content-Type", "text/plain");

                int httpResponseCode = http.POST(postMessage);

                if (httpResponseCode>0) {
                    sendResponse(CMD_POST_REQUEST, strlen((char*)_okResponseString), (char*)_okResponseString);
                }
                else {
                    sendResponse(CMD_POST_REQUEST, strlen((char*)_failResponseString), (char*)_failResponseString);
                }
                // Free resources
                http.end();

            }
            break;
        } 
    }
}

int PeripheralMode::hexToChar(int a, int b)
{
    return ((16 * charToInt(a)) + charToInt(b));
}

int PeripheralMode::charToInt(char a)
{
    // Convert to uppercase
    a = toupper(a);
    
    if (a >= '0' && a <= '9')
      return a - '0';
    if (a >= 'A' && a <= 'F')
      return ((a - 'A') + 10);
    return -1;
}

void PeripheralMode::checkArguments(int *_noOfArgs, int _maxArg, int _repeat)
{
    // Check the number of the arguments.
    if ((*_noOfArgs) < _maxArg) return;

    // If the repeatable payload option is not enabled, restrict to only first argument.
    if (!_repeat) (*_noOfArgs) = _maxArg;
}

void PeripheralMode::hexAsciiToAscii(char *_buffer, uint32_t _size)
{
    // Extract the data from the HEX on the fly. This can be done since the decoded data is two times smaller then original ASCII HEX data.
    for (uint32_t i = 0; i < _size; i+=2)
    {
        _buffer[i / 2] = hexToChar(_buffer[i], _buffer[i + 1]);
    }
}