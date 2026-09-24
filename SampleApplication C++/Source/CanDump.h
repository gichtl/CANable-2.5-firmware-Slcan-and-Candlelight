#pragma once

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include "Candlelight/Candlelight.h"

class CanDump
{
public:
    explicit CanDump(const std::string& interfaceName = "CANable")
        : m_interface(interfaceName)
    {
    }

    void print(const CANable::kCanPacket& packet, int64_t s64_RxTimestamp)
    {
        // ------------------------------------------------------------
        // Timestamp
        // s64_RxTimestamp is in microseconds since Unix epoch.
        //
        // Example: 1709057767916009
        // becomes: (1709057767.916009)
        // ------------------------------------------------------------
        const int64_t seconds = s64_RxTimestamp / 1000000;
        const int64_t microseconds = s64_RxTimestamp % 1000000;

        std::cout
            << '('
            << seconds
            << '.'
            << std::setfill('0')
            << std::setw(6)
            << microseconds
            << ") ";

        // ------------------------------------------------------------
        // Interface
        // ------------------------------------------------------------
        std::cout
            << m_interface
            << ' ';

        // ------------------------------------------------------------
        // CAN ID
        //
        // Standard ID : 3 hex digits
        // Extended ID : 8 hex digits
        // ------------------------------------------------------------
        std::cout
            << std::uppercase
            << std::hex
            << std::setfill('0');

        if (packet.mb_29bit)
        {
            std::cout
                << std::setw(8)
                << (packet.mu32_ID & 0x1FFFFFFFU);
        }
        else
        {
            std::cout
                << std::setw(3)
                << (packet.mu32_ID & 0x7FFU);
        }

        // ------------------------------------------------------------
        // CAN FD
        //
        // Format:
        //     <CAN-ID>##<flags><data>
        //
        // CANFD_BRS = 0x01
        // CANFD_ESI = 0x02
        // CANFD_FDF = 0x04
        //
        // Examples:
        //     FDF          -> ##4
        //     FDF + BRS    -> ##5
        //     FDF + ESI    -> ##6
        //     FDF + BRS
        //          + ESI   -> ##7
        // ------------------------------------------------------------
        if (packet.mb_FDF)
        {
            uint8_t flags = 0x04; // CANFD_FDF

            if (packet.mb_BRS)
                flags |= 0x01;    // CANFD_BRS

            if (packet.mb_ESI)
                flags |= 0x02;    // CANFD_ESI

            std::cout
                << "##"
                << static_cast<unsigned>(flags);
        }
        else
        {
            // --------------------------------------------------------
            // Classical CAN
            //
            // Normal:
            //     123#112233
            //
            // RTR:
            //     123#R
            // --------------------------------------------------------
            std::cout << '#';

            if (packet.mb_RTR)
            {
                std::cout << 'R';
            }
        }


        // ------------------------------------------------------------
        // Data
        //
        // Classical CAN: max. 8 bytes
        // CAN FD:        max. 64 bytes
        //
        // RTR frames have no data.
        // ------------------------------------------------------------
        if (!packet.mb_RTR)
        {
            const uint8_t maxDataLen =
                packet.mb_FDF ? 64 : 8;

            const uint8_t dataLen =
                (packet.mu8_DataLen > maxDataLen)
                    ? maxDataLen
                    : packet.mu8_DataLen;

            for (uint8_t i = 0; i < dataLen; ++i)
            {
                std::cout
                    << std::setw(2)
                    << static_cast<unsigned>(
                           packet.mu8_Data[i]);
            }
        }

        // ------------------------------------------------------------
        // Reset stream formatting
        // ------------------------------------------------------------
        std::cout
            << std::dec
            << std::setfill(' ')
            << '\n';
    }


private:
    std::string m_interface;
};
