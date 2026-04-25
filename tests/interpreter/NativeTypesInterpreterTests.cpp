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
