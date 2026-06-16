#pragma once
#include <string>
#include <variant>
#include <vector>
#include "tokeniser_glsl.h"

namespace glsl{
    enum class RegisterType{
        VARS,
        FUNCS,
        TYPES,
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

    struct TypeId{
        RegisterType type;
        size_t id;
        std::vector<Token> tokens;
    };

    struct TypeName{
        RegisterType type;
        std::string name;
        std::vector<Token> tokens;
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
        std::vector<size_t> tokens = {};
        GLSLVersionInfo glsl_version;
    };

    enum class ParserError{
        NONE,
        FAILED_TO_PUSH,
        END_OF_TOKENS,
    };

    std::string to_string(ParserError error);

    using Node = std::variant<UnOppNode, BinaryOppNode, ShaderNode>;
    class Parser{
    public:
        Parser(const std::vector<Token>& tokens);
        std::vector<Node> parse(const std::vector<Node>& input);
        [[nodiscard]] Result<Token, ParserError> peek_token(const uint8_t& look_ahead = 0) const;
        Result<Token, ParserError> consume_token();
        Result<Node,ParserError> push_node(const Node& node);
        std::vector<Node> nodes;
        std::vector<Token> tokens;
        size_t token_pos;
        ShaderNode shader_node;
    };
}
