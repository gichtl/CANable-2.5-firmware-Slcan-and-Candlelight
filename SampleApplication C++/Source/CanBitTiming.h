#ifndef CAN_BIT_TIMING_H
#define CAN_BIT_TIMING_H

#include <cstdint>

class CanBitTiming
{
public:
    struct Result
    {
        uint32_t brp;
        uint32_t seg1;
        uint32_t seg2;
        uint32_t actualBaud;
        uint32_t actualSP;   // in Promille, z.B. 875 = 87,5 %
    };

    CanBitTiming(uint32_t canfd = 0, uint32_t fclk = 0, uint32_t baud = 0, uint32_t sp = 0)
        : m_fd(canfd), m_fclk(fclk), m_baud(baud), m_sp(sp) {}

    void set(uint32_t canfd, uint32_t fclk, uint32_t baud, uint32_t sp)
    {
        m_fd = canfd; m_fclk = fclk; m_baud = baud; m_sp = sp;
    }

    bool calculate(Result& result)
    {
        if (m_baud == 0)
            return false;

        uint64_t bestBaudError = UINT64_MAX;
        uint32_t bestSPError = UINT32_MAX;
        bool found = false;

        for (uint32_t brp = BRP_MIN[m_fd]; brp <= BRP_MAX[m_fd]; ++brp)
        {
            const uint64_t denominator = uint64_t(brp) * m_baud;
            const uint32_t ntq = (uint64_t(m_fclk) + denominator / 2) / denominator;
            if (ntq < 10)
                continue;

            for (uint32_t seg2 = SEG2_MIN[m_fd]; seg2 <= SEG2_MAX[m_fd]; ++seg2)
            {
                if (seg2 >= ntq - 1)
                    continue;

                const uint32_t seg1 = ntq - 1 - seg2;
                if (seg1 < SEG1_MIN[m_fd] || seg1 > SEG1_MAX[m_fd])
                    continue;

                const uint64_t divisor = uint64_t(brp) * ntq;
                const uint64_t targetClock = uint64_t(m_baud) * divisor;
                const uint64_t baudError = (m_fclk > targetClock) ? m_fclk - targetClock : targetClock - m_fclk;

                const uint32_t actualBaud = m_fclk / divisor;
                const uint32_t actualSP = ((1 + seg1) * 1000 + ntq / 2) / ntq;
                const uint32_t spError = (actualSP > m_sp) ? actualSP - m_sp : m_sp - actualSP;

                if (!found ||
                    baudError < bestBaudError ||
                    (baudError == bestBaudError && spError < bestSPError))
                {
                    found = true;
                    bestBaudError = baudError;
                    bestSPError = spError;

                    result.brp = brp;
                    result.seg1 = seg1;
                    result.seg2 = seg2;
                    result.actualBaud = actualBaud;
                    result.actualSP = actualSP;
                }
            }
        }
        return found;
    }

private:
    uint32_t m_fd;
    uint32_t m_fclk;
    uint32_t m_baud;
    uint32_t m_sp;

    /*
     * Diese Grenzen müssen ggf. an den
     * verwendeten CAN-Controller angepasst werden.
     */
    static constexpr uint32_t BRP_MIN[2]  = {  1,  1};
    static constexpr uint32_t BRP_MAX[2]  = {512, 32};
    static constexpr uint32_t SEG1_MIN[2] = {  1,  1};
    static constexpr uint32_t SEG1_MAX[2] = {256, 32};
    static constexpr uint32_t SEG2_MIN[2] = {  1,  1};
    static constexpr uint32_t SEG2_MAX[2] = {128, 16};
};

#endif // CAN_BIT_TIMING_H