#include "parser_glsl.h"

namespace glsl{
    std::vector<std::string> splitWords(const std::string& str) {
        std::istringstream ss(str);
        std::vector<std::string> words;
        std::string word;

        while (ss >> word) {
            words.push_back(word);
        }

        return words;
    }

    Parser::Parser(const std::vector<Token>& tokens){
        nodes = {};
        this->tokens = tokens;
        token_pos = 0;
    }

    Result<Token, ParserError> Parser::peek_token(const uint8_t& look_ahead) const {
        if (token_pos + look_ahead >= tokens.size()){
            return ParserError::END_OF_TOKENS;
        }
        Token token = tokens.at(token_pos + look_ahead);
        return token;
    }

    Result<Token, ParserError> Parser::consume_token(){
        Result<Token,ParserError> result_token = peek_token();
        token_pos++;
        return result_token;
    }

    std::vector<Node> Parser::parse(const std::vector<Node>& input){
        Result<Token, ParserError> result_token = consume_token();
        ParserError parser_error = result_token.check_error();
        if (parser_error != ParserError::NONE){
            throw std::runtime_error("Parser error");
        }
        result_token.Handle_Error();
        Token token_version = result_token.GetData();
        if (token_version.type != TokenType::VERSION){
            throw std::runtime_error("Version expected here");
        }
        std::string data = token_version.value;
        std::vector<std::string> strings = splitWords(data);
        GLSLVersionInfo version_info = parseVersion(std::stoi(strings.at(0)), strings.at(1));
        shader_node.glsl_version = version_info;
        shader_node.tokens = {};
        while (token_pos < input.size()){
            Result<Token, ParserError> token_result = consume_token();
            ParserError error = token_result.check_error();
            if (error != ParserError::NONE){
                throw std::runtime_error("a error happened");
            }
            token_result.Handle_Error();
            Token token = token_result.GetData();
            if (token.type == TokenType::IDENT){

            }
        }
        throw std::runtime_error("not yet implemented");
        return nodes;
    }

    Result<Node, ParserError> Parser::push_node(const Node& node){
        shader_node.tokens.push_back(nodes.size());
        nodes.push_back(node);
        return node;
    }

    std::string to_string(ParserError error){
        switch (error){
            case ParserError::NONE: return "NONE";
            case ParserError::FAILED_TO_PUSH: return "FAILED_TO_PUSH";
            case ParserError::END_OF_TOKENS: return "END_OF_TOKENS";
        }
        throw std::runtime_error("Unknown ParserError");
    };
}
