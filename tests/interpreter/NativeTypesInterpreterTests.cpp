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
