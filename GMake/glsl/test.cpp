#include "tokeniser_glsl.h"
#include <string>
#include <vector>
#include <array>

#include "parser_glsl.h"

namespace glsl {

std::string to_string(TokenType type) {
    switch (type) {
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::IDENT: return "IDENT";
        case TokenType::KEYWORD: return "KEYWORD";
        case TokenType::LITERAL_TYPE: return "LITERAL_TYPE";
        case TokenType::VERSION: return "VERSION";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::LEFT_SQAURE_BRACKET: return "LEFT_SQAURE_BRACKET";
        case TokenType::RIGHT_SQAURE_BRACKET: return "RIGHT_SQAURE_BRACKET";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::STAR_ASSIGN: return "STAR_ASSIGN";
        case TokenType::PLUS_ASSIGN: return "PLUS_ASSIGN";
        case TokenType::MINUS_ASSIGN: return "MINUS_ASSIGN";
        case TokenType::SLASH_ASSIGN: return "SLASH_ASSIGN";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::PRECENT: return "PRECENT";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::GREATER: return "GREATER";
        case TokenType::LESS: return "LESS";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::NOT_EQUAL: return "NOT_EQUAL";
        case TokenType::NOT: return "NOT";
        case TokenType::OR: return "OR";
        case TokenType::AND: return "AND";
        case TokenType::BITWISE_AND: return "BITWISE_AND";
        case TokenType::BITWISE_OR: return "BITWISE_OR";
        case TokenType::BITWISE_XOR: return "BITWISE_XOR";
        case TokenType::BITWISE_LEFT_SHIFT: return "BITWISE_LEFT_SHIFT";
        case TokenType::BITWISE_RIGHT_SHIFT: return "BITWISE_RIGHT_SHIFT";
        case TokenType::BITWISE_NOT: return "BITWISE_NOT";
        case TokenType::QUESTION_MARK: return "QUESTION_MARK";
        case TokenType::COLON: return "COLON";
        case TokenType::INCREMENT: return "INCREMENT";
        case TokenType::DECREMENT: return "DECREMENT";
        case TokenType::ARROW: return "ARROW";
        case TokenType::DOUBLE_STAR: return "DOUBLE_STAR";
        case TokenType::ERROR: return "ERROR";
    case TokenType::NONE: return "NONE";
    }
    return "UNKNOWN";
}

std::string to_string(LiteralType type) {
    switch (type) {
        case LiteralType::INT: return "INT";
        case LiteralType::FLOAT: return "FLOAT";
        case LiteralType::BOOL: return "BOOL";
    }
    return "UNKNOWN";
}

void print_token(const Token& token) {
    std::string lit_types;
    for (size_t i = 0; i < token.literal_types.size(); i++) {
        if (i > 0) lit_types += ", ";
        lit_types += to_string(token.literal_types[i]);
    }


    fmt::println("Token{{\n"
                 "  type: {},\n"
                 "  value: \"{}\",\n"
                 "  line: {},\n"
                 "  range: [{}, {}],\n"
                 "  literal_types: [{}]\n"
                 "}}",
                 to_string(token.type),
                 token.value,
                 token.line_number,
                 token.position_range.first,
                 token.position_range.second,
                 lit_types);}
}

int main(){
    namespace fs = std::filesystem;
    const fs::path shader_paths = "shaders/test_shaders";
    const std::string file_data = glsl::ReadFilePath(shader_paths);
    std::vector<std::string> lines = {};
    std::string line;
    for (const char& data : file_data){
        if (data == '\n') {
            if (!line.empty() && line.back() == '\r'){
                line.pop_back();
            }
            lines.push_back(line);
            line.clear();
        }
        else{
            line.push_back(data);
        }
    }
    for (const std::string& string : lines){
        std::cout<<string<<std::endl;
    }
    for (const std::string& shader_file_name : lines){
        std::cout << shader_file_name << std::endl;
        glsl::Tokeniser tokeniser;
        fs::path full_path = fs::path("shaders") / shader_file_name;
        std::cout << full_path << std::endl;
        auto result = tokeniser.tokenize(full_path);
        std::cout << " i am here " << shader_file_name << std::endl;
        auto err = result.check_error();
        std::cout << to_string(err) << " thing " << std::endl;
        if (err != glsl::TokenizerError{}) {
            fmt::println("Error tokenizing {}: {}", shader_file_name, glsl::to_string(err));
            result.Handle_Error();
            continue;
        }
        std::vector<glsl::Token> tokens = result.GetData();
        std::cout << shader_file_name << " die " << std::endl;
        fmt::println("--- {} ({} tokens) ---", shader_file_name, tokens.size());
        for (const auto& token : tokens) {
            glsl::print_token(token);
        }
        result.Handle_Error();
    }

    for (const fs::directory_entry& entry : fs::directory_iterator("parsable_shaders")) {
        if (entry.is_regular_file()) {
            glsl::Tokeniser tokeniser;
            const fs::path& path = entry.path();
            Result<std::vector<glsl::Token>, glsl::TokenizerError> tokens_result = tokeniser.tokenize(path);
            glsl::TokenizerError error = tokens_result.check_error();
            std::cout << to_string(error) << std::endl;
            std::vector<glsl::Token> tokens = tokens_result.GetData();
            for (const auto& token : tokens){
                glsl::print_token(token);
            }
            glsl::Parser parser = glsl::Parser{{}};
            std::vector<glsl::Node> nodes = parser.parse(tokens);
        }
    }
}
