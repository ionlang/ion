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

    [[nodiscard]]  std::shared_ptr<ionir::Module> ionIrModule(
        const std::string& identifier = "test"
    );

    [[nodiscard]] std::shared_ptr<IonIrLoweringPass> ionIrLoweringPass();

    [[nodiscard]] std::shared_ptr<Function> emptyFunction(
        std::vector<std::shared_ptr<Statement>> statements = {}
    );
}
