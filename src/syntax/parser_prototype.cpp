#include <ionlang/const/const_name.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    ionshared::OptPtr<Args> Parser::parseArgs() {
        ionshared::SymbolTable<Arg> args = {};
        bool isInfinite = false;

        do {
            // Skip comma token if applicable.
            if (this->is(TokenKind::SymbolComma)) {
                // Prevent leading, lonely comma.
                if (args.isEmpty()) {
                    throw std::runtime_error("No leading lonely comma allowed");
                    // TODO:
//                    return this->noticeSentinel->makeError<Args>(
//                        IONLANG_NOTICE_FUNCTION_CALL_LEADING_COMMA
//                    );
                }

                // Skip over comma token.
                this->stream.next();
            }

            std::optional<Arg> argResult = this->parseArg();

            IONIR_PARSER_ASSURE(argResult)

            Arg arg = *argResult;

            // Set the argument on the symbol table.
            args[arg.second] = arg;
        }
        while (this->is(TokenKind::SymbolComma));

        return std::make_shared<Args>(args, isInfinite);
    }

    ionshared::OptPtr<Prototype> Parser::parsePrototype(const ionshared::Ptr<Module> &parent) {
        std::optional<std::string> id = this->parseId();

        IONIR_PARSER_ASSURE(id)
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL))

        ionshared::Ptr<Args> args = std::make_shared<Args>();

        // Parse arguments if applicable.
        if (!this->is(TokenKind::SymbolParenthesesR)) {
            ionshared::OptPtr<Args> temporaryArgs = this->parseArgs();

            IONIR_PARSER_ASSURE(temporaryArgs)

            args = *temporaryArgs;
        }

        this->stream.skip();

        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolArrow))

        ionshared::OptPtr<Type> returnType = this->parseType();

        IONIR_PARSER_ASSURE(returnType)

        return std::make_shared<Prototype>(*id, args, *returnType, parent);
    }

    ionshared::OptPtr<Extern> Parser::parseExtern(const ionshared::Ptr<Module> &parent) {
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::KeywordExtern))

        ionshared::OptPtr<Prototype> prototype = this->parsePrototype(parent);

        IONIR_PARSER_ASSURE(prototype)

        return std::make_shared<Extern>(*prototype);
    }

    ionshared::OptPtr<Function> Parser::parseFunction(const ionshared::Ptr<Module> &parent) {
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::KeywordFunction))

        ionshared::OptPtr<Prototype> prototype = this->parsePrototype(parent);
        ionshared::OptPtr<FunctionBody> bodyResult = this->parseFunctionBody(nullptr);

        IONIR_PARSER_ASSURE(prototype)
        IONIR_PARSER_ASSURE(bodyResult)

        ionshared::Ptr<FunctionBody> body = *bodyResult;

        ionshared::Ptr<Function> function =
            std::make_shared<Function>(
                *prototype,
                body
            );

        body->setParent(function);

        return function;
    }
}
