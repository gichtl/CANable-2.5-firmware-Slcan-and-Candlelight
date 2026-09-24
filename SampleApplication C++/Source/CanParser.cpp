#include <cstdlib>
#include <cstring>

#include "CanParser.h"


bool CanParser::parseBitrate(const char* str, uint32_t& value)
{
    char* end = nullptr;

    unsigned long v = std::strtoul(str, &end, 10);

    if (end == str)
        return false;

    if (*end == 'k' || *end == 'K')
    {
        v *= 1000;
    }
    else if (*end == 'M' || *end == 'm')
    {
        v *= 1000000;
    }
    else if (*end == 'G' || *end == 'g')
    {
        v *= 1000000000UL;
    }
    else if (*end != '\0')
    {
        return false;
    }

    value = static_cast<uint32_t>(v);

    return true;
}


bool CanParser::parseSamplePoint(const char* str, uint32_t& value)
{
    char* end = nullptr;

    double v = std::strtod(str, &end);

    if (end == str || *end != '\0')
        return false;

    if (v <= 0.0 || v >= 1.0)
        return false;

    value = v * 1000.0;

    return true;
}


bool CanParser::parse(int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i)
    {
        // -------------------------
        // ABR bitrate
        // -------------------------
        if (std::strcmp(argv[i], "bitrate") == 0)
        {
            if (++i >= argc)
                return false;

            if (!parseBitrate(argv[i], m_CanConfig.bitrate))
                return false;
        }

        // -------------------------
        // DBR bitrate
        // -------------------------
        else if (std::strcmp(argv[i], "dbitrate") == 0)
        {
            if (++i >= argc)
                return false;

            if (!parseBitrate(argv[i], m_CanConfig.dbitrate))
                return false;
        }

        // -------------------------
        // ABR sample point
        // -------------------------
        else if (std::strcmp(argv[i], "sample-point") == 0)
        {
            if (++i >= argc)
                return false;

            if (!parseSamplePoint(argv[i], m_CanConfig.sample_point))
                return false;
        }

        // -------------------------
        // DBR sample point
        // -------------------------
        else if (std::strcmp(argv[i], "dsample-point") == 0)
        {
            if (++i >= argc)
                return false;

            if (!parseSamplePoint(argv[i], m_CanConfig.dsample_point))
                return false;
        }

        // -------------------------
        // FD
        // -------------------------
        else if (std::strcmp(argv[i], "fd") == 0)
        {
            if (++i >= argc)
                return false;

            if (std::strcmp(argv[i], "on") == 0)
            {
                //m_ABRConfig.fd = true;
                m_CanConfig.fd = true;
            }
            else if (std::strcmp(argv[i], "off") == 0)
            {
                //m_ABRConfig.fd = false;
                m_CanConfig.fd = false;
            }
            else
            {
                return false;
            }
        }

        // -------------------------
        // unbekannter Parameter
        // -------------------------
        else
        {
            return false;
        }
    }

    return true;
}


const CanConfig& CanParser::getCanConfig() const
{
    return m_CanConfig;
}
