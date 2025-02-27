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
};

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KW_ONCE:         return "KW_ONCE";
        case TokenType::KW_UPON:         return "KW_UPON";
        case TokenType::KW_A:            return "KW_A";
        case TokenType::KW_TIME:         return "KW_TIME";
        case TokenType::IDENTIFIER:      return "IDENTIFIER";
        case TokenType::VERB:            return "VERB";
        case TokenType::NOUN:            return "NOUN";
        case TokenType::ADJECTIVE:       return "ADJECTIVE";
        case TokenType::PERIOD:          return "PERIOD";
        case TokenType::KW_END:          return "KW_END";
        case TokenType::STRING:          return "STRING";
        case TokenType::KW_IF:           return "KW_IF";
        case TokenType::KW_THEN:         return "KW_THEN";
        case TokenType::KW_ELSE:         return "KW_ELSE";
        case TokenType::KW_ENDIF:        return "KW_ENDIF";
        case TokenType::KW_CHOOSE:       return "KW_CHOOSE";
        case TokenType::END_OF_FILE:     return "EOF";
        case TokenType::KW_WHEN:         return "KW_WHEN";
        case TokenType::KW_DURING:       return "KW_DURING";
        case TokenType::KW_OTHERWISE:    return "KW_OTHERWISE";
        case TokenType::KW_INCREASED:    return "KW_INCREASED";
        case TokenType::KW_BY:           return "KW_BY";
        case TokenType::KW_TOTAL:        return "KW_TOTAL";
        case TokenType::KW_OF:           return "KW_OF";
        case TokenType::TAG:             return "TAG";
        case TokenType::KW_END_OF_STORY: return "KW_END_OF_STORY";
        case TokenType::KW_RANDOM:       return "KW_RANDOM";
        case TokenType::KW_UNCERTAIN:    return "KW_UNCERTAIN";
        default:                         return "UNKNOWN";
    }
}

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

#endif // TOKEN_HPP
