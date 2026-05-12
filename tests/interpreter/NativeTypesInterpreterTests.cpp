#include "InterpreterTestsFixture.hpp"

TEST_F(InterpreterTests, StringToSolidConvertsStringToInt) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "11"...
            gossip.spill_tea(string.to_solid())...
        }
    )";

    expectOutput(source, "11\n");
}

TEST_F(InterpreterTests, StringToUpperCaseConvertsCorrectly) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "abc - def"...
            gossip.spill_tea(string.scream())...
        }
    )";

    expectOutput(source, "ABC - DEF\n");
}

TEST_F(InterpreterTests, StringToLowerCaseConvertsCorrectly) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "ABC - DEF"...
            gossip.spill_tea(string.mumble())...
        }
    )";

    expectOutput(source, "abc - def\n");
}

TEST_F(InterpreterTests, StringLengthReturnsCorrectInt) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "abc"...
            gossip.spill_tea(string.yap_level())...
        }
    )";

    expectOutput(source, "3\n");
}

TEST_F(InterpreterTests, EmptyStringLengthReturnsZero) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about ""...
            gossip.spill_tea(string.yap_level())...
        }
    )";

    expectOutput(source, "0\n");
}

TEST_F(InterpreterTests, EmptyStringIsEmptyReturnsTrue) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about ""...
            gossip.spill_tea(string.speechless())...
        }
    )";

    expectOutput(source, "totally\n");
}

TEST_F(InterpreterTests, NotEmptyStringIsEmptyReturnsTrue) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "abc"...
            gossip.spill_tea(string.speechless())...
        }
    )";

    expectOutput(source, "nah\n");
}

TEST_F(InterpreterTests, StringTrimConvertsToTrimmedFromBothSides) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "  .abc - def/  "...
            gossip.spill_tea(string.cut_the_crap())...
        }
    )";

    expectOutput(source, ".abc - def/\n");
}

TEST_F(InterpreterTests, StringSubstringConvertCorrectlyWhenIndexInBounds) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "0123456789abcdefghij"...
            gossip.spill_tea(string.chop(10))...
        }
    )";

    expectOutput(source, "abcdefghij\n");
}

TEST_F(InterpreterTests, StringSubstringWithTwoArgsConvertsCorrectlyWhenInBounds) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "0123456789abcdefghij"...
            gossip.spill_tea(string.chop(10, 3))...
        }
    )";

    expectOutput(source, "abc\n");
}

TEST_F(InterpreterTests, StringSubstringWithTwoArgsConvertsTrimsAllLeftWhenCountIsOutOfBounds) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "0123456789abcdefghij"...
            gossip.spill_tea(string.chop(10, 100))...
        }
    )";

    expectOutput(source, "abcdefghij\n");
}

TEST_F(InterpreterTests, StringConcatAppendsOneString) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "ab"...
            gossip.spill_tea(string.keep_yapping("cd"))...
        }
    )";

    expectOutput(source, "abcd\n");
}

TEST_F(InterpreterTests, StringConcatAppendsMultipleStrings) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "ab"...
            gossip.spill_tea(string.keep_yapping("cd", "ef", "gh"))...
        }
    )";

    expectOutput(source, "abcdefgh\n");
}

TEST_F(InterpreterTests, StringConcatWorksOnNonStringTypes) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "x"...
            gossip.spill_tea(string.keep_yapping(1, ghosted, 2.0, [1, 2]))...
        }
    )";

    expectOutput(source, "x1ghosted2.[1, 2]\n");
}

TEST_F(InterpreterTests, StringContainsWorksOnTruth) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "abcde"...
            gossip.spill_tea(string.sniff_out("cd"))...
        }
    )";

    expectOutput(source, "totally\n");
}

TEST_F(InterpreterTests, StringContainsWorksOnFalsity) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "abcde"...
            gossip.spill_tea(string.sniff_out("xy"))...
        }
    )";

    expectOutput(source, "nah\n");
}

TEST_F(InterpreterTests, Failure_StringUnknownMethodThrowsLogicError) {
    auto source = R"(
        gig macho() {
            stash string about "abc"...
            string.non_existing()...
        }
    )";

    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::Logic,
        "This yap cannot do 'non_existing'. Tell it to shut up.");
}

TEST_F(InterpreterTests, Failure_StringToSolidFailsWhenStringIsNotInt) {
    auto source = R"(
        gig macho() {
            stash string about "abc"...
            string.to_solid()...
        }
    )";
    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::Undefined,
        "stoi");
}

TEST_F(InterpreterTests, Failure_StringToSolidFailsWhenCalledWithArgs) {
    auto source = R"(
        gig macho() {
            stash string about "123"...
            string.to_solid(1)...
        }
    )";
    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::OutOfBounds,
        "Expected 0 args, got 1");
}

TEST_F(InterpreterTests, Failure_StringToUpperCaseFailsWhenCalledWithArgs) {
    auto source = R"(
        gig macho() {
            stash string about "abc"...
            string.scream(1)...
        }
    )";
    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::OutOfBounds,
        "Expected 0 args, got 1");
}

TEST_F(InterpreterTests, Failure_StringToLowerCaseFailsWhenCalledWithArgs) {
    auto source = R"(
        gig macho() {
            stash string about "ABC"...
            string.mumble(1)...
        }
    )";
    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::OutOfBounds,
        "Expected 0 args, got 1");
}

TEST_F(InterpreterTests, Failure_StringLengthFailsWhenCalledWithArgs) {
    auto source = R"(
        gig macho() {
            stash string about "abc"...
            string.yap_level(1)...
        }
    )";
    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::OutOfBounds,
        "Expected 0 args, got 1");
}

TEST_F(InterpreterTests, Failure_StringIsEmptyFailsWhenCalledWithArgs) {
    auto source = R"(
        gig macho() {
            stash string about "abc"...
            string.speechless(1)...
        }
    )";
    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::OutOfBounds,
        "Expected 0 args, got 1");
}

TEST_F(InterpreterTests, Failure_StringTrimFailsWhenCalledWithArgs) {
    auto source = R"(
        gig macho() {
            stash string about "abc"...
            string.cut_the_crap(1)...
        }
    )";
    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::OutOfBounds,
        "Expected 0 args, got 1");
}

TEST_F(InterpreterTests, Failure_StringSubstringFailsWhenCalledWithTooFewArgs) {
    auto source = R"(
        gig macho() {
            stash string about "abc"...
            string.chop()...
        }
    )";
    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::Logic,
        "Expected number of args between 1 and 2, got 0");
}

TEST_F(InterpreterTests, Failure_StringSubstringFailsWhenCalledWithTooManyArgs) {
    auto source = R"(
        gig macho() {
            stash string about "abc"...
            string.chop(1, 2, 3)...
        }
    )";
    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::Logic,
        "Expected number of args between 1 and 2, got 3");
}

TEST_F(InterpreterTests, Failure_StringSubstringFailsWhenIndexIsOutOfBounds) {
    auto source = R"(
        gig macho() {
            stash string about "abc"...
            string.chop(10)...
        }
    )";
    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::Undefined,
        "basic_string");
}

TEST_F(InterpreterTests, Failure_StringConcatFailsWhenCalledWithTooFewArgs) {
    auto source = R"(
        gig macho() {
            stash string about "abc"...
            string.keep_yapping()...
        }
    )";
    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::Logic,
        "Expected number of args between 1 and");
}

TEST_F(InterpreterTests, Failure_StringContainsFailsWhenCalledWithTooFewArgs) {
    auto source = R"(
        gig macho() {
            stash string about "abc"...
            string.sniff_out()...
        }
    )";
    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::OutOfBounds,
        "Expected 1 args, got 0");
}

TEST_F(InterpreterTests, Failure_StringContainsFailsWhenCalledWithTooManyArgs) {
    auto source = R"(
        gig macho() {
            stash string about "abc"...
            string.sniff_out("a", "b")...
        }
    )";
    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::OutOfBounds,
        "Expected 1 args, got 2");
}

TEST_F(InterpreterTests, Failure_StringContainsFailsWhenArgIsNotString) {
    auto source = R"(
        gig macho() {
            stash string about "abc"...
            string.sniff_out(1)...
        }
    )";
    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::TypeMismatch,
        "Anticipated yap instead of solid");
}

TEST_F(InterpreterTests, GossipPrintsVectorWithVariableElement) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash x about 22...
            stash list about [11, x, 33]...
            gossip.spill_tea(list)...
        }
    )";

    expectOutput(source, "[11, 22, 33]\n");
}

TEST_F(InterpreterTests, VectorPatchCorrectlySetsElements) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            gossip.spill_tea(vector)...
            vector.patch(1, 99)...
            gossip.spill_tea(vector)...
            vector.patch(3, 0)...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source,
        "[11, 22, 33]\n"
        "[99, 22, 33]\n"
        "[99, 22, 0]\n");
}

TEST_F(InterpreterTests, VectorYoinkReturnsElementAtOneBasedIndex) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            gossip.spill_tea(vector.yoink(2))...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source,
        "22\n"
        "[11, 22, 33]\n");
}

TEST_F(InterpreterTests, VectorPatchUpdatesElement) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            vector.patch(1, 99)...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source, "[99, 22, 33]\n");
}

TEST_F(InterpreterTests, VectorShoveAddsElementAtEnd) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            vector.shove(44)...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source, "[11, 22, 33, 44]\n");
}

TEST_F(InterpreterTests, VectorKickRemovesAndReturnsLastElement) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            gossip.spill_tea(vector.kick())...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source, "33\n[11, 22]\n");
}

TEST_F(InterpreterTests, VectorVibeCheckReturnsWhetherEmpty) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11]...
            gossip.spill_tea(vector.vibe_check())...
            vector.reset_the_vibe()...
            gossip.spill_tea(vector.vibe_check())...
        }
    )";

    expectOutput(source, "nah\ntotally\n");
}

TEST_F(InterpreterTests, VectorVibeCountReturnsCurrentSize) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            gossip.spill_tea(vector.vibe_count())...
            vector.kick()...
            gossip.spill_tea(vector.vibe_count())...
        }
    )";

    expectOutput(source, "3\n2\n");
}

TEST_F(InterpreterTests, VectorResetTheVibeClearsAllElements) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            vector.reset_the_vibe()...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source, "[]\n");
}

TEST_F(InterpreterTests, VectorResetTheVibeResetsVectorType) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            vector.reset_the_vibe()...
            psst: Int -> Float
            vector.shove(11.11)...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source, "[11.11]\n");
}

TEST_F(InterpreterTests, Failure_VectorUnknownMethodThrowsLogicError) {
    auto source = R"(
        gig macho() {
            stash vector about [1, 2, 3]...
            vector.non_existing()...
        }
    )";

    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::Logic,
        "Lineup cannot do non_existing");
}

TEST_F(InterpreterTests, Failure_VectorYoinkFailsWhenCalledWithWrongArgCount) {
    auto source = R"(
        gig macho() {
            stash vector about [11, 22, 33]...
            vector.yoink()...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::OutOfBounds, "Expected 1 args, got 0");
}

TEST_F(InterpreterTests, Failure_VectorYoinkFailsWhenIndexIsOutOfBounds) {
    auto source = R"(
        gig macho() {
            stash vector about [11, 22, 33]...
            vector.yoink(10)...
        }
    )";

    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::OutOfBounds,
        "Lineup got only 3 places");
}

TEST_F(InterpreterTests, Failure_VectorYoinkFailsWhenIndexIsZeroOrLower) {
    auto source = R"(
        gig macho() {
            stash vector about [11, 22, 33]...
            vector.yoink(0)...
        }
    )";

    expectRuntimeError(
        source,
        RuntimeError::Type::OutOfBounds,
        "Lineup place must be bigger than 1 buddy");
}

TEST_F(InterpreterTests, Failure_VectorYoinkFailsWhenIndexIsNotInt) {
    auto source = R"(
        gig macho() {
            stash vector about [11, 22, 33]...
            vector.yoink("two")...
        }
    )";

    expectRuntimeError(
        source,
        RuntimeError::Type::TypeMismatch,
        "Anticipated solid instead of yap");
}

TEST_F(InterpreterTests, Failure_VectorPatchFailsWhenCalledOnEmptyVector) {
    auto source = R"(
        gig macho() {
            stash vector about []...
            vector.patch(1, 10)...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::Logic, "Lineup is empty");
}

TEST_F(InterpreterTests, Failure_VectorPatchFailsWhenCalledWithWrongArgCount) {
    auto source = R"(
        gig macho() {
            stash vector about [11, 22, 33]...
            vector.patch(1)...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::OutOfBounds, "Expected 2 args, got 1");
}

TEST_F(InterpreterTests, Failure_VectorPatchFailsWhenNewValueTypeDoesNotMatch) {
    auto source = R"(
        gig macho() {
            stash vector about [11, 22, 33]...
            vector.patch(2, "oops")...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::TypeMismatch, "Vibes don't match");
}

TEST_F(InterpreterTests, Failure_VectorShoveFailsWhenCalledWithWrongArgCount) {
    auto source = R"(
        gig macho() {
            stash vector about [11, 22, 33]...
            vector.shove()...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::OutOfBounds, "Expected 1 args, got 0");
}

TEST_F(InterpreterTests, Failure_VectorShoveFailsWhenTypeDoesNotMatch) {
    auto source = R"(
        gig macho() {
            stash vector about [11, 22, 33]...
            vector.shove("oops")...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::TypeMismatch, "Vibes don't match");
}

TEST_F(InterpreterTests, Failure_VectorKickFailsWhenCalledOnEmptyVector) {
    auto source = R"(
        gig macho() {
            stash vector about []...
            vector.kick()...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::Logic, "Lineup is empty");
}

TEST_F(InterpreterTests, Failure_VectorKickFailsWhenCalledWithArguments) {
    auto source = R"(
        gig macho() {
            stash vector about [11]...
            vector.kick(1)...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::OutOfBounds, "Expected 0 args, got 1");
}

TEST_F(InterpreterTests, Failure_VectorVibeCheckFailsWhenCalledWithArguments) {
    auto source = R"(
        gig macho() {
            stash vector about [11]...
            vector.vibe_check(1)...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::OutOfBounds, "Expected 0 args, got 1");
}

TEST_F(InterpreterTests, Failure_VectorVibeCountFailsWhenCalledWithArguments) {
    auto source = R"(
        gig macho() {
            stash vector about [11]...
            vector.vibe_count(1)...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::OutOfBounds, "Expected 0 args, got 1");
}

TEST_F(InterpreterTests, Failure_VectorResetTheVibeFailsWhenCalledWithArguments) {
    auto source = R"(
        gig macho() {
            stash vector about [11]...
            vector.reset_the_vibe(1)...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::OutOfBounds, "Expected 0 args, got 1");
}

TEST_F(InterpreterTests, VectorShoveIntoEmptyVectorSetsAndKeepsType) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about []...
            vector.shove(1)...
            vector.shove(2)...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source, "[1, 2]\n");
}
