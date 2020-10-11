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
        std::vector<Token> tokens = {};

        // Reserve the amount of items in the vector.
        tokens.reserve(amountOfItems);

        // Populate the tokens vector.
        for (int i = 0; i < amountOfItems; i++) {
            tokens.push_back(token());
        }

        // Create the Stream and provide the tokens.
        return TokenStream(tokens);
    }

    Parser parser(const std::vector<Token> &tokens) {
        return Parser(TokenStream(tokens));
    }

    ionshared::Ptr<ionir::Module> ionIrModule(const std::string& identifier) {
        return std::make_shared<ionir::Module>(
            std::make_shared<ionir::Identifier>(identifier)
        );
    }

    ionshared::Ptr<IonIrLoweringPass> ionIrLoweringPass() {
        ionshared::Ptr<ionir::Module> module = ionIrModule();

        ionshared::PtrSymbolTable<ionir::Module> modules =
            std::make_shared<ionshared::SymbolTable<ionshared::Ptr<ionir::Module>>>();

        // TODO: Inserting module, but can be done inline above -- it's just a headache.
        modules->set(**module->identifier, module);

        ionshared::Ptr<IonIrLoweringPass> ionIrCodegenPass =
            std::make_shared<IonIrLoweringPass>(
                std::make_shared<ionshared::PassContext>(),
                modules
            );

        if (!ionIrCodegenPass->setModuleBuffer(**module->identifier)) {
            throw std::runtime_error("Could not set active module buffer during bootstrap process");
        }

        return ionIrCodegenPass;
    }

    ionshared::Ptr<Function> emptyFunction(std::vector<ionshared::Ptr<Statement>> statements) {
        ionshared::Ptr<VoidType> returnType = type_factory::typeVoid();

        // TODO: Consider support for module here.
        ionshared::Ptr<Prototype> prototype = std::make_shared<Prototype>(
            test::constant::foobar,
            std::make_shared<Args>(),
            returnType,
            nullptr
        );

        // The parent will be filled in below.
        ionshared::Ptr<Block> body =
            std::make_shared<Block>(nullptr, std::move(statements));

        // TODO: Provide module parent for function.
        ionshared::Ptr<Function> function =
            std::make_shared<Function>(nullptr, prototype, body);

        // Fill in the body's parent.
        body->parent = function;

        // Fill in the prototype's parent.
        prototype->parent = function;

        return function;
    }
}
