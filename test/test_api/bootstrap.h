#pragma once

#include <vector>
#include <ionshared/llvm/llvm_module.h>
#include <ionlang/passes/lowering/ionir_lowering_pass.h>
#include <ionlang/lexical/token.h>
#include <ionlang/syntax/parser.h>
#include "util.h"

namespace ionlang::test::bootstrap {
    [[nodiscard]] Token token(TokenKind kind = TokenKind::Unknown);

    [[nodiscard]] TokenStream tokenStream(size_t amountOfItems = 1);

    [[nodiscard]] Parser parser(const std::vector<Token>& tokens);

    [[nodiscard]]  ionshared::Ptr<ionir::Module> ionIrModule(
        const std::string& identifier = "test"
    );

    [[nodiscard]] ionshared::Ptr<IonIrLoweringPass> ionIrLoweringPass();

    [[nodiscard]] ionshared::Ptr<Function> emptyFunction(
        std::vector<ionshared::Ptr<Statement>> statements = {}
    );
}
