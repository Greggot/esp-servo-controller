#pragma once
#include <cstdint>

namespace SkyBlue
{
    enum command_t : uint8_t
    {
        report,
        read,
        write,

        amount
    };
    inline const char* commandstring(command_t cmd) {
        static const char* descr[] = {"report", "read", "write"};
        return descr[cmd];
    }
    
    enum class type_t : uint16_t
    {
        none,
        steady,

        rotorservo,
        linearservo,
        camera
    };
    const char* typeToString(type_t type){
        static const char* descr[] = {"none", "steady", "rotor", "linear", "camera"};
        return descr[static_cast<int>(type)];
    }

    union ID
    {
        uint32_t raw;
        struct {
            uint16_t number;
            type_t type;
        };

        ID(uint16_t number, type_t type) : number(number), type(type){}
        ID(uint32_t raw = 0) : raw(raw) {}
    };
    bool operator< (const ID& r, const ID& l) { return r.raw < l.raw; }

    #define BUFFER_SIZE 1440
    constexpr size_t BUFFER_HEADER_SiZE() { return sizeof(ID) + sizeof(command_t) + sizeof(size_t); }
    constexpr size_t BUFFER_ARG_SIZE() {
        return BUFFER_SIZE - BUFFER_HEADER_SiZE(); 
    }

    #pragma pack(push, 1)
    struct buffer
    {
        ID id;
        command_t command;
        size_t length;
        uint8_t data[BUFFER_ARG_SIZE()];
    };
    #pragma pack(pop)
}