#include <memory>
#include <vector>
#include <gtest/gtest.h>
#include <lexer/Lexer.hpp>

struct LexerTests : public ::testing::Test {
    std::unique_ptr<Lexer> sut;

    std::unique_ptr<Lexer> makeSut(std::string source) {
        return std::make_unique<Lexer>(std::move(source));
    }

    std::vector<Token> tokenizeAll() {
        if (not sut) {
            return std::vector<Token>();
        }

        std::vector<Token> tokens{};
        while (not sut->tokenizedAll()) {
            tokens.push_back(sut->getNextTokenAndAdvance());
        }
        return tokens;
    }
};

TEST_F(LexerTests, MainFunc) {
    sut = makeSut(
        "gig macho() {\n"
        "    yeet 0...\n"
        "}"
    );

    auto tokens = tokenizeAll();

    ASSERT_EQ(tokens.size(), 9);

    EXPECT_EQ(tokens[0].getType(), Token::Type::Func);      // gig
    EXPECT_EQ(tokens[1].getType(), Token::Type::Ident);     // macho
    EXPECT_EQ(tokens[2].getType(), Token::Type::LParen);    // (
    EXPECT_EQ(tokens[3].getType(), Token::Type::RParen);    // )
    EXPECT_EQ(tokens[4].getType(), Token::Type::LBrace);    // {
    EXPECT_EQ(tokens[5].getType(), Token::Type::Return);    // yeet
    EXPECT_EQ(tokens[6].getType(), Token::Type::Int);       // 0
    EXPECT_EQ(tokens[7].getType(), Token::Type::Semi);      // ...
    EXPECT_EQ(tokens[8].getType(), Token::Type::RBrace);    // }

    EXPECT_TRUE(tokens[6].valueIs<std::int32_t>());
    EXPECT_EQ(tokens[6].getValue<std::int32_t>(), 0);
}

TEST_F(LexerTests, LanguagePrototype) {
    sut = makeSut(
        "psst: very useful thingy\n"
        "gig calculate_stuff (x, y) {\n"
        "    yeet 2 with 2 without 2...\n"
        "}\n"
        "\n"
        "psst: This is the start of the mess\n"
        "gig macho() {\n"
        "    stash number about 10...\n"
        "    stash isNumberTen about number looks_like 11...\n"
        "\n" // line 10
        "    perhaps (isNumberTen looks_like totally) {\n"
        "        scream: \"The number is is ten\"...\n"
        "    }\n"
        "    or_whatever (isNumberTen looks_like nah) {\n"
        "        scream: \"The number is not ten\"...\n"
        "    }\n"
        "    screw_it {\n"
        "        scream: \"How the fck did I get here\"...\n"
        "    }\n"
        "\n" // line 20
        "    stash floatingNumber about 11.0...\n"
        "    perhaps (floatingNumber looks_like 10.0) {\n"
        "        scream: \"The floatingNumber is ten\"...\n"
        "    }\n"
        "    or_whatever (floatingNumber kinda_sus 20.0) {\n"
        "        scream: \"The floatingNumber is not 20\"...\n"
        "    }\n"
        "    or_whatever (floatingNumber tiny_ish 5.0) {\n"
        "        scream: \"The floatingNumber is smaller than 5\"...\n"
        "    }\n" // line 30
        "    screw_it {\n"
        "        scream: \"This language is so weird\"...\n"
        "    }\n"
        "\n"
        "    stash counter about 0...\n"
        "    do_until_bored {\n"
        "        scream: counter...\n"
        "        counter might_be (pump_it counter)...\n"
        "\n"
        "        perhaps (counter bigger_ish 3) {\n" // line 40
        "            rage_quit!!!\n"
        "        }\n"
        "    }\n"
        "\n"
        "    stash n about calculate_stuff(10, 20)...\n"
        "    spin_around (n) times {\n"
        "        scream: \"Spinnin\"...\n"
        "    }\n"
        "\n"
        "    yeet ghosted...\n" // line 50
        "}\n"
        "\n"
        "rant_stop\n"
        "    Output:\n"
        "    \n"
        "    THE NUMBER IS NOT TEN!!!\n"
        "    THE FLOATINGNUMBER IS NOT 20!!!\n"
        "    0!!!\n"
        "    1!!!\n"
        "    2!!!\n" // line 60
        "    3!!!\n"
        "    4!!!\n"
        "    SPINNIN!!!\n"
        "    SPINNIN!!!\n"
        "\n"
        "    Syntax:\n"
        "        Every statement must end with an ellipsis (...) to indicate hesitation.\n"
        "    Comments:\n"
        "        Single-line comments use 'psst:',\n"
        "        block comments are between 'rant_stop' and 'rant_start'.\n" // line 70
        "    Variables:\n"
        "        Declared using 'stash [name] about [value]' (dynamic typing).\n"
        "    Assignment:\n"
        "        Variable updates use 'might_be' instead of 'about'.\n"
        "    Types:\n"
        "        Booleans are 'totally' (true), 'nah' (false)\n"
        "        null is 'ghosted'.\n"
        "    Functions:\n"
        "        Defined using the 'gig' keyword and return values using 'yeet'.\n"
        "    Output:\n"
        "        'scream' prints arguments to console in UPPERCASE with appended '!!!'.\n"
        "    Conditionals:\n"
        "        Logic flow uses 'perhaps' (if), 'or_whatever' (else if), and 'screw_it' (else).\n"
        "    Loops:\n"
        "        Iteration implemented via 'do_until_bored' (while) and 'spin_around' (for).\n"
        "    Flow Control:\n"
        "        Loops are terminated aggressively using the 'rage_quit!!!' command.\n"
        "    Equality:\n"
        "        Comparisons use 'looks_like' (==) and 'kinda_sus' (!=).\n"
        "    Relational:\n"
        "        Size comparisons use 'bigger_ish' (>) and 'tiny_ish' (<).\n"
        "    Math:\n"
        "        Arithmetic uses 'with' (+) and 'without' (-)\n"
        "        The 'pump_it' operator is used for incrementing values.\n"
        "rant_start\n"
    );

    auto tokens = tokenizeAll();
}
