
#include <array>
#include <unordered_map>
#include <functional>

class Protocol
{
  public:
    typedef std::function<void(uint8_t*, uint8_t)> ResponseFunction;

    class OperationContext {
      public:
        OperationContext(uint8_t device, const uint8_t* arguments, uint8_t argumentsLength, ResponseFunction responseFunction);
        void writeResponse(uint8_t* response, uint8_t responseLength) const { _responseFunction(response, responseLength); }

        uint8_t device() const { return _device; }
        const uint8_t* arguments() const { return _arguments; }
        uint8_t argument(uint8_t i) const { return (i < _argumentsLength) ? _arguments[i] : 0; }
        uint8_t argumentsLength() const { return _argumentsLength; }

      private:
        uint8_t _device;
        const uint8_t* _arguments;
        uint8_t _argumentsLength;
        ResponseFunction _responseFunction;
    };

    typedef std::function<void(const OperationContext&)> OperationFunction;

    bool tryProcessReceivedData(const uint8_t *data, int dataLength, ResponseFunction responseFunction) const;
    void addFunction(uint8_t peripheral, uint8_t operation, OperationFunction function);

  private:
    std::unordered_map<uint16_t, OperationFunction> operations; 
};
