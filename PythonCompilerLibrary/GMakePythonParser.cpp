#include "GMakePythonParser.h"

const size_t INVALID = std::numeric_limits<size_t>::max();

bool keyword_check(const token& token, const KeyWord& keyword){
    if (token.type != TokenType::KEYWORD){
        return false;
    }
    if (std::get<KeyWord>(token.value) == keyword){
        return true;
    }
    return false;
}

void Parser::reset_parser(){
    Node_pos = 0;
    AST = {};
    IndentLevel = 0;
    Tokens = {};
    TokenPos = 0;
}

std::vector<Node> Parser::parse_code(){
    ProgramNode program = {.lines = {}};
    while (peek_token().type != TokenType::EMPTY){
        size_t ast_node = parse_statement();
        if (ast_node == INVALID){}
        program.lines.push_back(ast_node);
    }
    push_node(program);
    std::vector<Node> local_ast = AST;
    reset_parser();
    return local_ast;
}

size_t Parser::parse_body(){
    BodyNode body = {.lines = {}};
    while (peek_token().type != TokenType::EMPTY){
        size_t ast_node = parse_statement();
        if (ast_node == INVALID){}
        body.lines.push_back(ast_node);
    }
    size_t body_node = push_node(body);
    return body_node;
}

size_t Parser::parse_statement(){
    size_t statement_node = INVALID;
    if (keyword_check(peek_token(), KeyWord::FROM) || keyword_check(peek_token(), KeyWord::IMPORT)){
        statement_node = parse_import();
    }
    else if (keyword_check(peek_token(), KeyWord::CLASS)){
        get_token();

    }
    else if (peek_token().type == TokenType::IDENT){
        size_t asignment = parse_assignment();
        if (asignment == INVALID){
            statement_node = parse_expression();
        }
    }
    return statement_node;
}

size_t Parser::parse_class(){
    ClassNode class_node = {};
    token tok = expect_token(TokenType::IDENT);
    class_node.name = std::get<std::string>(tok.value);
    expect_token(TokenType::LPAREN);
    class_node.args = parse_arguments();
    class_node.body = parse_body();
    size_t class_node_index = push_node(class_node);
    return class_node_index;
}

std::vector<size_t> Parser::parse_arguments(){
    std::vector<size_t> argument_nodes = {};
    size_t identifier_node = parse_identifier();
    argument_nodes.push_back(identifier_node);
    while (peek_token().type == TokenType::COMMA){
        get_token();
        size_t ident_node = parse_identifier();
        argument_nodes.push_back(ident_node);
    }
    return argument_nodes;
}

size_t Parser::parse_assignment(){
    AssignNode assign_node = {};
    size_t ident = parse_identifier();
    assign_node.identifier = ident;
    if (peek_token().type == TokenType::COLON){
        //@TODO: parse annotations
        throw std::runtime_error("not implemented yet");
    }
    else{
        assign_node.annotation = INVALID;
    }
    if (peek_token().type == TokenType::EQUAL){
        assign_node.value = parse_expression();
    }
    else{
        return INVALID;
    }
    size_t assign_node_inex = push_node(assign_node);
    return assign_node_inex;
}

size_t Parser::parse_expression(){
    size_t expression_node = INVALID;
    expression_node = parse_factor();
    return expression_node;
}

size_t Parser::parse_import(){
    ImportNode import_node = {};
    bool is_not_atribute_allowed = true;
    if (keyword_check(peek_token(), KeyWord::IMPORT)){
        get_token();
        import_node.from = INVALID;
        token tok = expect_token(TokenType::KEYWORD);
        keyword_check(tok, KeyWord::IMPORT);
        import_node.import.push_back(parse_identifier(is_not_atribute_allowed));
        if (keyword_check(peek_token(), KeyWord::AS)){
            get_token();
            import_node.alias = parse_identifier(is_not_atribute_allowed);
        }
        while (peek_token().type == TokenType::COMMA){
            get_token();
            import_node.import.push_back(parse_identifier(is_not_atribute_allowed));
            if (keyword_check(peek_token(), KeyWord::AS)){
                get_token();
                import_node.alias = parse_identifier(is_not_atribute_allowed);
            }
        }
    }
    else{
        get_token();
        import_node.from = parse_identifier();
        token tok = expect_token(TokenType::KEYWORD);
        keyword_check(tok, KeyWord::IMPORT);
        import_node.import.push_back(parse_identifier(is_not_atribute_allowed));
        if (keyword_check(peek_token(), KeyWord::AS)){
            get_token();
            import_node.alias = parse_identifier(is_not_atribute_allowed);
        }
        while (peek_token().type == TokenType::COMMA){
            get_token();
            import_node.import.push_back(parse_identifier());
            if (keyword_check(peek_token(), KeyWord::AS)){
                get_token();
                import_node.alias = parse_identifier(is_not_atribute_allowed);
            }
        }
    }
    size_t import_node_index = push_node(import_node);
    return import_node_index;
}

size_t Parser::parse_identifier(bool is_not_atribute_allowed){
    IdentNode ident_node = {};
    token tok = expect_token(TokenType::IDENT);
    size_t ident_node_index = push_node(ident_node);
    bool is_atribute = false;
    while (peek_token().type == TokenType::DOT){
        is_atribute = true;
    }
    if (is_atribute && is_not_atribute_allowed){
        throw std::runtime_error("this is not a valid token");
    }
    return ident_node_index;
}

size_t Parser::parse_factor(){
    size_t factor_node = INVALID;
    if (peek_token().type == TokenType::NUMBER){
        NumberNode number_node = {};
        token tok = get_token();
        std::string tok_value = std::get<std::string>(tok.value);
        bool is_floating_point = false;
        if (tok_value.contains(".")){
            is_floating_point = true;
        }
        std::string tok_value_copy = std::string();
        for (char c : tok_value){
            if (c != '_'){
                tok_value_copy.push_back(c);
            }
        }
        tok_value = tok_value_copy;
        if (is_floating_point){
            number_node.number = std::stod(tok_value);
        }
        else{
            number_node.number = std::stoll(tok_value);
        }
        factor_node = push_node(number_node);
    }
    else if (peek_token().type == TokenType::IDENT){
        factor_node = parse_identifier();
    }
    else{
        factor_node = INVALID;
        throw std::runtime_error("this is not a valid token");
    }
    return factor_node;
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
    if (TokenPos + look_ahead >= Tokens.size()){
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

token Parser::expect_token(TokenType type){
    bool is_expected = match_token(type);
    if (!is_expected){
        throw std::runtime_error("this is not a valid token");
    }
    return get_token();
}