#include "colours.h"

#include <cmath>
#include <stdexcept>

namespace Colours {
    std::string to_string(const ColourConvertError& value) {

    }

    RGB8::RGB8(const uint8_t& r, const uint8_t& g, const uint8_t& b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    RGBA8::RGBA8(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    RGB::RGB(const float& r, const float& g, const float& b) {
        if (r > 1 || r < 0 || g > 1 || g < 0 || b > 1 || b < 0) {
            throw std::runtime_error("r is wrong size");
        }
        this->r = r;
        this->g = g;
        this->b = b;
    }

    RGBA::RGBA(const float& r, const float& g, const float& b, const float& a) {
        if (r > 1 || r < 0 || g > 1 || g < 0 || b > 1 || b < 0 || a > 1 || a < 0) {
            throw std::runtime_error("r is wrong size");
        }
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    HSL::HSL(const float& h, const float& s, const float& l) {
        if (h < 0) {
            throw std::runtime_error("");
        }
        this->h = std::fmod(h, 360.0);
        if (l > 100 || 0 > l) {
            throw std::out_of_range("l is to large");
        }
        if (s > 100 || 0 > s) {
            throw std::out_of_range("s is too small");
        }
        this->l = l;
        this->s = s;
    }

    HSLA::HSLA(const float& h, const float& s, const float& l, const float& a) {
        if (h < 0) {
            throw std::runtime_error("");
        }
        this->h = std::fmod(h, 360.0);
        if (l > 100 || 0 > l) {
            throw std::out_of_range("l is to large");
        }
        if (s > 100 || 0 > s) {
            throw std::out_of_range("s is too large");
        }
        if (a > 1.0 || 0 > a) {
            throw std::out_of_range("a is too large");
        }
        this->l = l;
        this->s = s;
        this->a = a;
    }

    HWB::HWB(const float& h, const float& w, const float& b) {
        if (h < 0) {
            throw std::runtime_error("");
        }
        this->h = std::fmod(h, 360.0);
        if (w > 100 || 0 > w) {
            throw std::out_of_range("l is to large");
        }
        if (b > 100 || 0 > b) {
            throw std::out_of_range("s is too small");
        }
        if (w + b >= 100) {
            throw std::runtime_error("w and b together is too large");
        }
        this->b = b;
        this->w = w;
    }

    LAB::LAB(const float& l, const float& a, const float& b) {
        if (l > 100 || 0 > l) {
            throw std::runtime_error("l is wrong size");
        }
        this->l = l;
        if (b > 127 || -128 > b) {
            throw std::runtime_error("b is wrong size");
        }
        this->b = b;
        if (a > 127 || -128 > a) {
            throw std::runtime_error("a is wrong size");
        }
        this->a = a;
    }

    OKLAB::OKLAB(const float& l, const float& a, const float& b) {
        if (l > 1 || 0 > l) {
            throw std::runtime_error("l is wrong size");
        }
        this->l = l;
        if (b > 0.4 || -0.4 > b) {
            throw std::runtime_error("b is wrong size");
        }
        this->b = b;
        if (a > 0.4 || -0.4 > a) {
            throw std::runtime_error("a is wrong size");
        }
        this->a = a;
    }

    LCH::LCH(const float& l, const float& c, const float& h) {
        if (l > 100 || 0 > l) {
            throw std::runtime_error("l is wrong size");
        }
        this->l = l;
        if (0 > c) {
            throw std::runtime_error("b is wrong size");
        }
        this->c = c;
        if (h > 360 || 0 > h) {
            throw std::runtime_error("a is wrong size");
        }
        this->h = h;
    }

    OKLCH::OKLCH(const float& l, const float& c, const float& h) {
        if (l > 1 || 0 > l) {
            throw std::runtime_error("l is wrong size");
        }
        this->l = l;
        if (0 > c) {
            throw std::runtime_error("b is wrong size");
        }
        this->c = c;
        if (h > 360 || 0 > h) {
            throw std::runtime_error("a is wrong size");
        }
        this->h = h;
    }
}
