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
        "        pump_it counter...\n"
        "\n"
        "        perhaps (counter bigger_ish 3) {\n" // line 40
        "            rage_quit!!!\n"
        "        }\n"
        "    }\n"
        "\n"
        "    stash n about calculate_stuff(10, 20)...\n"
        "    spin_around (n) {\n"
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

    // Line 1: psst: very useful thingy (comment - skipped)
    // Line 2: gig calculate_stuff (x, y) {
    size_t i = 0;
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Func);      // gig
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // calculate_stuff
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LParen);    // (
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // x
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Comma);     // ,
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // y
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RParen);    // )
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LBrace);    // {
    
    // Line 3: yeet 2 with 2 without 2...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Return);    // yeet
    EXPECT_EQ(tokens[i].getType(), Token::Type::Int);         // 2
    const auto idxInt2v1 = i++;
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Plus);      // with
    EXPECT_EQ(tokens[i].getType(), Token::Type::Int);         // 2
    const auto idxInt2v2 = i++;
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Minus);     // without
    EXPECT_EQ(tokens[i].getType(), Token::Type::Int);         // 2
    const auto idxInt2v3 = i++;
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 4: }
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RBrace);    // }
    
    // Line 6: psst: This is the start of the mess (comment - skipped)
    // Line 7: gig macho() {
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Func);      // gig
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // macho
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LParen);    // (
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RParen);    // )
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LBrace);    // {
    
    // Line 8: stash number about 10...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Var);       // stash
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // number
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Assign);    // about
    EXPECT_EQ(tokens[i].getType(), Token::Type::Int);         // 10
    const auto idxInt10 = i++;
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 9: stash isNumberTen about number looks_like 11...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Var);       // stash
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // isNumberTen
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Assign);    // about
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // number
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Equal);     // looks_like
    EXPECT_EQ(tokens[i].getType(), Token::Type::Int);         // 11
    const auto idxInt11 = i++;
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 11: perhaps (isNumberTen looks_like totally) {
    EXPECT_EQ(tokens[i++].getType(), Token::Type::If);        // perhaps
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LParen);    // (
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // isNumberTen
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Equal);     // looks_like
    EXPECT_EQ(tokens[i++].getType(), Token::Type::True);     // totally
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RParen);    // )
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LBrace);    // {
    
    // Line 12: scream: "The number is is ten"...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Print);     // scream
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Colon);     // :
    EXPECT_EQ(tokens[i++].getType(), Token::Type::String);    // "The number is is ten"
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 13: }
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RBrace);    // }
    
    // Line 14: or_whatever (isNumberTen looks_like nah) {
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Elif);      // or_whatever
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LParen);    // (
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // isNumberTen
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Equal);     // looks_like
    EXPECT_EQ(tokens[i++].getType(), Token::Type::False);     // nah
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RParen);    // )
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LBrace);    // {
    
    // Line 15: scream: "The number is not ten"...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Print);     // scream
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Colon);     // :
    EXPECT_EQ(tokens[i++].getType(), Token::Type::String);    // "The number is not ten"
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 16: }
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RBrace);    // }
    
    // Line 17: screw_it {
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Else);      // screw_it
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LBrace);    // {
    
    // Line 18: scream: "How the fck did I get here"...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Print);     // scream
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Colon);     // :
    EXPECT_EQ(tokens[i++].getType(), Token::Type::String);    // "How the fck did I get here"
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 19: }
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RBrace);    // }
    
    // Line 21: stash floatingNumber about 11.0...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Var);       // stash
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // floatingNumber
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Assign);    // about
    EXPECT_EQ(tokens[i].getType(), Token::Type::Float);       // 11.0
    const auto idxFloat11 = i++;
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 22: perhaps (floatingNumber looks_like 10.0) {
    EXPECT_EQ(tokens[i++].getType(), Token::Type::If);        // perhaps
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LParen);    // (
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // floatingNumber
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Equal);     // looks_like
    EXPECT_EQ(tokens[i].getType(), Token::Type::Float);       // 10.0
    const auto idxFloat10 = i++;
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RParen);    // )
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LBrace);    // {
    
    // Line 23: scream: "The floatingNumber is ten"...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Print);     // scream
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Colon);     // :
    EXPECT_EQ(tokens[i++].getType(), Token::Type::String);    // "The floatingNumber is ten"
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 24: }
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RBrace);    // }
    
    // Line 25: or_whatever (floatingNumber kinda_sus 20.0) {
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Elif);      // or_whatever
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LParen);    // (
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // floatingNumber
    EXPECT_EQ(tokens[i++].getType(), Token::Type::NotEqual);  // kinda_sus
    EXPECT_EQ(tokens[i].getType(), Token::Type::Float);       // 20.0
    const auto idxFloat20 = i++;
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RParen);    // )
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LBrace);    // {
    
    // Line 26: scream: "The floatingNumber is not 20"...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Print);     // scream
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Colon);     // :
    EXPECT_EQ(tokens[i++].getType(), Token::Type::String);    // "The floatingNumber is not 20"
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 27: }
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RBrace);    // }
    
    // Line 28: or_whatever (floatingNumber tiny_ish 5.0) {
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Elif);      // or_whatever
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LParen);    // (
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // floatingNumber
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Less);      // tiny_ish
    EXPECT_EQ(tokens[i].getType(), Token::Type::Float);       // 5.0
    const auto idxFloat5 = i++;
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RParen);    // )
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LBrace);    // {
    
    // Line 29: scream: "The floatingNumber is smaller than 5"...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Print);     // scream
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Colon);     // :
    EXPECT_EQ(tokens[i++].getType(), Token::Type::String);    // "The floatingNumber is smaller than 5"
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 30: }
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RBrace);    // }
    
    // Line 31: screw_it {
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Else);      // screw_it
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LBrace);    // {
    
    // Line 32: scream: "This language is so weird"...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Print);     // scream
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Colon);     // :
    EXPECT_EQ(tokens[i++].getType(), Token::Type::String);    // "This language is so weird"
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 33: }
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RBrace);    // }
    
    // Line 35: stash counter about 0...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Var);       // stash
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // counter
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Assign);    // about
    EXPECT_EQ(tokens[i].getType(), Token::Type::Int);         // 0
    const auto idxInt0 = i++;
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 36: do_until_bored {
    EXPECT_EQ(tokens[i++].getType(), Token::Type::While);     // do_until_bored
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LBrace);    // {
    
    // Line 37: scream: counter...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Print);     // scream
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Colon);     // :
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // counter
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 38: pump_it counter...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Incr);      // pump_it
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // counter
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 40: perhaps (counter bigger_ish 3) {
    EXPECT_EQ(tokens[i++].getType(), Token::Type::If);        // perhaps
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LParen);    // (
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // counter
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Greater);   // bigger_ish
    EXPECT_EQ(tokens[i].getType(), Token::Type::Int);         // 3
    const auto idxInt3 = i++;
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RParen);    // )
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LBrace);    // {
    
    // Line 41: rage_quit!!!
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Break);     // rage_quit
    EXPECT_EQ(tokens[i++].getType(), Token::Type::BrSemi);    // !!!
    
    // Line 42: }
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RBrace);    // }
    
    // Line 43: }
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RBrace);    // }
    
    // Line 45: stash n about calculate_stuff(10, 20)...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Var);       // stash
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // n
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Assign);    // about
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // calculate_stuff
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LParen);    // (
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Int);       // 10
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Comma);     // ,
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Int);       // 20
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RParen);    // )
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 46: spin_around (n) times {
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Repeat);    // spin_around
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LParen);    // (
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Ident);     // n
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RParen);    // )
    EXPECT_EQ(tokens[i++].getType(), Token::Type::LBrace);    // {
    
    // Line 47: scream: "Spinnin"...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Print);     // scream
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Colon);     // :
    EXPECT_EQ(tokens[i++].getType(), Token::Type::String);    // "Spinnin"
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 48: }
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RBrace);    // }
    
    // Line 50: yeet ghosted...
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Return);    // yeet
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Null);      // ghosted
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Semi);      // ...
    
    // Line 51: }
    EXPECT_EQ(tokens[i++].getType(), Token::Type::RBrace);    // }
    
    EXPECT_EQ(tokens[i++].getType(), Token::Type::Eof);       // \0

    // Block comment (rant_stop ... rant_start) - skipped
    
    EXPECT_EQ(i, tokens.size()) << "Expected " << i << " tokens, but got " << tokens.size();

    EXPECT_TRUE(tokens[idxInt2v1].valueIs<std::int32_t>());
    EXPECT_EQ(tokens[idxInt2v1].getValue<std::int32_t>(), 2);
    
    EXPECT_TRUE(tokens[idxInt2v2].valueIs<std::int32_t>());
    EXPECT_EQ(tokens[idxInt2v2].getValue<std::int32_t>(), 2);
    
    EXPECT_TRUE(tokens[idxInt2v3].valueIs<std::int32_t>());
    EXPECT_EQ(tokens[idxInt2v3].getValue<std::int32_t>(), 2);
    
    EXPECT_TRUE(tokens[idxInt10].valueIs<std::int32_t>());
    EXPECT_EQ(tokens[idxInt10].getValue<std::int32_t>(), 10);
    
    EXPECT_TRUE(tokens[idxInt11].valueIs<std::int32_t>());
    EXPECT_EQ(tokens[idxInt11].getValue<std::int32_t>(), 11);
    
    EXPECT_TRUE(tokens[idxFloat10].valueIs<double>());
    EXPECT_EQ(tokens[idxFloat10].getValue<double>(), 10.0);
    
    EXPECT_TRUE(tokens[idxFloat11].valueIs<double>());
    EXPECT_EQ(tokens[idxFloat11].getValue<double>(), 11.0);
    
    EXPECT_TRUE(tokens[idxFloat20].valueIs<double>());
    EXPECT_EQ(tokens[idxFloat20].getValue<double>(), 20.0);
    
    EXPECT_TRUE(tokens[idxFloat5].valueIs<double>());
    EXPECT_EQ(tokens[idxFloat5].getValue<double>(), 5.0);
    
    EXPECT_TRUE(tokens[idxInt0].valueIs<std::int32_t>());
    EXPECT_EQ(tokens[idxInt0].getValue<std::int32_t>(), 0);
    
    EXPECT_TRUE(tokens[idxInt3].valueIs<std::int32_t>());
    EXPECT_EQ(tokens[idxInt3].getValue<std::int32_t>(), 3);
}
