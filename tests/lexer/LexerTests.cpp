#include <memory>
#include <initializer_list>
#include <vector>
#include <gtest/gtest.h>
#include <lexer/Lexer.hpp>

struct LexerTests : public ::testing::Test {
    std::unique_ptr<Lexer> sut;

    std::unique_ptr<Lexer> makeSut(std::string source) {
        constexpr FileId dummyFileId{0ull};
        return std::make_unique<Lexer>(std::move(source), dummyFileId);
    }

    LexerResult tokenize(std::string source) {
        sut = makeSut(std::move(source));
        return sut->tokenize();
    }

    std::vector<Token> tokenizeOk(std::string source) {
        const auto result = tokenize(std::move(source));
        EXPECT_TRUE(result.errors.empty());

        return result.tokens;
    }

    void expectHasErrors(const LexerResult& result) {
        EXPECT_FALSE(result.errors.empty());
    }

    void expectNoTokens(const LexerResult& result) {
        EXPECT_TRUE(result.tokens.empty());
    }

    void expectSingleError(const LexerResult& result, LexerErrorType expectedType) {
        ASSERT_EQ(result.errors.size(), 1);
        EXPECT_EQ(result.errors[0].type, expectedType);
    }

    void expectErrorAt(
        const LexerResult& result,
        size_t index,
        LexerErrorType expectedType,
        std::uint32_t expectedLine,
        std::uint32_t expectedColumn,
        std::uint32_t expectedLength) {
    
        ASSERT_LT(index, result.errors.size());
        const auto& error = result.errors[index];
        EXPECT_EQ(error.type, expectedType);
        EXPECT_EQ(error.line, expectedLine);
        EXPECT_EQ(error.column, expectedColumn);
        EXPECT_EQ(error.length, expectedLength);
    }

    void expectTypes(
        const std::vector<Token>& tokens,
        std::initializer_list<Token::Type> expectedTypes) {

        ASSERT_EQ(tokens.size(), expectedTypes.size());

        size_t index = 0;
        for (const auto expectedType : expectedTypes) {
            EXPECT_EQ(tokens[index].getType(), expectedType) << "Token mismatch at index: " << index;
            ++index;
        }
    }

    void expectTypes(std::string source, std::initializer_list<Token::Type> expectedTypes) {
        expectTypes(tokenizeOk(std::move(source)), expectedTypes);
    }

    template <typename TValue>
    void expectValue(const std::vector<Token>& tokens, size_t index, const TValue& expectedValue) {
        ASSERT_LT(index, tokens.size());
        EXPECT_TRUE(tokens[index].valueIs<TValue>());
        EXPECT_EQ(tokens[index].getValue<TValue>(), expectedValue);
    }
};

TEST_F(LexerTests, MainFunc) {
    const auto tokens = tokenizeOk("gig macho() { yeet 0... }");

    expectTypes(tokens, {
        Token::Type::Func, Token::Type::Ident, Token::Type::LParen, Token::Type::RParen,
        Token::Type::LBrace, Token::Type::Return, Token::Type::Int, Token::Type::Semi, Token::Type::RBrace
    });
    expectValue<std::int32_t>(tokens, 6, 0);
}

TEST_F(LexerTests, DotTokenizing) {
    const auto tokens = tokenizeOk("gossip.spill_tea(\"Hello\")...");

    expectTypes(tokens, {
        Token::Type::Ident, Token::Type::Dot, Token::Type::Ident, Token::Type::LParen,
        Token::Type::String, Token::Type::RParen, Token::Type::Semi
    });
    expectValue<std::string>(tokens, 0, "gossip");
    expectValue<std::string>(tokens, 2, "spill_tea");
    expectValue<std::string>(tokens, 4, "Hello");
}

TEST_F(LexerTests, ImportModule) {
    const auto tokens = tokenizeOk("summon gossip...");

    expectTypes(tokens, {Token::Type::Import, Token::Type::Ident, Token::Type::Semi});
    expectValue<std::string>(tokens, 1, "gossip");
}

TEST_F(LexerTests, VectorDefinition) {
    const auto tokens = tokenizeOk("stash list about [11, 22, 33]...");

    expectTypes(tokens, {
        Token::Type::Var, Token::Type::Ident, Token::Type::Assign, Token::Type::LBracket,
        Token::Type::Int, Token::Type::Comma, Token::Type::Int, Token::Type::Comma,
        Token::Type::Int, Token::Type::RBracket, Token::Type::Semi
    });
    expectValue<std::string>(tokens, 1, "list");
    expectValue<std::int32_t>(tokens, 4, 11);
    expectValue<std::int32_t>(tokens, 6, 22);
    expectValue<std::int32_t>(tokens, 8, 33);
}

TEST_F(LexerTests, AndAlso) {
    expectTypes("also", {Token::Type::And});
}

TEST_F(LexerTests, OrEither) {
    expectTypes("either", {Token::Type::Or});
}

TEST_F(LexerTests, PrototypeCommentsAreSkipped) {
    expectTypes("psst: useful\ngig macho() { yeet 0... }", {
        Token::Type::Func, Token::Type::Ident, Token::Type::LParen, Token::Type::RParen,
        Token::Type::LBrace, Token::Type::Return, Token::Type::Int, Token::Type::Semi, Token::Type::RBrace
    });
}

TEST_F(LexerTests, PrototypeBlockCommentIsSkipped) {
    expectTypes("stash x about 1...\nrant_stop\nignored text\nrant_start\nstash y about 2...", {
        Token::Type::Var, Token::Type::Ident, Token::Type::Assign, Token::Type::Int, Token::Type::Semi,
        Token::Type::Var, Token::Type::Ident, Token::Type::Assign, Token::Type::Int, Token::Type::Semi
    });
}

TEST_F(LexerTests, PrototypeFunctionWithMathOperators) {
    const auto tokens = tokenizeOk("gig calculate_stuff(x, y) { yeet 2 with 2 without 2... }");

    expectTypes(tokens, {
        Token::Type::Func, Token::Type::Ident, Token::Type::LParen, Token::Type::Ident, Token::Type::Comma,
        Token::Type::Ident, Token::Type::RParen, Token::Type::LBrace, Token::Type::Return, Token::Type::Int,
        Token::Type::Plus, Token::Type::Int, Token::Type::Minus, Token::Type::Int, Token::Type::Semi, Token::Type::RBrace
    });
    expectValue<std::int32_t>(tokens, 9, 2);
    expectValue<std::int32_t>(tokens, 11, 2);
    expectValue<std::int32_t>(tokens, 13, 2);
}

TEST_F(LexerTests, PrototypeConditionalChainTokens) {
    expectTypes("perhaps (isNumberTen looks_like totally) {} or_whatever (isNumberTen looks_like nah) {} screw_it {}", {
        Token::Type::If, Token::Type::LParen, Token::Type::Ident, Token::Type::Equal, Token::Type::True,
        Token::Type::RParen, Token::Type::LBrace, Token::Type::RBrace,
        Token::Type::Elif, Token::Type::LParen, Token::Type::Ident, Token::Type::Equal, Token::Type::False,
        Token::Type::RParen, Token::Type::LBrace, Token::Type::RBrace,
        Token::Type::Else, Token::Type::LBrace, Token::Type::RBrace
    });
}

TEST_F(LexerTests, PrototypeFloatAssignmentTokenAndValue) {
    const auto tokens = tokenizeOk("stash floatingNumber about 11.0...");
    expectTypes(tokens, {Token::Type::Var, Token::Type::Ident, Token::Type::Assign, Token::Type::Float, Token::Type::Semi});
    expectValue<double>(tokens, 3, 11.0);
}

TEST_F(LexerTests, PrototypeNotEqualComparisonTokenAndValue) {
    const auto tokens = tokenizeOk("perhaps (floatingNumber kinda_sus 20.0) {}");
    expectTypes(tokens, {
        Token::Type::If, Token::Type::LParen, Token::Type::Ident, Token::Type::NotEqual,
        Token::Type::Float, Token::Type::RParen, Token::Type::LBrace, Token::Type::RBrace
    });
    expectValue<double>(tokens, 4, 20.0);
}

TEST_F(LexerTests, PrototypeLessComparisonTokenAndValue) {
    const auto tokens = tokenizeOk("perhaps (floatingNumber tiny_ish 5.0) {}");
    expectTypes(tokens, {
        Token::Type::If, Token::Type::LParen, Token::Type::Ident, Token::Type::Less,
        Token::Type::Float, Token::Type::RParen, Token::Type::LBrace, Token::Type::RBrace
    });
    expectValue<double>(tokens, 4, 5.0);
}

TEST_F(LexerTests, PrototypeLoopIncrAndBreakTokens) {
    expectTypes("do_until_bored { pump_it counter... perhaps (counter bigger_ish 3) { rage_quit!!! } }", {
        Token::Type::Loop, Token::Type::LBrace,
        Token::Type::Incr, Token::Type::Ident, Token::Type::Semi,
        Token::Type::If, Token::Type::LParen, Token::Type::Ident, Token::Type::Greater, Token::Type::Int,
        Token::Type::RParen, Token::Type::LBrace, Token::Type::Break, Token::Type::BrSemi,
        Token::Type::RBrace, Token::Type::RBrace
    });
}

TEST_F(LexerTests, PrototypeFunctionCallAssignmentValues) {
    const auto tokens = tokenizeOk("stash n about calculate_stuff(10, 20)...");
    expectTypes(tokens, {
        Token::Type::Var, Token::Type::Ident, Token::Type::Assign, Token::Type::Ident,
        Token::Type::LParen, Token::Type::Int, Token::Type::Comma, Token::Type::Int,
        Token::Type::RParen, Token::Type::Semi
    });
    expectValue<std::int32_t>(tokens, 5, 10);
    expectValue<std::int32_t>(tokens, 7, 20);
}

TEST_F(LexerTests, PrototypeRepeatLoopAndCallTokens) {
    expectTypes("spin_around (n) { gossip.spill_tea(\"Spinnin\")... }", {
        Token::Type::Repeat, Token::Type::LParen, Token::Type::Ident, Token::Type::RParen,
        Token::Type::LBrace, Token::Type::Ident, Token::Type::Dot, Token::Type::Ident,
        Token::Type::LParen, Token::Type::String, Token::Type::RParen, Token::Type::Semi, Token::Type::RBrace
    });
}

TEST_F(LexerTests, PrototypeNullReturnTokens) {
    expectTypes("yeet ghosted...", {Token::Type::Return, Token::Type::Null, Token::Type::Semi});
}

TEST_F(LexerTests, ReassignKeywordTokenizing) {
    expectTypes("x might_be 42...", {
        Token::Type::Ident, Token::Type::Reassign, Token::Type::Int, Token::Type::Semi
    });
}

TEST_F(LexerTests, ColonSingleCharTokenizing) {
    expectTypes("foo: bar", {
        Token::Type::Ident, Token::Type::Colon, Token::Type::Ident
    });
}

TEST_F(LexerTests, EmptySourceProducesError) {
    const auto result = tokenize("");
    expectSingleError(result, LexerErrorType::EmptySource);
    expectErrorAt(result, 0ull, LexerErrorType::EmptySource, 1u, 1u, 1u);
    expectNoTokens(result);
}

TEST_F(LexerTests, WhitespaceOnlySourceProducesError) {
    const auto result = tokenize("  \t\n\n   \t");
    expectSingleError(result, LexerErrorType::EmptySource);
    expectErrorAt(result, 0ull, LexerErrorType::EmptySource, 1u, 1u, 1u);
    expectNoTokens(result);
}

TEST_F(LexerTests, CommentsOnlySourceProducesEmptySourceError) {
    const auto result = tokenize(
        "psst: hello\n"
        "   \n"
        "rant_stop\n"
        "ignored\n"
        "rant_start\n"
        "\t"
    );

    expectSingleError(result, LexerErrorType::EmptySource);
    expectNoTokens(result);
}

TEST_F(LexerTests, UnterminatedStringBeforeNewLineProducesError) {
    const auto result = tokenize("\"hello\nstash x about 1...");

    expectHasErrors(result);
    expectErrorAt(result, 0ull, LexerErrorType::UnterminatedString, 1u, 2u, 5u);
    ASSERT_FALSE(result.tokens.empty());
    EXPECT_EQ(result.tokens[0].getType(), Token::Type::Var);
}

TEST_F(LexerTests, UnterminatedStringAtEndProducesError) {
    const auto result = tokenize("\"hello");

    expectSingleError(result, LexerErrorType::UnterminatedString);
    expectNoTokens(result);
}

TEST_F(LexerTests, UnterminatedBlockCommentProducesError) {
    const auto result = tokenize("rant_stop\nthis never ends");

    ASSERT_EQ(result.errors.size(), 2);
    expectErrorAt(result, 0ull, LexerErrorType::UnterminatedBlockComment, 1u, 1u, 9u);
    expectErrorAt(result, 1ull, LexerErrorType::EmptySource, 1u, 1u, 1u);
    expectNoTokens(result);
}

TEST_F(LexerTests, UnknownTokenProducesErrorAndContinuesLexing) {
    const auto result = tokenize("@ stash x about 1...");

    expectHasErrors(result);
    expectErrorAt(result, 0ull, LexerErrorType::UnknownToken, 1u, 1u, 1u);
    ASSERT_FALSE(result.tokens.empty());
    EXPECT_EQ(result.tokens[0].getType(), Token::Type::Var);
}

TEST_F(LexerTests, MultipleUnknownTokensProduceMultipleErrors) {
    const auto result = tokenize("@#");

    ASSERT_EQ(result.errors.size(), 2);
    expectErrorAt(result, 0ull, LexerErrorType::UnknownToken, 1u, 1u, 1u);
    expectErrorAt(result, 1ull, LexerErrorType::UnknownToken, 1u, 2u, 1u);
    expectNoTokens(result);
}

TEST_F(LexerTests, MixedWhitespaceBetweenStatements) {
    expectTypes("\tstash a about 1...\r\n  stash b about 2...", {
        Token::Type::Var, Token::Type::Ident, Token::Type::Assign, Token::Type::Int, Token::Type::Semi,
        Token::Type::Var, Token::Type::Ident, Token::Type::Assign, Token::Type::Int, Token::Type::Semi
    });
}

TEST_F(LexerTests, SequentialCommentsAreSkipped) {
    expectTypes(
        "psst: first\n"
        "psst: second\n"
        "rant_stop\n"
        "ignored\n"
        "rant_start\n"
        "psst: third\n"
        "stash x about 1...",
        {Token::Type::Var, Token::Type::Ident, Token::Type::Assign, Token::Type::Int, Token::Type::Semi}
    );
}

TEST_F(LexerTests, ExplicitNullCharTokenizesToEof) {
    const auto tokens = tokenizeOk(std::string("\0", 1));
    expectTypes(tokens, {Token::Type::Eof});
}
