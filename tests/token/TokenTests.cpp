#include <token/Token.hpp>
#include <gtest/gtest.h>
#include <cstdint>

struct TokenTests : public ::testing::Test {
    void expectTokenAttributes(
        const Token& t,
        Token::Type expectedType, 
        std::uint32_t line,
        std::uint32_t col) {

        EXPECT_EQ(t.getType(), expectedType);
        EXPECT_EQ(t.getLine(), line);
        EXPECT_EQ(t.getColumn(), col);
    }
};

TEST_F(TokenTests, CreatesIntegerTokenCorrectly) {
    std::int32_t val = 123;
    Token sut(Token::Type::Int, 1ull, 5ull, val);

    expectTokenAttributes(sut, Token::Type::Int, 1ull, 5ull);
    ASSERT_TRUE(sut.valueIs<std::int32_t>());
    EXPECT_EQ(sut.getValue<std::int32_t>(), val);
}

TEST_F(TokenTests, CreatesDoubleTokenCorrectly) {
    double val = 3.14;
    Token sut(Token::Type::Float, 2ull, 7ull, val);

    expectTokenAttributes(sut, Token::Type::Float, 2ull, 7ull);
    ASSERT_TRUE(sut.valueIs<double>());
    EXPECT_DOUBLE_EQ(sut.getValue<double>(), val);
}

TEST_F(TokenTests, IdentifierHasNoNumericValueByDefault) {
    std::string ident{"foo"};
    Token sut(Token::Type::Ident, 3ull, 1ull, std::string{ident});

    expectTokenAttributes(sut, Token::Type::Ident, 3ull, 1ull);
    EXPECT_TRUE(sut.valueIs<std::string>());
    EXPECT_EQ(sut.getValue<std::string>(), ident);
}

TEST_F(TokenTests, EofTokenHandlesEmptyText) {
    Token sut(Token::Type::Eof, 10ull, 0ull);
    
    EXPECT_EQ(sut.getType(), Token::Type::Eof);
}
