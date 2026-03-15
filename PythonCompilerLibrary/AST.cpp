#include "AST.h"
#include <algorithm>

bool keyword_check(const token& token, const KeyWord& keyword){
    if (token.type != TokenType::KEYWORD){
        return false;
    }
    if (std::get<KeyWord>(token.value) == keyword){
        return true;
    }
    return false;
}
Parser::Parser(){
    TokenPos = 0;
    Tokens = {};
    IndentLevel = 0;
    Node_pos = 0;
    AST = {};
}

void Parser::reset_parser(){
    TokenPos = 0;
    Tokens = {};
    IndentLevel = 0;
    Node_pos = 0;
    AST = {};
}

std::vector<Node> Parser::program_parse_ast(const std::vector<token>& tokens){
    Tokens = tokens;
    while(TokenPos < Tokens.size()){}
    reset_parser();
    return AST;
}

size_t Parser::parse_expression(){
    size_t exp_node = parse_keyword_or();
    return exp_node;
}

size_t Parser::parse_keyword_or(){
    size_t exp_node = -1;
    size_t left = parse_keyword_and();
    token tok = {};
    while (peek_token().type != TokenType::EMPTY && keyword_check(peek_token(), KeyWord::OR)){
        token operator_node = get_token();
        TokenType operator_token = operator_node.type;
        auto it = std::find_if(token_type_char.begin(), token_type_char.end(),[&](const auto& pair){ return pair.first == operator_token; });
        std::string operator_str;
        if (it != token_type_char.end()) {
            operator_str = it->second;  // <-- dereference iterator
        } else {
            throw std::runtime_error("that's just not a thing");
        }
        BinaryExprNode expr = {.opp = operator_str, .left = left, .right = parse_keyword_and()};
        exp_node = push_node(expr);
    }
    if (exp_node == -1){
        exp_node = left;
    }
    return exp_node;
}

size_t Parser::parse_keyword_and(){
    size_t exp_node = -1;
    size_t left = parse_comparison();
    token tok = {};
    while (peek_token().type != TokenType::EMPTY && keyword_check(peek_token(), KeyWord::AND)){
        token operator_node = get_token();
        TokenType operator_token = operator_node.type;
        auto it = std::find_if(token_type_char.begin(), token_type_char.end(),[&](const auto& pair){ return pair.first == operator_token; });
        std::string operator_str;
        if (it != token_type_char.end()) {
            operator_str = it->second;  // <-- dereference iterator
        } else {
            throw std::runtime_error("that's just not a thing");
        }
        BinaryExprNode expr = {.opp = operator_str, .left = left, .right = parse_comparison()};
        exp_node = push_node(expr);
    }
    if (exp_node == -1){
        exp_node = left;
    }
    return exp_node;
}

size_t Parser::parse_comparison(){
    size_t exp_node = -1;
    size_t left = parse_or();
    token tok = {};
    while ((peek_token().type != TokenType::EMPTY) && (peek_token().type == TokenType::GREATER || peek_token().type == TokenType::LESS || peek_token().type == TokenType::LESSEQUAL
            || peek_token().type == TokenType::GREATEREQUAL || peek_token().type == TokenType::NOTEQUAL || peek_token().type == TokenType::EQEQUAL ||
            keyword_check(peek_token(),KeyWord::IS) || (keyword_check(peek_token(), KeyWord::NOT) && keyword_check(peek_token(1),KeyWord::IN)) || keyword_check(peek_token(), KeyWord::IN))){
        token operator_node = get_token();
        TokenType operator_token = operator_node.type;
        auto it = std::find_if(token_type_char.begin(), token_type_char.end(),[&](const auto& pair){ return pair.first == operator_token; });
        std::string operator_str;
        if (it != token_type_char.end()) {
            operator_str = it->second;  // <-- dereference iterator
        } else {
            throw std::runtime_error("that's just not a thing");
        }
        BinaryExprNode expr = {.opp = operator_str, .left = left, .right = parse_or()};
        exp_node = push_node(expr);
    }
    if (exp_node == -1){
        exp_node = left;
    }
    return exp_node;
}

size_t Parser::parse_or(){
    size_t exp_node = -1;
    size_t left = parse_xor();
    token tok = {};
    while ((peek_token().type != TokenType::EMPTY) && (peek_token().type == TokenType::VBAR)){
        token operator_node = get_token();
        TokenType operator_token = operator_node.type;
        auto it = std::find_if(token_type_char.begin(), token_type_char.end(),[&](const auto& pair){ return pair.first == operator_token; });
        std::string operator_str;
        if (it != token_type_char.end()) {
            operator_str = it->second;  // <-- dereference iterator
        } else {
            throw std::runtime_error("that's just not a thing");
        }
        BinaryExprNode expr = {.opp = operator_str, .left = left, .right = parse_xor()};
        exp_node = push_node(expr);
    }
    if (exp_node == -1){
        exp_node = left;
    }
    return exp_node;
}

size_t Parser::parse_xor(){
    size_t exp_node = -1;
    size_t left = parse_and();
    token tok = {};
    while ((peek_token().type != TokenType::EMPTY) && peek_token().type == TokenType::CIRCUMFLEX){
        token operator_node = get_token();
        TokenType operator_token = operator_node.type;
        auto it = std::find_if(token_type_char.begin(), token_type_char.end(),[&](const auto& pair){ return pair.first == operator_token; });
        std::string operator_str;
        if (it != token_type_char.end()) {
            operator_str = it->second;  // <-- dereference iterator
        } else {
            throw std::runtime_error("that's just not a thing");
        }
        BinaryExprNode expr = {.opp = operator_str, .left = left, .right = parse_and()};
        exp_node = push_node(expr);
    }
    if (exp_node == -1){
        exp_node = left;
    }
    return exp_node;
}

size_t Parser::parse_and(){
    size_t exp_node = -1;
    size_t left = parse_shift();
    token tok = {};
    while ((peek_token().type != TokenType::EMPTY) && peek_token().type == TokenType::AMPER){
        token operator_node = get_token();
        TokenType operator_token = operator_node.type;
        auto it = std::find_if(token_type_char.begin(), token_type_char.end(),[&](const auto& pair){ return pair.first == operator_token; });
        std::string operator_str;
        if (it != token_type_char.end()) {
            operator_str = it->second;  // <-- dereference iterator
        } else {
            throw std::runtime_error("that's just not a thing");
        }
        BinaryExprNode expr = {.opp = operator_str, .left = left, .right = parse_shift()};
        exp_node = push_node(expr);
    }
    if (exp_node == -1){
        exp_node = left;
    }
    return exp_node;
}

size_t Parser::parse_shift(){
    size_t exp_node = -1;
    size_t left = parse_addsub();
    token tok = {};
    while ((peek_token().type != TokenType::EMPTY) && (peek_token().type == TokenType::LEFTSHIFT || peek_token().type == TokenType::RIGHTSHIFT)){
        token operator_node = get_token();
        TokenType operator_token = operator_node.type;
        auto it = std::find_if(token_type_char.begin(), token_type_char.end(),[&](const auto& pair){ return pair.first == operator_token; });
        std::string operator_str;
        if (it != token_type_char.end()) {
            operator_str = it->second;  // <-- dereference iterator
        } else {
            throw std::runtime_error("that's just not a thing");
        }
        BinaryExprNode expr = {.opp = operator_str, .left = left, .right = parse_addsub()};
        exp_node = push_node(expr);
    }
    if (exp_node == -1){
        exp_node = left;
    }
    return exp_node;
}

size_t Parser::parse_addsub(){
    size_t exp_node = -1;
    size_t left = parse_muldiv();
    token tok = {};
    while ((peek_token().type != TokenType::EMPTY) && (peek_token().type == TokenType::PLUS || peek_token().type == TokenType::MINUS)){
        token operator_node = get_token();
        TokenType operator_token = operator_node.type;
        auto it = std::find_if(token_type_char.begin(), token_type_char.end(),[&](const auto& pair){ return pair.first == operator_token; });
        std::string operator_str;
        if (it != token_type_char.end()) {
            operator_str = it->second;  // <-- dereference iterator
        } else {
            throw std::runtime_error("that's just not a thing");
        }
        BinaryExprNode expr = {.opp = operator_str, .left = left, .right = parse_muldiv()};
        exp_node = push_node(expr);
    }
    if (exp_node == -1){
        exp_node = left;
    }
    return exp_node;
}

size_t Parser::parse_muldiv(){
    size_t exp_node = -1;
    size_t left = parse_exponent();
    token tok = {};
    while ((peek_token().type != TokenType::EMPTY) && (peek_token().type == TokenType::AT || peek_token().type == TokenType::STAR ||
            peek_token().type == TokenType::SLASH || peek_token().type == TokenType::DOUBLESLASH || peek_token().type == TokenType::PERCENT)){
        token operator_node = get_token();
        TokenType operator_token = operator_node.type;
        auto it = std::find_if(token_type_char.begin(), token_type_char.end(),[&](const auto& pair){ return pair.first == operator_token; });
        std::string operator_str;
        if (it != token_type_char.end()) {
            operator_str = it->second;  // <-- dereference iterator
        } else {
            throw std::runtime_error("that's just not a thing");
        }
        BinaryExprNode expr = {.opp = operator_str, .left = left, .right = parse_exponent()};
        exp_node = push_node(expr);
    }
    if (exp_node == -1){
        exp_node = left;
    }
    return exp_node;
}

size_t Parser::parse_exponent(){
    size_t left = parse_factor();

    if (peek_token().type == TokenType::DOUBLESTAR){
        token operator_node = get_token();
        TokenType operator_token = operator_node.type;
        auto it = std::find_if(token_type_char.begin(),token_type_char.end(),[&](const auto& pair){ return pair.first == operator_token; });
        std::string operator_str;
        if (it != token_type_char.end()){
            operator_str = it->second;
        } else {
            throw std::runtime_error("that's just not a thing");
        }
        // recursion instead of loop
        size_t right = parse_exponent();
        BinaryExprNode expr = {.opp = operator_str,.left = left,.right = right};
        return push_node(expr);
    }
    return left;
}

size_t Parser::parse_factor(){
    size_t expr_node = -1;
    Node factor;
    token tok = get_token();
    if (tok.type == TokenType::NUMBER){
        NumberNode number{.number = std::stof(std::get<std::string>(tok.value))};
        factor = number;
    }
    else if (tok.type == TokenType::STRING){
        StringNode string;
        string.string = std::get<std::string>(tok.value);
        factor = string;
    }
    else if (tok.type == TokenType::KEYWORD && (KeyWord::TRUE == std::get<KeyWord>(tok.value) || KeyWord::FALSE == std::get<KeyWord>(tok.value))){
        KeyWord value = std::get<KeyWord>(tok.value);
        bool bool_value;
        if (KeyWord::TRUE == value){
            bool_value = true;
        }
        else{
            bool_value = false;
        }
        BoolNode boolean{.boolean = bool_value};
        factor = boolean;
    }
    else if (tok.type == TokenType::LBRACE || tok.type == TokenType::LBRACK || tok.type == TokenType::LPAREN){
        if (tok.type == TokenType::LPAREN){
            size_t expr = parse_expression();
            if (TokenType::COMMA == get_token().type){
                CollectionNode collection = {.type = CollectionType::Tuple, .values = {}};
                std::vector<size_t> collected;
                collected.push_back(expr);
                while (TokenType::COMMA == peek_token().type){
                    get_token();
                    collected.push_back(parse_expression());
                }
                if (TokenType::RPAREN != peek_token().type){
                    throw std::runtime_error("parentheses are not allowed");
                }
                collection.values = collected;
                factor = collection;
            }
            else{
                expr_node = parse_expression();
            }
        }
        else{
            CollectionNode collection = {};
            TokenType end_brack = tok.type;
            CollectionType collection_type;
            bool is_dict = false;
            if (tok.type == TokenType::LBRACE){}
            else{
                collection_type = CollectionType::List;
            }
            std::vector<size_t> collected;
            while (TokenType::COMMA == peek_token().type || TokenType::COLON != peek_token().type){
                token comma_or_colon = get_token();

                if (tok.type == TokenType::COLON){
                    is_dict = true;
                }
                collected.push_back(parse_expression());
            }
            if (is_dict == false){
                collection_type = CollectionType::Set;
            }
            else{
                collection_type = CollectionType::Dict;
            }
            collection.values = collected;
            collection.type = collection_type;
            factor = collection;
        }
    }
    else{
        //@TODO: all the other ones we do still need here in the future
    }
    size_t node_index = 0;
    if (expr_node != -1){
        node_index = expr_node;
    }
    else{
        node_index = push_node(factor);
    }
    return node_index;
}

size_t Parser::push_node(const Node& node){
    size_t node_index = AST.size();
    AST.push_back(node);
    return node_index;
}

token Parser::get_token(){
    token tok = peek_token();
    TokenPos++;
    return tok;
}

token Parser::peek_token(const int& look_ahead){
    token tok;
    if (TokenPos  + look_ahead > Tokens.size()){
        tok = {.type = TokenType::EMPTY, .value = ""};
    }
    else{
        tok = Tokens[TokenPos + look_ahead];
    }
    return tok;
}

bool Parser::match_token(const TokenType& type){
    return peek_token().type == type;
}

void Parser::expect_token(TokenType type){
    bool is_expected = match_token(type);
    if (!is_expected){
        throw std::runtime_error("this is not a valid token");
    }
    get_token();
}
