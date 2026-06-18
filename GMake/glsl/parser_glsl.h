#pragma once
#include <string>
#include <variant>
#include <vector>
#include "tokeniser_glsl.h"


namespace glsl{
    enum class GLSLTypeGroup{
        SCALAR,
        VECTOR,
        MATRIX,
        SAMPLER,
        IMAGE,
        BUFFER_OBJECT,
        STRUCT,
        VOID,
    };

    struct TypeInfo;
    struct TypeDec;

    struct Field{
        std::unique_ptr<TypeInfo> type_name;
        std::vector<std::string> name_aliases;
    };

    struct FieldDec{
        std::unique_ptr<TypeDec> type_name;
        std::vector<std::string> name_aliases;
    };

    struct TypeDec{
        std::string name;
        std::string description;
        GLSLTypeGroup group;
        std::optional<std::map<std::unique_ptr<TypeDec>, FieldDec>> fields;
    };

    struct TypeInfo{
        bool is_build_in;
        std::string name;
        std::string description;
        GLSLTypeGroup group;
        std::optional<std::map<std::unique_ptr<TypeInfo>, Field>> fields;
    };

    enum class TypeError{
        NONE DEFAULT_ERROR,
        TYPE_NOT_FOUND,
        TYPE_ALREADY_EXISTS,
        INVALID_FIELD_TYPE_NAME,
    };

    inline std::string to_string(const TypeError& type){
        switch (type){
            case TypeError::TYPE_NOT_FOUND: return "TYPE_NOT_FOUND";
            case TypeError::NONE: return "NONE";
            case TypeError::TYPE_ALREADY_EXISTS: return "TYPE_ALREADY_EXISTS";
            case TypeError::INVALID_FIELD_TYPE_NAME: return "INVALID_FIELD_TYPE_NAME";
        }
        throw std::runtime_error("how did this happen");
    };

    namespace BuiltinTypes {
        inline const TypeDec Void           = {"void",           "no type / no return value",                  GLSLTypeGroup::VOID, std::nullopt};

        inline const TypeDec Bool           = {"bool",           "boolean true/false",                          GLSLTypeGroup::SCALAR, std::nullopt};
        inline const TypeDec Int            = {"int",            "32-bit signed integer",                       GLSLTypeGroup::SCALAR, std::nullopt};
        inline const TypeDec UInt           = {"uint",           "32-bit unsigned integer",                     GLSLTypeGroup::SCALAR, std::nullopt};
        inline const TypeDec Float          = {"float",          "32-bit floating-point",                       GLSLTypeGroup::SCALAR, std::nullopt};
        inline const TypeDec Double         = {"double",         "64-bit floating-point",                       GLSLTypeGroup::SCALAR, std::nullopt};

        inline const TypeDec Vec2           = {"vec2",           "2-component float vector",                    GLSLTypeGroup::VECTOR, std::nullopt};
        inline const TypeDec Vec3           = {"vec3",           "3-component float vector",                    GLSLTypeGroup::VECTOR, std::nullopt};
        inline const TypeDec Vec4           = {"vec4",           "4-component float vector",                    GLSLTypeGroup::VECTOR, std::nullopt};
        inline const TypeDec IVec2          = {"ivec2",          "2-component signed integer vector",           GLSLTypeGroup::VECTOR, std::nullopt};
        inline const TypeDec IVec3          = {"ivec3",          "3-component signed integer vector",           GLSLTypeGroup::VECTOR, std::nullopt};
        inline const TypeDec IVec4          = {"ivec4",          "4-component signed integer vector",           GLSLTypeGroup::VECTOR, std::nullopt};
        inline const TypeDec UVec2          = {"uvec2",          "2-component unsigned integer vector",         GLSLTypeGroup::VECTOR, std::nullopt};
        inline const TypeDec UVec3          = {"uvec3",          "3-component unsigned integer vector",         GLSLTypeGroup::VECTOR, std::nullopt};
        inline const TypeDec UVec4          = {"uvec4",          "4-component unsigned integer vector",         GLSLTypeGroup::VECTOR, std::nullopt};
        inline const TypeDec DVec2          = {"dvec2",          "2-component double vector",                   GLSLTypeGroup::VECTOR, std::nullopt};
        inline const TypeDec DVec3          = {"dvec3",          "3-component double vector",                   GLSLTypeGroup::VECTOR, std::nullopt};
        inline const TypeDec DVec4          = {"dvec4",          "4-component double vector",                   GLSLTypeGroup::VECTOR, std::nullopt};
        inline const TypeDec BVec2          = {"bvec2",          "2-component bool vector",                     GLSLTypeGroup::VECTOR, std::nullopt};
        inline const TypeDec BVec3          = {"bvec3",          "3-component bool vector",                     GLSLTypeGroup::VECTOR, std::nullopt};
        inline const TypeDec BVec4          = {"bvec4",          "4-component bool vector",                     GLSLTypeGroup::VECTOR, std::nullopt};

        inline const TypeDec Mat2           = {"mat2",           "2x2 float matrix",                            GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec Mat3           = {"mat3",           "3x3 float matrix",                            GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec Mat4           = {"mat4",           "4x4 float matrix",                            GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec Mat2x3         = {"mat2x3",         "2 columns x 3 rows float matrix",             GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec Mat2x4         = {"mat2x4",         "2 columns x 4 rows float matrix",             GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec Mat3x2         = {"mat3x2",         "3 columns x 2 rows float matrix",             GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec Mat3x4         = {"mat3x4",         "3 columns x 4 rows float matrix",             GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec Mat4x2         = {"mat4x2",         "4 columns x 2 rows float matrix",             GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec Mat4x3         = {"mat4x3",         "4 columns x 3 rows float matrix",             GLSLTypeGroup::MATRIX, std::nullopt};

        inline const TypeDec DMat2          = {"dmat2",          "2x2 double matrix",                           GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec DMat3          = {"dmat3",          "3x3 double matrix",                           GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec DMat4          = {"dmat4",          "4x4 double matrix",                           GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec DMat2x3        = {"dmat2x3",        "2 columns x 3 rows double matrix",            GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec DMat2x4        = {"dmat2x4",        "2 columns x 4 rows double matrix",            GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec DMat3x2        = {"dmat3x2",        "3 columns x 2 rows double matrix",            GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec DMat3x4        = {"dmat3x4",        "3 columns x 4 rows double matrix",            GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec DMat4x2        = {"dmat4x2",        "4 columns x 2 rows double matrix",            GLSLTypeGroup::MATRIX, std::nullopt};
        inline const TypeDec DMat4x3        = {"dmat4x3",        "4 columns x 3 rows double matrix",            GLSLTypeGroup::MATRIX, std::nullopt};

        inline const TypeDec Sampler1D        = {"sampler1D",        "1D texture sampler",                     GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec Sampler2D        = {"sampler2D",        "2D texture sampler",                     GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec Sampler3D        = {"sampler3D",        "3D texture sampler",                     GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec SamplerCube      = {"samplerCube",      "cubemap texture sampler",                 GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec Sampler2DRect    = {"sampler2DRect",    "rectangle texture sampler",               GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec Sampler1DArray   = {"sampler1DArray",   "1D texture array sampler",                GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec Sampler2DArray   = {"sampler2DArray",   "2D texture array sampler",                GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec SamplerCubeArray = {"samplerCubeArray", "cubemap array sampler",                   GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec SamplerBuffer    = {"samplerBuffer",    "buffer texture sampler",                  GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec Sampler2DMS      = {"sampler2DMS",      "2D multisample texture sampler",          GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec Sampler2DMSArray = {"sampler2DMSArray", "2D multisample array texture sampler",    GLSLTypeGroup::SAMPLER, std::nullopt};

        inline const TypeDec Sampler1DShadow        = {"sampler1DShadow",        "1D shadow sampler",                    GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec Sampler2DShadow        = {"sampler2DShadow",        "2D shadow sampler",                    GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec SamplerCubeShadow      = {"samplerCubeShadow",      "cubemap shadow sampler",                GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec Sampler2DRectShadow    = {"sampler2DRectShadow",    "rectangle shadow sampler",              GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec Sampler1DArrayShadow   = {"sampler1DArrayShadow",   "1D array shadow sampler",               GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec Sampler2DArrayShadow   = {"sampler2DArrayShadow",   "2D array shadow sampler",               GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec SamplerCubeArrayShadow = {"samplerCubeArrayShadow", "cubemap array shadow sampler",          GLSLTypeGroup::SAMPLER, std::nullopt};

        inline const TypeDec ISampler1D        = {"isampler1D",        "1D integer texture sampler",              GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec ISampler2D        = {"isampler2D",        "2D integer texture sampler",              GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec ISampler3D        = {"isampler3D",        "3D integer texture sampler",              GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec ISamplerCube      = {"isamplerCube",      "cubemap integer sampler",                 GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec ISampler2DRect    = {"isampler2DRect",    "rectangle integer sampler",               GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec ISampler1DArray   = {"isampler1DArray",   "1D integer array sampler",                 GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec ISampler2DArray   = {"isampler2DArray",   "2D integer array sampler",                 GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec ISamplerCubeArray = {"isamplerCubeArray", "cubemap integer array sampler",            GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec ISamplerBuffer    = {"isamplerBuffer",    "integer buffer sampler",                   GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec ISampler2DMS      = {"isampler2DMS",      "integer multisample sampler",              GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec ISampler2DMSArray = {"isampler2DMSArray", "integer multisample array sampler",        GLSLTypeGroup::SAMPLER, std::nullopt};

        inline const TypeDec USampler1D        = {"usampler1D",        "1D unsigned integer sampler",              GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec USampler2D        = {"usampler2D",        "2D unsigned integer sampler",              GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec USampler3D        = {"usampler3D",        "3D unsigned integer sampler",              GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec USamplerCube      = {"usamplerCube",      "cubemap unsigned integer sampler",         GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec USampler2DRect    = {"usampler2DRect",    "rectangle unsigned integer sampler",       GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec USampler1DArray   = {"usampler1DArray",   "1D unsigned integer array sampler",        GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec USampler2DArray   = {"usampler2DArray",   "2D unsigned integer array sampler",        GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec USamplerCubeArray = {"usamplerCubeArray", "cubemap unsigned integer array sampler",   GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec USamplerBuffer    = {"usamplerBuffer",    "unsigned integer buffer sampler",          GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec USampler2DMS      = {"usampler2DMS",      "unsigned integer multisample sampler",     GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec USampler2DMSArray = {"usampler2DMSArray", "unsigned integer array multisample",       GLSLTypeGroup::SAMPLER, std::nullopt};

        inline const TypeDec Image1D        = {"image1D",        "1D image unit",                     GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec Image2D        = {"image2D",        "2D image unit",                     GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec Image3D        = {"image3D",        "3D image unit",                     GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec ImageCube      = {"imageCube",      "cubemap image unit",                GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec Image2DRect    = {"image2DRect",    "rectangle image unit",              GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec Image1DArray   = {"image1DArray",   "1D array image unit",               GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec Image2DArray   = {"image2DArray",   "2D array image unit",               GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec ImageCubeArray = {"imageCubeArray", "cubemap array image unit",          GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec ImageBuffer    = {"imageBuffer",    "buffer image unit",                 GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec Image2DMS      = {"image2DMS",      "multisample image unit",            GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec Image2DMSArray = {"image2DMSArray", "multisample array image unit",      GLSLTypeGroup::IMAGE, std::nullopt};

        inline const TypeDec IImage1D        = {"iimage1D",        "integer 1D image unit",            GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec IImage2D        = {"iimage2D",        "integer 2D image unit",            GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec IImage3D        = {"iimage3D",        "integer 3D image unit",            GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec IImageCube      = {"iimageCube",      "integer cubemap image unit",       GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec IImage2DRect    = {"iimage2DRect",    "integer rectangle image unit",     GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec IImage1DArray   = {"iimage1DArray",   "integer 1D array image unit",      GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec IImage2DArray   = {"iimage2DArray",   "integer 2D array image unit",      GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec IImageCubeArray = {"iimageCubeArray", "integer cubemap array image unit", GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec IImageBuffer    = {"iimageBuffer",    "integer buffer image unit",        GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec IImage2DMS      = {"iimage2DMS",      "integer multisample image unit",   GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec IImage2DMSArray = {"iimage2DMSArray", "integer array multisample image",  GLSLTypeGroup::IMAGE, std::nullopt};

        inline const TypeDec UImage1D        = {"uimage1D",        "unsigned 1D image unit",            GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec UImage2D        = {"uimage2D",        "unsigned 2D image unit",            GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec UImage3D        = {"uimage3D",        "unsigned 3D image unit",            GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec UImageCube      = {"uimageCube",      "unsigned cubemap image unit",       GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec UImage2DRect    = {"uimage2DRect",    "unsigned rectangle image unit",     GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec UImage1DArray   = {"uimage1DArray",   "unsigned 1D array image unit",      GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec UImage2DArray   = {"uimage2DArray",   "unsigned 2D array image unit",      GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec UImageCubeArray = {"uimageCubeArray", "unsigned cubemap array image unit", GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec UImageBuffer    = {"uimageBuffer",    "unsigned buffer image unit",        GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec UImage2DMS      = {"uimage2DMS",      "unsigned multisample image unit",   GLSLTypeGroup::IMAGE, std::nullopt};
        inline const TypeDec UImage2DMSArray = {"uimage2DMSArray", "unsigned array multisample image",  GLSLTypeGroup::IMAGE, std::nullopt};

        inline const TypeDec AtomicUInt      = {"atomic_uint",     "unsigned integer atomic counter",   GLSLTypeGroup::BUFFER_OBJECT, std::nullopt};

        inline const TypeDec SubpassInput    = {"subpassInput",    "framebuffer local subpass input",       GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec SubpassInputMS  = {"subpassInputMS",  "multisampled framebuffer subpass input",GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec ISubpassInput   = {"isubpassInput",   "integer subpass input",                  GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec ISubpassInputMS = {"isubpassInputMS", "integer multisample subpass input",      GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec USubpassInput   = {"usubpassInput",   "unsigned integer subpass input",         GLSLTypeGroup::SAMPLER, std::nullopt};
        inline const TypeDec USubpassInputMS = {"usubpassInputMS", "unsigned multisample subpass input",     GLSLTypeGroup::SAMPLER, std::nullopt};

        class Buildins{
            public:
                std::vector<TypeDec> buildin_type_decs = {};
        };
    }

    struct Empty{};

    class TypeRegistry{
    public:
        Result<Empty, TypeError> register_build_in_type(const TypeDec& desc);
        Result<Empty, TypeError> register_type(const std::string& name, const std::string& description, std::optional<std::map<std::unique_ptr<TypeInfo>, Field>> fields);
        Result<TypeInfo*, TypeError> get(const std::string& name);
        [[nodiscard]] bool has(const std::string& name) const;
        std::map<std::string, TypeInfo> type_map;
    };

    enum class OperatorType{
        ADDITION,
        SUBTRACTION,
        MULTIPLICATION,
        DIVISION,
        MODULATION,
        SUBSCRIPT,
        DOT,
        GREATER_THAN,
        LESS_THAN,
        EQUAL_TO,
        NOT_EQUAL,
        NOT,
        OR,
        AND,
        BITWISE_AND,
        BITWISE_OR,
        BITWISE_XOR,
        BITWISE_NOT,
        LEFT_SHIFT,
        RIGHT_SHIFT,
        QUESTION_MARK,
        COLON,
        ARROW,
        EXPONENT,
    };

    struct UnOppNode{
        OperatorType operator_name;
        size_t value_node;
        std::vector<Token> tokens;
    };

    struct BinaryOppNode{
        OperatorType operator_name;
        size_t left_node;
        size_t right_node;
        std::vector<Token> tokens;
    };

    struct ShaderNode{
        std::vector<size_t> nodes = {};
        GLSLVersionInfo glsl_version;
        std::vector<Token> tokens = {};
    };

    enum class StorageModifier{
        DEFAULT,
        IN,
        OUT,
    };

    struct StorageModifierNode{
        StorageModifier is_out;
        std::vector<Token> tokens = {};
    };

    enum class ParserError{
        NONE,
        FAILED_TO_PUSH,
        END_OF_TOKENS,
    };
    using Node = std::variant<UnOppNode, BinaryOppNode, ShaderNode>;

    template <typename T>
    concept IsNode = requires(const T& t)
    {
        { t.tokens } -> std::same_as<const std::vector<Token>&>;
        {t} -> std::same_as<const Node&>;
    };

    std::string to_string(ParserError error);

    class Parser{
    public:
        Parser(const std::vector<Token>& tokens);
        std::vector<Node> parse(const std::vector<Node>& input);
        [[nodiscard]] Result<Token, ParserError> peek_token(const uint8_t& look_ahead = 0) const;
        Result<Token, ParserError> consume_token();
        template <IsNode T>
        Result<Node,ParserError> push_node(T& node);
        std::vector<Node> nodes;
        std::vector<Token> tokens;
        size_t token_pos;
        ShaderNode shader_node;
        size_t start_token_pos;
        size_t end_token_pos;
    };
}
