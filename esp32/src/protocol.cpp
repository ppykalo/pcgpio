
#include "protocol.h"

bool Protocol::tryProcessReceivedData(const uint8_t *data, int dataLength, Protocol::ResponseFunction responseFunction) const
{
    if (dataLength < 5)
        return false;

 // read packet length
    uint8_t packetLength = data[0];
    if (dataLength < packetLength)
        return false;

 // read peripheral
    uint8_t packetPeripheral = data[1];

 // read function
    uint8_t packetFunction = data[2];

 // read device/gpio
    uint8_t packetDevice = data[3];

    const uint8_t* packetArguments = data + 4;
    uint8_t packetArgumentsLength = packetLength - 5;
    
 // verify checksum
    uint8_t packetChecksum = data[packetLength-1];
    if (packetChecksum != 0x00)
        return false;

    uint16_t functionId = (uint16_t) packetPeripheral << 8 | packetFunction;
    auto functionIt = this->operations.find(functionId);
    if (functionIt == this->operations.end())
        return false;

    functionIt->second(OperationContext(packetDevice, packetArguments, packetArgumentsLength, responseFunction));

    return true;
}

void Protocol::addFunction(uint8_t peripheral, uint8_t operation, OperationFunction function)
{
    uint16_t functionId = (uint16_t) peripheral << 8 | operation;
    this->operations[functionId] = function;
}

Protocol::OperationContext::OperationContext(
    uint8_t device, const uint8_t* arguments, uint8_t argumentsLength, 
    Protocol::ResponseFunction responseFunction)
    : _device(device), _arguments(arguments), _argumentsLength(argumentsLength), _responseFunction(responseFunction)
{
}
