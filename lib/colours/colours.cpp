#include "colours.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <variant>

namespace Colours {

std::string to_string(const ColourConvertError& value) {
    switch (value) {
        case ColourConvertError::OK: return "OK";
        case ColourConvertError::INVALID_SOURCE_TYPE: return "INVALID_SOURCE_TYPE";
        case ColourConvertError::INVALID_TARGET_ENUM: return "INVALID_TARGET_ENUM";
        case ColourConvertError::UNSUPPORTED_CONVERSION: return "UNSUPPORTED_CONVERSION";
        case ColourConvertError::OUT_OF_GAMUT: return "OUT_OF_GAMUT";
        case ColourConvertError::INVALID_COLOR_VALUES: return "INVALID_COLOR_VALUES";
        case ColourConvertError::DIVISION_BY_ZERO: return "DIVISION_BY_ZERO";
        case ColourConvertError::ALPHA_NOT_SUPPORTED: return "ALPHA_NOT_SUPPORTED";
    }
    return "UNKNOWN";
}

static constexpr double SRGB_TO_LINEAR_THRESHOLD = 0.04045;
static constexpr double SRGB_GAMMA = 2.4;
static constexpr double LINEAR_TO_SRGB_THRESHOLD = 0.0031308;

static inline double srgb_to_linear(double c) {
    if (c <= SRGB_TO_LINEAR_THRESHOLD) {
        return c / 12.92;
    }
    return std::pow((c + 0.055) / 1.055, SRGB_GAMMA);
}

static inline double linear_to_srgb(double c) {
    if (c <= LINEAR_TO_SRGB_THRESHOLD) {
        return 12.92 * c;
    }
    return 1.055 * std::pow(c, 1.0 / SRGB_GAMMA) - 0.055;
}

static inline double clamp01(double v) {
    return v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v);
}

static inline uint8_t quantize_u8(double v) {
    double clamped = clamp01(v);
    return static_cast<uint8_t>(std::round(clamped * 255.0));
}

static LinearRGB rgb_to_linear_srgb(double r, double g, double b, double a) {
    return LinearRGB{
        srgb_to_linear(r),
        srgb_to_linear(g),
        srgb_to_linear(b),
        a
    };
}

static LinearRGB xyz_to_linear_srgb(double x, double y, double z, double a) {
    double r =  3.2406 * x - 1.5372 * y - 0.4986 * z;
    double g = -0.9689 * x + 1.8758 * y + 0.0415 * z;
    double b =  0.0557 * x - 0.2040 * y + 1.0570 * z;
    return LinearRGB{ r, g, b, a };
}

static LinearRGB linear_srgb_to_xyz(const LinearRGB& lin) {
    double x = 0.4124 * lin.r + 0.3576 * lin.g + 0.1805 * lin.b;
    double y = 0.2126 * lin.r + 0.7152 * lin.g + 0.0722 * lin.b;
    double z = 0.0193 * lin.r + 0.1192 * lin.g + 0.9505 * lin.b;
    return LinearRGB{ x, y, z, lin.a };
}

static LinearRGB lab_to_linear_srgb(double L, double a, double b, double alpha) {
    constexpr double Xn = 0.95047;
    constexpr double Yn = 1.0;
    constexpr double Zn = 1.08883;
    constexpr double eps = 216.0 / 24389.0;
    constexpr double kappa = 24389.0 / 27.0;

    double fy = (L + 16.0) / 116.0;
    double fx = a / 500.0 + fy;
    double fz = fy - b / 200.0;

    double xr, yr, zr;
    if (fy * fy * fy > eps) yr = fy * fy * fy;
    else yr = (116.0 * fy - 16.0) / kappa;
    if (fx * fx * fx > eps) xr = fx * fx * fx;
    else xr = (116.0 * fx - 16.0) / kappa;
    if (fz * fz * fz > eps) zr = fz * fz * fz;
    else zr = (116.0 * fz - 16.0) / kappa;

    double x = xr * Xn;
    double y = yr * Yn;
    double z = zr * Zn;

    return xyz_to_linear_srgb(x, y, z, alpha);
}

static LinearRGB oklab_to_linear_srgb(double L, double a, double b_, double alpha) {
    double l_ = L + 0.3963377774 * a + 0.2158037573 * b_;
    double m_ = L - 0.1055613458 * a - 0.0638541728 * b_;
    double s_ = L - 0.0894841775 * a - 1.2914855480 * b_;

    double l = l_ * l_ * l_;
    double m = m_ * m_ * m_;
    double s = s_ * s_ * s_;

    double x = +4.0767416621 * l - 3.3077115913 * m + 0.2309699292 * s;
    double y = -1.2684380046 * l + 2.6097574011 * m - 0.3413193965 * s;
    double z = -0.0041960863 * l - 0.7034186147 * m + 1.7076147010 * s;

    return xyz_to_linear_srgb(x, y, z, alpha);
}

static LinearRGB lch_to_lab(double L, double C, double H) {
    double h_rad = H * M_PI / 180.0;
    double a = C * std::cos(h_rad);
    double b = C * std::sin(h_rad);
    return LinearRGB{ L, a, b, 1.0 };
}

static LinearRGB oklch_to_oklab(double L, double C, double H) {
    double h_rad = H * M_PI / 180.0;
    double a = C * std::cos(h_rad);
    double b = C * std::sin(h_rad);
    return LinearRGB{ L, a, b, 1.0 };
}

static LinearRGB hsl_to_linear_srgb(double h, double s, double l, double alpha) {
    h = std::fmod(h, 360.0);
    if (h < 0) h += 360.0;
    s = clamp01(s / 100.0);
    l = clamp01(l / 100.0);

    double c = (1.0 - std::abs(2.0 * l - 1.0)) * s;
    double x = c * (1.0 - std::abs(std::fmod(h / 60.0, 2.0) - 1.0));
    double m = l - c / 2.0;

    double r, g, b;
    if (h < 60.0) { r = c; g = x; b = 0; }
    else if (h < 120.0) { r = x; g = c; b = 0; }
    else if (h < 180.0) { r = 0; g = c; b = x; }
    else if (h < 240.0) { r = 0; g = x; b = c; }
    else if (h < 300.0) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }

    return rgb_to_linear_srgb(r + m, g + m, b + m, alpha);
}

static LinearRGB hwb_to_linear_srgb(double h, double w, double b, double alpha) {
    h = std::fmod(h, 360.0);
    if (h < 0) h += 360.0;
    w = clamp01(w / 100.0);
    b = clamp01(b / 100.0);

    if (w + b >= 1.0) {
        double sum = w + b;
        w /= sum;
        b /= sum;
    }

    double c = 1.0 - w - b;
    double x = c * (1.0 - std::abs(std::fmod(h / 60.0, 2.0) - 1.0));

    double r, g, b_;
    if (h < 60.0) { r = c; g = x; b_ = 0; }
    else if (h < 120.0) { r = x; g = c; b_ = 0; }
    else if (h < 180.0) { r = 0; g = c; b_ = x; }
    else if (h < 240.0) { r = 0; g = x; b_ = c; }
    else if (h < 300.0) { r = x; g = 0; b_ = c; }
    else { r = c; g = 0; b_ = x; }

    return rgb_to_linear_srgb(r + w, g + w, b_ + w, alpha);
}

LinearRGB to_linear_srgb(const RGB& c) {
    return rgb_to_linear_srgb(c.r, c.g, c.b, 1.0);
}

LinearRGB to_linear_srgb(const RGBA& c) {
    return rgb_to_linear_srgb(c.r, c.g, c.b, c.a);
}

LinearRGB to_linear_srgb(const RGB8& c) {
    return rgb_to_linear_srgb(c.r / 255.0, c.g / 255.0, c.b / 255.0, 1.0);
}

LinearRGB to_linear_srgb(const RGBA8& c) {
    return rgb_to_linear_srgb(c.r / 255.0, c.g / 255.0, c.b / 255.0, c.a / 255.0);
}

LinearRGB to_linear_srgb(const HSL& c) {
    return hsl_to_linear_srgb(c.h, c.s, c.l, 1.0);
}

LinearRGB to_linear_srgb(const HSLA& c) {
    return hsl_to_linear_srgb(c.h, c.s, c.l, c.a);
}

LinearRGB to_linear_srgb(const HWB& c) {
    return hwb_to_linear_srgb(c.h, c.w, c.b, 1.0);
}

LinearRGB to_linear_srgb(const LAB& c) {
    return lab_to_linear_srgb(c.l, c.a, c.b, 1.0);
}

LinearRGB to_linear_srgb(const OKLAB& c) {
    return oklab_to_linear_srgb(c.l, c.a, c.b, 1.0);
}

LinearRGB to_linear_srgb(const LCH& c) {
    LinearRGB lab = lch_to_lab(c.l, c.c, c.h);
    return lab_to_linear_srgb(lab.r, lab.g, lab.b, 1.0);
}

LinearRGB to_linear_srgb(const OKLCH& c) {
    LinearRGB oklab = oklch_to_oklab(c.l, c.c, c.h);
    return oklab_to_linear_srgb(oklab.r, oklab.g, oklab.b, 1.0);
}

template <>
RGB from_linear_srgb<RGB>(const LinearRGB& lin) {
    return RGB{
        static_cast<float>(linear_to_srgb(lin.r)),
        static_cast<float>(linear_to_srgb(lin.g)),
        static_cast<float>(linear_to_srgb(lin.b))
    };
}

template <>
RGBA from_linear_srgb<RGBA>(const LinearRGB& lin) {
    return RGBA{
        static_cast<float>(linear_to_srgb(lin.r)),
        static_cast<float>(linear_to_srgb(lin.g)),
        static_cast<float>(linear_to_srgb(lin.b)),
        static_cast<float>(lin.a)
    };
}

template <>
RGB8 from_linear_srgb<RGB8>(const LinearRGB& lin) {
    return RGB8{
        quantize_u8(linear_to_srgb(lin.r)),
        quantize_u8(linear_to_srgb(lin.g)),
        quantize_u8(linear_to_srgb(lin.b))
    };
}

template <>
RGBA8 from_linear_srgb<RGBA8>(const LinearRGB& lin) {
    return RGBA8{
        quantize_u8(linear_to_srgb(lin.r)),
        quantize_u8(linear_to_srgb(lin.g)),
        quantize_u8(linear_to_srgb(lin.b)),
        quantize_u8(lin.a)
    };
}

static LinearRGB linear_srgb_to_hsl(const LinearRGB& lin) {
    double r = clamp01(linear_to_srgb(lin.r));
    double g = clamp01(linear_to_srgb(lin.g));
    double b = clamp01(linear_to_srgb(lin.b));

    double maxc = std::max(r, std::max(g, b));
    double minc = std::min(r, std::min(g, b));
    double l = (maxc + minc) / 2.0;

    double h = 0.0, s = 0.0;
    if (maxc != minc) {
        double d = maxc - minc;
        s = l > 0.5 ? d / (2.0 - maxc - minc) : d / (maxc + minc);
        if (maxc == r) h = (g - b) / d + (g < b ? 6.0 : 0.0);
        else if (maxc == g) h = (b - r) / d + 2.0;
        else h = (r - g) / d + 4.0;
        h *= 60.0;
    }
    return LinearRGB{ h, s * 100.0, l * 100.0, lin.a };
}

template <>
HSL from_linear_srgb<HSL>(const LinearRGB& lin) {
    LinearRGB hsl = linear_srgb_to_hsl(lin);
    return HSL{
        static_cast<float>(hsl.r),
        static_cast<float>(hsl.g),
        static_cast<float>(hsl.b)
    };
}

template <>
HSLA from_linear_srgb<HSLA>(const LinearRGB& lin) {
    LinearRGB hsl = linear_srgb_to_hsl(lin);
    return HSLA{
        static_cast<float>(hsl.r),
        static_cast<float>(hsl.g),
        static_cast<float>(hsl.b),
        static_cast<float>(lin.a)
    };
}

static LinearRGB linear_srgb_to_hwb(const LinearRGB& lin) {
    double r = clamp01(linear_to_srgb(lin.r));
    double g = clamp01(linear_to_srgb(lin.g));
    double b = clamp01(linear_to_srgb(lin.b));

    double maxc = std::max(r, std::max(g, b));
    double minc = std::min(r, std::min(g, b));

    double h = 0.0;
    if (maxc != minc) {
        double d = maxc - minc;
        if (maxc == r) h = (g - b) / d + (g < b ? 6.0 : 0.0);
        else if (maxc == g) h = (b - r) / d + 2.0;
        else h = (r - g) / d + 4.0;
        h *= 60.0;
    }
    double w = minc;
    double b_ = 1.0 - maxc;

    return LinearRGB{ h, w * 100.0, b_ * 100.0, lin.a };
}

template <>
HWB from_linear_srgb<HWB>(const LinearRGB& lin) {
    LinearRGB hwb = linear_srgb_to_hwb(lin);
    return HWB{
        static_cast<float>(hwb.r),
        static_cast<float>(hwb.g),
        static_cast<float>(hwb.b)
    };
}

static LinearRGB linear_srgb_to_lab(const LinearRGB& lin) {
    LinearRGB xyz = linear_srgb_to_xyz(lin);
    constexpr double Xn = 0.95047;
    constexpr double Yn = 1.0;
    constexpr double Zn = 1.08883;
    constexpr double eps = 216.0 / 24389.0;
    constexpr double kappa = 24389.0 / 27.0;

    double xr = xyz.r / Xn;
    double yr = xyz.g / Yn;
    double zr = xyz.b / Zn;

    double fx, fy, fz;
    if (xr > eps) fx = std::cbrt(xr);
    else fx = (kappa * xr + 16.0) / 116.0;
    if (yr > eps) fy = std::cbrt(yr);
    else fy = (kappa * yr + 16.0) / 116.0;
    if (zr > eps) fz = std::cbrt(zr);
    else fz = (kappa * zr + 16.0) / 116.0;

    double L = 116.0 * fy - 16.0;
    double a = 500.0 * (fx - fy);
    double b = 200.0 * (fy - fz);

    return LinearRGB{ L, a, b, lin.a };
}

template <>
LAB from_linear_srgb<LAB>(const LinearRGB& lin) {
    LinearRGB lab = linear_srgb_to_lab(lin);
    return LAB{
        static_cast<float>(lab.r),
        static_cast<float>(lab.g),
        static_cast<float>(lab.b)
    };
}

static LinearRGB linear_srgb_to_oklab(const LinearRGB& lin) {
    LinearRGB xyz = linear_srgb_to_xyz(lin);

    double l_ =  0.4122214708 * xyz.r + 0.5363325363 * xyz.g + 0.0514459929 * xyz.b;
    double m_ =  0.2119034982 * xyz.r + 0.6806995451 * xyz.g + 0.1073969566 * xyz.b;
    double s_ =  0.0883024619 * xyz.r + 0.2817188376 * xyz.g + 0.6299787005 * xyz.b;

    double l = std::cbrt(l_);
    double m = std::cbrt(m_);
    double s = std::cbrt(s_);

    double L =  0.2104542553 * l + 0.7936177850 * m - 0.0040720468 * s;
    double a =  1.9779984951 * l - 2.4285922050 * m + 0.4505937099 * s;
    double b =  0.0259040371 * l + 0.7827717662 * m - 0.8086757660 * s;

    return LinearRGB{ L, a, b, lin.a };
}

template <>
OKLAB from_linear_srgb<OKLAB>(const LinearRGB& lin) {
    LinearRGB oklab = linear_srgb_to_oklab(lin);
    return OKLAB{
        static_cast<float>(oklab.r),
        static_cast<float>(oklab.g),
        static_cast<float>(oklab.b)
    };
}

static LinearRGB lab_to_lch(double L, double a, double b) {
    double C = std::sqrt(a * a + b * b);
    double H = std::atan2(b, a) * 180.0 / M_PI;
    if (H < 0) H += 360.0;
    return LinearRGB{ L, C, H, 1.0 };
}

template <>
LCH from_linear_srgb<LCH>(const LinearRGB& lin) {
    LinearRGB lab = linear_srgb_to_lab(lin);
    LinearRGB lch = lab_to_lch(lab.r, lab.g, lab.b);
    return LCH{
        static_cast<float>(lch.r),
        static_cast<float>(lch.g),
        static_cast<float>(lch.b)
    };
}

static LinearRGB oklab_to_oklch(double L, double a, double b) {
    double C = std::sqrt(a * a + b * b);
    double H = std::atan2(b, a) * 180.0 / M_PI;
    if (H < 0) H += 360.0;
    return LinearRGB{ L, C, H, 1.0 };
}

template <>
OKLCH from_linear_srgb<OKLCH>(const LinearRGB& lin) {
    LinearRGB oklab = linear_srgb_to_oklab(lin);
    LinearRGB oklch = oklab_to_oklch(oklab.r, oklab.g, oklab.b);
    return OKLCH{
        static_cast<float>(oklch.r),
        static_cast<float>(oklch.g),
        static_cast<float>(oklch.b)
    };
}

template <IsColourType T>
Result<ColourVariant, ColourConvertError> convert(const T& colour, const Colours& target) {
    LinearRGB lin;

    if constexpr (std::is_same_v<T, RGB>) {
        lin = to_linear_srgb(colour);
    } else if constexpr (std::is_same_v<T, RGBA>) {
        lin = to_linear_srgb(colour);
    } else if constexpr (std::is_same_v<T, RGB8>) {
        lin = to_linear_srgb(colour);
    } else if constexpr (std::is_same_v<T, RGBA8>) {
        lin = to_linear_srgb(colour);
    } else if constexpr (std::is_same_v<T, HSL>) {
        lin = to_linear_srgb(colour);
    } else if constexpr (std::is_same_v<T, HSLA>) {
        lin = to_linear_srgb(colour);
    } else if constexpr (std::is_same_v<T, HWB>) {
        lin = to_linear_srgb(colour);
    } else if constexpr (std::is_same_v<T, LAB>) {
        lin = to_linear_srgb(colour);
    } else if constexpr (std::is_same_v<T, OKLAB>) {
        lin = to_linear_srgb(colour);
    } else if constexpr (std::is_same_v<T, LCH>) {
        lin = to_linear_srgb(colour);
    } else if constexpr (std::is_same_v<T, OKLCH>) {
        lin = to_linear_srgb(colour);
    } else {
        return Result<ColourVariant, ColourConvertError>(ColourConvertError::INVALID_SOURCE_TYPE);
    }

    if (lin.r < 0.0 || lin.r > 1.0 || lin.g < 0.0 || lin.g > 1.0 || lin.b < 0.0 || lin.b > 1.0 || lin.a < 0.0 || lin.a > 1.0) {
        return Result<ColourVariant, ColourConvertError>(ColourConvertError::OUT_OF_GAMUT);
    }

    bool target_has_alpha = false;
    switch (target) {
        case Colours::RGBA:
        case Colours::RGBA8:
        case Colours::HSLA:
            target_has_alpha = true;
            break;
        default:
            target_has_alpha = false;
    }

    bool source_has_alpha = false;
    if constexpr (std::is_same_v<T, RGBA> || std::is_same_v<T, RGBA8> || std::is_same_v<T, HSLA>) {
        source_has_alpha = true;
    }

    if (source_has_alpha && !target_has_alpha && lin.a != 1.0) {
        return Result<ColourVariant, ColourConvertError>(ColourConvertError::ALPHA_NOT_SUPPORTED);
    }

    ColourVariant result;
    switch (target) {
        case Colours::RGB:
            result = from_linear_srgb<RGB>(lin);
            break;
        case Colours::RGBA:
            result = from_linear_srgb<RGBA>(lin);
            break;
        case Colours::RGB8:
            result = from_linear_srgb<RGB8>(lin);
            break;
        case Colours::RGBA8:
            result = from_linear_srgb<RGBA8>(lin);
            break;
        case Colours::HSL:
            result = from_linear_srgb<HSL>(lin);
            break;
        case Colours::HSLA:
            result = from_linear_srgb<HSLA>(lin);
            break;
        case Colours::HWB:
            result = from_linear_srgb<HWB>(lin);
            break;
        case Colours::LAB:
            result = from_linear_srgb<LAB>(lin);
            break;
        case Colours::OKLAB:
            result = from_linear_srgb<OKLAB>(lin);
            break;
        case Colours::LCH:
            result = from_linear_srgb<LCH>(lin);
            break;
        case Colours::OKLCH:
            result = from_linear_srgb<OKLCH>(lin);
            break;
        default:
            return Result<ColourVariant, ColourConvertError>(ColourConvertError::INVALID_TARGET_ENUM);
    }

    return Result<ColourVariant, ColourConvertError>(std::move(result));
}

template Result<ColourVariant, ColourConvertError> convert<RGB>(const RGB&, const Colours&);
template Result<ColourVariant, ColourConvertError> convert<RGBA>(const RGBA&, const Colours&);
template Result<ColourVariant, ColourConvertError> convert<RGB8>(const RGB8&, const Colours&);
template Result<ColourVariant, ColourConvertError> convert<RGBA8>(const RGBA8&, const Colours&);
template Result<ColourVariant, ColourConvertError> convert<HSL>(const HSL&, const Colours&);
template Result<ColourVariant, ColourConvertError> convert<HSLA>(const HSLA&, const Colours&);
template Result<ColourVariant, ColourConvertError> convert<HWB>(const HWB&, const Colours&);
template Result<ColourVariant, ColourConvertError> convert<LAB>(const LAB&, const Colours&);
template Result<ColourVariant, ColourConvertError> convert<OKLAB>(const OKLAB&, const Colours&);
template Result<ColourVariant, ColourConvertError> convert<LCH>(const LCH&, const Colours&);
template Result<ColourVariant, ColourConvertError> convert<OKLCH>(const OKLCH&, const Colours&);

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