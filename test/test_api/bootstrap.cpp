#include <utility>
#include <ionlang/const/const.h>
#include <ionlang/misc/type_factory.h>
#include "const.h"
#include "bootstrap.h"

namespace ionlang::test::bootstrap {
    Token token(TokenKind kind) {
        return Token(kind, "");
    }

    TokenStream tokenStream(int amountOfItems) {
        std::vector<Token> tokens = {};

        // Populate the tokens vector.
        for (int i = 0; i < amountOfItems; i++) {
            tokens.push_back(token());
        }

        // Create the Stream and provide the tokens.
        return TokenStream(tokens);
    }

    Parser parser(const std::vector<Token> &tokens) {
        return ionlang::Parser(ionlang::TokenStream(tokens));
    }

    ionshared::Ptr<ionir::Module> ionIrModule(const std::string &identifier) {
        return std::make_shared<ionir::Module>(identifier);
    }

    ionshared::Ptr<IonIrCodegenPass> ionIrCodegenPass() {
        ionshared::Ptr<ionir::Module> module = ionIrModule();

        ionshared::PtrSymbolTable<ionir::Module> modules =
            std::make_shared<ionshared::SymbolTable<ionshared::Ptr<ionir::Module>>>();

        // TODO: Inserting module, but can be done inline above -- it's just a headache.
        modules->insert(module->getId(), module);

        ionshared::Ptr<IonIrCodegenPass> ionIrCodegenPass =
            std::make_shared<IonIrCodegenPass>(modules);

        if (!ionIrCodegenPass->setModuleBuffer(module->getId())) {
            throw std::runtime_error("Could not set active module buffer during bootstrap process");
        }

        return ionIrCodegenPass;
    }

    ionshared::Ptr<Function> emptyFunction(std::vector<ionshared::Ptr<Statement>> statements) {
        ionshared::Ptr<VoidType> returnType = TypeFactory::typeVoid();

        // TODO: Consider support for module here.
        ionshared::Ptr<Prototype> prototype = std::make_shared<Prototype>(test::constant::foobar, std::make_shared<Args>(), returnType, nullptr);

        ionshared::Ptr<BasicBlock> entrySection = std::make_shared<BasicBlock>(BasicBlockOpts{
            nullptr,
            BasicBlockKind::Entry,
            Const::basicBlockEntryId,
            {},
            std::move(statements)
        });

        // TODO: Fix mumbo-jumbo debugging code. -------------

        typedef ionshared::PtrSymbolTable<BasicBlock> BasicBlockPtrSymbolTable;
        typedef ionshared::SymbolTable<ionshared::Ptr<BasicBlock>> tt;

        auto t1 = std::map<std::string, ionshared::Ptr<BasicBlock>>{
            {entrySection->getId(), entrySection}
        };

        BasicBlockPtrSymbolTable sections = std::make_shared<tt>(t1);

        // --------------------

        ionshared::Ptr<Function> function = std::make_shared<Function>(prototype, nullptr);
        ionshared::Ptr<FunctionBody> body = std::make_shared<FunctionBody>(function, sections);

        entrySection->setParent(body);
        function->setBody(body);

        return function;
    }
}
