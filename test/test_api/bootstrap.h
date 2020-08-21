#pragma once

#include <vector>
#include "ionlang/passes/codegen/ionir_codegen_pass.h"
#include "../../include/ionlang/lexical/token.h"
#include "ionir/lexical/token_kind.h"
#include "../../include/ionlang/syntax/parser.h"
#include "ionir/llvm/llvm_module.h"
#include "../../include/ionir/misc/helpers.h"
#include "util.h"

namespace ionlang::test::bootstrap {
    Token token(TokenKind kind = TokenKind::Unknown);

    TokenStream tokenStream(int amountOfItems = 1);

    Parser parser(const std::vector<Token> &tokens);

    ionshared::Ptr<ionir::Module> ionIrModule(std::string identifier = "test");

    ionshared::Ptr<IonIrCodegenPass> ionIrCodegenPass();

    ionshared::Ptr<Function> emptyFunction(std::vector<ionshared::Ptr<Statement>> statements = {});
}
