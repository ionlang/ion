#pragma once

#include <iostream>
#include <array>
#include <string>
#include <ionlang/passes/lowering/ionir_lowering_pass.h>
#include <ionlang/lexical/token.h>
#include "util.h"
#include "../pch.h"

namespace ionlang::test::compare {
    /**
     * Compare 2 strings. Both strings are trimmed
     * before comparison.
     */
    bool strings(std::string expected, std::string actual);

    /**
     * Compare an LLVM IR output string with
     * a stored LLVM IR file. Both inputs are trimmed
     * before comparison. Returns false if the stored
     * LLVM IR file does not exist.
     */
    bool ir(std::string output, const std::string &fileName);

    bool ir(const ionshared::Ptr<ionir::LlvmCodegenPass> &llvmCodegenPass, const std::string &fileName);

    template<unsigned int N>
    void tokenSets(std::array<Token, N> expected, std::vector<Token> actual) {
        // Both sets should have the same length.
        EXPECT_EQ(expected.size(), actual.size());

        // Compare results by iterating over both expected and actual, resulting tokens.
        int i = 0;

        for (auto &token : actual) {
            // Compare iterator value with its corresponding expected token kind.
            EXPECT_EQ(expected[i], token);

            // Increment counter to prepare for next iteration.
            i++;
        }
    }
}
