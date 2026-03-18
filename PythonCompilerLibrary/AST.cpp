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

    // 1. Parentheses (tightest precedence)
    if (tok.type == TokenType::LPAREN) {
        size_t first_expr = parse_expression();
        if (peek_token().type == TokenType::COMMA) {
            CollectionNode tuple{.type = CollectionType::Tuple};
            std::vector<size_t> values;
            values.push_back(first_expr);
            while (peek_token().type == TokenType::COMMA) {
                get_token();
                values.push_back(parse_expression());
            }
            if (peek_token().type != TokenType::RPAREN)
                throw std::runtime_error("Expected closing parenthesis");
            get_token();
            tuple.values = values;
            factor = tuple;
        } else {
            if (peek_token().type != TokenType::RPAREN)
                throw std::runtime_error("Expected closing parenthesis");
            get_token();
            expr_node = first_expr;
        }
    }
    // 2. Collections
    else if (tok.type == TokenType::LBRACE || tok.type == TokenType::LBRACK) {
        CollectionNode collection{};
        std::vector<size_t> collected;
        bool is_dict = false;
        TokenType end = (tok.type == TokenType::LBRACE) ? TokenType::RBRACE : TokenType::RBRACK;

        if (peek_token().type == end) {
            get_token(); // consume empty collection
            collection.type = (tok.type == TokenType::LBRACE) ? CollectionType::Dict : CollectionType::List;
        } else {
            size_t first = parse_expression();
            if (tok.type == TokenType::LBRACE && peek_token().type == TokenType::COLON) {
                is_dict = true;
                get_token();
                collected.push_back(first);
                collected.push_back(parse_expression());
            } else {
                collected.push_back(first);
            }

            while (peek_token().type == TokenType::COMMA) {
                get_token();
                if (peek_token().type == end) break;
                size_t key = parse_expression();
                if (is_dict) {
                    if (peek_token().type != TokenType::COLON)
                        throw std::runtime_error("Expected ':' in dict");
                    get_token();
                    collected.push_back(key);
                    collected.push_back(parse_expression());
                } else {
                    collected.push_back(key);
                }
            }

            if (peek_token().type != end)
                throw std::runtime_error("Expected closing bracket");
            get_token();
            if (tok.type == TokenType::LBRACE)
                collection.type = is_dict ? CollectionType::Dict : CollectionType::Set;
            else
                collection.type = CollectionType::List;

            collection.values = collected;
        }
        factor = collection;
    }
    else if (tok.type == TokenType::NUMBER){
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
    else if (tok.type == TokenType::IDENT){
        Node ident_res;
        std::string ident = std::get<std::string>(tok.value);
        size_t annotations = -1;
        size_t expression = -1;
        bool is_assign = true;
        if (peek_token().type == TokenType::COLON){
            get_token();
            annotations = parse_expression();
            if (peek_token().type == TokenType::EQUAL){
                expression = parse_expression();
            }
        }
        else if (peek_token().type == TokenType::EQUAL){
            expression = parse_expression();
        }
        else if (peek_token().type == TokenType::LPAREN){
            is_assign = false;
            std::vector<size_t> args = {};
            bool default_happend = false;
            bool arg_happend = false;
            bool kwarg_happend = false;
            while (peek_token().type == TokenType::COMMA){
                ArgNode argument = {};
                token arg_first = get_token();
                IdentNode ident_node = {.ident = std::get<std::string>(arg_first.value)};
                size_t ident_size_t = push_node(ident_node);
                size_t annotations_expr = -1;
                if (peek_token().type == TokenType::COLON){
                    annotations_expr = parse_expression();
                }
                size_t expression_arg = -1;
                if (peek_token().type == TokenType::EQUAL){
                    default_happend = true;
                    expression_arg = parse_expression();
                }
                else if (peek_token().type == TokenType::STAR){
                    get_token();
                    if ((peek_token().type == TokenType::STAR)){
                        get_token();
                        if (!(std::get<std::string>(peek_token().value) == "kwarg" && peek_token().type == TokenType::IDENT)){
                            throw std::runtime_error("Expected kwarg");
                        }
                        kwarg_happend = true;
                        argument.is_kwargs = true;
                    }
                    else if (peek_token().type == TokenType::IDENT && std::get<std::string>(peek_token().value) == "arg"){
                        arg_happend = true;
                        argument.is_args = true;
                    }
                    else{
                        throw std::runtime_error("Expected kwarg");
                    }
                }
                argument.annotation = annotations_expr;
                argument.default_value = expression_arg;
                argument.identifier = ident_size_t;
                size_t arg = push_node(argument);
                args.push_back(arg);
            }
            IdentNode func_name = {.ident = ident};
            size_t func_identifier = push_node(func_name);
            CallExprNode call_expr_node = {.is_await = false, .name = func_identifier, .args = args};
            factor = call_expr_node;
        }
        if (is_assign){
            IdentNode ident_node = {.ident = ident};
            size_t identifier = push_node(ident_node);
            AssignNode assign = {.identifier = identifier, .value = expression, .annotation = annotations};
            factor = assign;
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
