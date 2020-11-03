#include <utility>
#include <ionir/construct/module.h>
#include <ionir/construct/identifier.h>
#include <ionlang/const/const.h>
#include <ionlang/type_system/type_factory.h>
#include "const.h"
#include "bootstrap.h"

namespace ionlang::test::bootstrap {
    Token token(TokenKind kind) {
        return Token(kind, "");
    }

    TokenStream tokenStream(size_t amountOfItems) {
        std::vector<Token> tokens{};

        // Reserve the amount of items in the vector.
        tokens.reserve(amountOfItems);

        // Populate the tokens vector.
        for (int i = 0; i < amountOfItems; i++) {
            tokens.push_back(token());
        }

        // Create the Stream and provide the tokens.
        return TokenStream(tokens);
    }

    Parser parser(const std::vector<Token>& tokens) {
        return Parser(TokenStream(tokens));
    }

    std::shared_ptr<ionir::Module> irModule(const std::string& identifier) {
        return std::make_shared<ionir::Module>(
            std::make_shared<ionir::Identifier>(identifier)
        );
    }

    std::shared_ptr<IonIrLoweringPass> irLoweringPass() {
        std::shared_ptr<ionir::Module> module = bootstrap::irModule();

        ionshared::PtrSymbolTable<ionir::Module> modules =
            std::make_shared<ionshared::SymbolTable<std::shared_ptr<ionir::Module>>>();

        // TODO: Inserting module, but can be done inline above -- it's just a headache.
        modules->set(**module->identifier, module);

        std::shared_ptr<IonIrLoweringPass> irCodegenPass =
            std::make_shared<IonIrLoweringPass>(
                std::make_shared<ionshared::PassContext>(),
                modules
            );

        // TODO: CRITICAL: Breaking tests. Find a way to set the module buffer.
//        if (!irCodegenPass->setModuleBuffer(**module->identifier)) {
//            throw std::runtime_error("Could not set active module buffer during bootstrap process");
//        }

        return irCodegenPass;
    }

    std::shared_ptr<Function> emptyFunction(const std::vector<std::shared_ptr<Statement>>& statements) {
        std::shared_ptr<VoidType> returnType = type_factory::typeVoid();

        // TODO: Consider support for module here.
        std::shared_ptr<Prototype> prototype = Prototype::make(
            test::constant::foobar,
            std::make_shared<Args>(),
            returnType
        );

        // The parent will be filled in below.
        std::shared_ptr<Block> body = Block::make(statements);

        // TODO: Provide module parent for function.
        std::shared_ptr<Function> function = Function::make(prototype, body);

        // Fill in the body's parent.
        body->parent = function;

        // Fill in the prototype's parent.
        prototype->parent = function;

        return function;
    }
}
