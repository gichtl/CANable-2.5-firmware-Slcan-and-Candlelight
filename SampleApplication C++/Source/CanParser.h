#ifndef CAN_PARSER_H
#define CAN_PARSER_H

#include <cstdint>
#include <cstdlib>

struct CanConfig
{
    uint32_t bitrate = 0;
    uint32_t dbitrate = 0;

    uint32_t sample_point = 875;      // Darstellung in Promille (87.5%)
    uint32_t dsample_point = 875;

    bool fd = false;
};

class CanParser
{
public:
    CanParser() = default;

    bool parse(int argc, char* argv[]);

    const CanConfig& getCanConfig() const;

private:
    bool parseBitrate(const char* str, uint32_t& value);
    bool parseSamplePoint(const char* str, uint32_t& value);

    CanConfig m_CanConfig;
};

#endif