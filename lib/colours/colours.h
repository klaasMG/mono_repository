#pragma once
#include <cstdint>

namespace Colours {
    class RGBA8 {
        public
        :
        RGBA8() = default;
        RGBA8(const uint8_t & r, const uint8_t & g, const uint8_t & b, const uint8_t & a);
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 0;
    };

    class RGB8 {
    public:
        RGB8() = default;
        RGB8(const uint8_t& r, const uint8_t& g, const uint8_t& b);
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
    };

    class RGBA {
    public:
        RGBA() = default;
        RGBA(const float& r, const float& g, const float& b, const float& a);
        float r = 0;
        float g = 0;
        float b = 0;
        float a = 0;
    };

    class RGB {
    public:
        RGB() = default;
        RGB(const float& r, const float& g, const float& b);
        float r = 0;
        float g = 0;
        float b = 0;
    };

    class HSL {
    public:
        HSL() = default;
        HSL(const float& h, const float& s, const float& l);
        float h = 0;
        float s = 0;
        float l = 0;
    };

    class HSLA {
    public:
        HSLA() = default;
        HSLA(const float& h, const float& s, const float& l, const float& a);
        float h = 0;
        float s = 0;
        float l = 0;
        float a = 0;
    };

    class HWB {
    public:
        HWB() = default;
        HWB(const float& h, const float& w, const float& b);
        float h = 0;
        float w = 0;
        float b = 0;
    };

    class LAB {
    public:
        LAB() = default;
        LAB(const float& l, const float& a, const float& b);
        float l = 0;
        float a = 0;
        float b = 0;
    };

    class OKLAB {
    public:
        OKLAB() = default;
        OKLAB(const float& l, const float& a, const float& b);
        float l = 0;
        float a = 0;
        float b = 0;
    };

    class LCH {
    public:
        LCH() = default;
        LCH(const float& l, const float& c, const float& h);
        float l = 0;
        float c = 0;
        float h = 0;
    };

    class OKLCH {
    public:
        OKLCH() = default;
        OKLCH(const float& l, const float& c, const float& h);
        float l = 0;
        float c = 0;
        float h = 0;
    };
}
