#include "parser_glsl.h"

namespace glsl{
    TypeRegistry::TypeRegistry(){
        for (const TypeDec& build_in : BuiltinTypes::build_ins){
            Result<Empty, TypeError> error = register_build_in_type(build_in);
            if (error.check_error() != TypeError::NONE){
                std::cout << to_string(error.check_error()) << std::endl;
            }
        }
    }

    bool TypeRegistry::has(const std::string& name) const{
        if (type_map.contains(name)){
            return true;
        }
        return false;
    }

    Result<TypeInfo*, TypeError> TypeRegistry::get(const std::string& name){
        if (has(name)){
            return &type_map.at(name);
        }
        return TypeError::TYPE_NOT_FOUND;
    }

    Result<Empty, TypeError>  TypeRegistry::register_type(const std::string& name, const std::string& description, std::optional<std::map<std::shared_ptr<TypeInfo>, Field>> fields){
        if (fields.has_value()){
            for (const std::pair<const std::shared_ptr<TypeInfo>, Field>& field : fields.value()){
                    if (!has(field.first->name)){
                        return TypeError::INVALID_FIELD_TYPE_NAME;
                    }
            }
        }
        if (has(name)){
            return TypeError::TYPE_ALREADY_EXISTS;
        }
        TypeInfo type_info = TypeInfo{.is_build_in = false, .name = name, .description = description, .group = GLSLTypeGroup::STRUCT, .fields = std::move(fields)};
        type_map.emplace(name, std::move(type_info));
        return Empty{};
    }

    Result<Empty, TypeError>  TypeRegistry::register_build_in_type(const TypeDec& type){
        if (type.fields.has_value()){
            for (const std::pair<const std::shared_ptr<TypeDec>, FieldDec>& field : type.fields.value()){
                if (!has(field.first->name)){
                    return TypeError::INVALID_FIELD_TYPE_NAME;
                }
            }
        }
        if (has(type.name)){
            return TypeError::TYPE_ALREADY_EXISTS;
        }
        std::optional<std::map<std::shared_ptr<TypeInfo>, Field>> fields = {};
        if (type.fields.has_value()){
            for (const std::pair<const std::shared_ptr<TypeDec>, FieldDec>& field : *type.fields){
                if (!has(field.first->name)){
                    throw std::runtime_error("Type " + field.first->name + " not registered");
                }
                const TypeInfo& src = type_map.at(field.first->name);
                auto key = std::make_unique<TypeInfo>(TypeInfo{
                    .is_build_in = src.is_build_in,
                    .name = src.name,
                    .description = src.description,
                    .group = src.group,
                    .fields = std::nullopt
                });
                auto value = Field{
         std::make_unique<TypeInfo>(TypeInfo{
                    .is_build_in = src.is_build_in,
                    .name = src.name,
                    .description = src.description,
                    .group = src.group,
                    .fields = std::nullopt
                }),
                field.second.name_aliases
            };
        fields->emplace(std::move(key), std::move(value));
            }
        }
        else{
            fields = std::nullopt;
        }
        TypeInfo type_info = TypeInfo{.is_build_in = true, .name = type.name, .description = type.description, .group = type.group, .fields = std::move(fields)};
        type_map.emplace(type_info.name, std::move(type_info));
        return Empty{};
    }

    template<typename T>
    std::vector<T> slice(const std::vector<T>& v, size_t start, size_t end){
        return std::vector<T>{v.begin() + start, v.begin() + end};
    }
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
        end_token_pos = 0;
        start_token_pos = 0;
        shader_node = {};
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
        end_token_pos = token_pos;
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
        shader_node.nodes = {};
        while (token_pos < input.size()){
            Result<Token, ParserError> token_result = consume_token();
            ParserError error = token_result.check_error();
            if (error != ParserError::NONE){
                throw std::runtime_error("a error happened");
            }
            token_result.Handle_Error();
            Token token = token_result.GetData();
            if (token.type == TokenType::KEYWORD){
                Node node;
                if (token.value == "out" || token.value == "in"){
                    StorageModifierNode storage_modifier_node;
                    if (token.value == "out"){
                        storage_modifier_node.is_out = StorageModifier::OUT;
                    }
                    else{
                        storage_modifier_node.is_out = StorageModifier::IN;
                    }
                }
            }
        }
        throw std::runtime_error("not yet implemented");
        return nodes;
    }

    template <IsNode T>
    Result<Node, ParserError> Parser::push_node(T& node){
        std::vector<Token> tokens_node = slice(tokens, start_token_pos, end_token_pos);
        start_token_pos = token_pos; end_token_pos = token_pos;
        shader_node.nodes.push_back(nodes.size());
        node.tokens = tokens_node;
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
