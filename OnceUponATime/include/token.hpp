// token.hpp

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <algorithm>

enum class TokenType {
    KW_ONCE,
    KW_UPON,
    KW_A,
    KW_TIME,
    IDENTIFIER,
    NUMBER,
    ADJECTIVE,
    NOUN,
    VERB,
    KW_END,
    PERIOD,
    STRING,
    KW_IF,
    KW_THEN,
    KW_ELSE,
    KW_ENDIF,
    KW_CHOOSE,
    END_OF_FILE,
    KW_WHEN,
    KW_DURING,
    KW_OTHERWISE,
    KW_INCREASED,
    KW_BY,
    KW_TOTAL,
    KW_OF,
    TAG,
    KW_END_OF_STORY,
    KW_RANDOM,
    KW_UNCERTAIN,
    KW_WHILE,
    KW_ENDWHILE,
    KW_FOR,
    KW_EACH,
    KW_DO,
    KW_ENDFOR,
    KW_DEFINE_FUNCTION,
    KW_ENDFUNCTION,
    KW_CALL,
    KW_RETURN,
    KW_HAS,
    KW_IS,
    KW_ADD,
    KW_SUBTRACT,
    KW_MULTIPLY,
    KW_DIVIDE,
    KW_EQUALS,
    KW_INPUT,
    KW_NARRATE,
    KW_TELL,
    KW_REMARK,
    KW_NOTE,
    KW_COMMENT,
    KW_IN,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    COMMA
};

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KW_ONCE:            return "KW_ONCE";
        case TokenType::KW_UPON:            return "KW_UPON";
        case TokenType::KW_A:               return "KW_A";
        case TokenType::KW_TIME:            return "KW_TIME";
        case TokenType::IDENTIFIER:         return "IDENTIFIER";
        case TokenType::NUMBER:             return "NUMBER";
        case TokenType::ADJECTIVE:          return "ADJECTIVE";
        case TokenType::NOUN:               return "NOUN";
        case TokenType::VERB:               return "VERB";
        case TokenType::KW_END:             return "KW_END";
        case TokenType::PERIOD:             return "PERIOD";
        case TokenType::STRING:             return "STRING";
        case TokenType::KW_IF:              return "KW_IF";
        case TokenType::KW_THEN:            return "KW_THEN";
        case TokenType::KW_ELSE:            return "KW_ELSE";
        case TokenType::KW_ENDIF:           return "KW_ENDIF";
        case TokenType::KW_CHOOSE:          return "KW_CHOOSE";
        case TokenType::END_OF_FILE:        return "END_OF_FILE";
        case TokenType::KW_WHEN:            return "KW_WHEN";
        case TokenType::KW_DURING:          return "KW_DURING";
        case TokenType::KW_OTHERWISE:       return "KW_OTHERWISE";
        case TokenType::KW_INCREASED:       return "KW_INCREASED";
        case TokenType::KW_BY:              return "KW_BY";
        case TokenType::KW_TOTAL:           return "KW_TOTAL";
        case TokenType::KW_OF:              return "KW_OF";
        case TokenType::TAG:                return "TAG";
        case TokenType::KW_END_OF_STORY:    return "KW_END_OF_STORY";
        case TokenType::KW_RANDOM:          return "KW_RANDOM";
        case TokenType::KW_UNCERTAIN:       return "KW_UNCERTAIN";
        case TokenType::KW_WHILE:           return "KW_WHILE";
        case TokenType::KW_ENDWHILE:        return "KW_ENDWHILE";
        case TokenType::KW_FOR:             return "KW_FOR";
        case TokenType::KW_EACH:            return "KW_EACH";
        case TokenType::KW_DO:              return "KW_DO";
        case TokenType::KW_ENDFOR:          return "KW_ENDFOR";
        case TokenType::KW_DEFINE_FUNCTION: return "KW_DEFINE_FUNCTION";
        case TokenType::KW_ENDFUNCTION:     return "KW_ENDFUNCTION";
        case TokenType::KW_CALL:            return "KW_CALL";
        case TokenType::KW_RETURN:          return "KW_RETURN";
        case TokenType::KW_HAS:             return "KW_HAS";
        case TokenType::KW_IS:              return "KW_IS";
        case TokenType::KW_ADD:             return "KW_ADD";
        case TokenType::KW_SUBTRACT:        return "KW_SUBTRACT";
        case TokenType::KW_MULTIPLY:        return "KW_MULTIPLY";
        case TokenType::KW_DIVIDE:          return "KW_DIVIDE";
        case TokenType::KW_EQUALS:          return "KW_EQUALS";
        case TokenType::KW_INPUT:           return "KW_INPUT";
        case TokenType::KW_NARRATE:         return "KW_NARRATE";
        case TokenType::KW_TELL:            return "KW_TELL";
        case TokenType::KW_REMARK:          return "KW_REMARK";
        case TokenType::KW_NOTE:            return "KW_NOTE";
        case TokenType::KW_COMMENT:         return "KW_COMMENT";
        case TokenType::KW_IN:              return "KW_IN";
        case TokenType::LEFT_BRACKET:       return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET:      return "RIGHT_BRACKET";
        case TokenType::COMMA:              return "COMMA";
        default:                          return "UNKNOWN";
    }
}

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

#endif
