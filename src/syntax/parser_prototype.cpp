#include <ionlang/const/const_name.h>
#include <ionlang/misc/util.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    AstPtrResult<Args> Parser::parseArgs() {
        ionshared::SymbolTable<Arg> args = ionshared::SymbolTable<Arg>();
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
                this->tokenStream.next();
            }

            std::optional<Arg> argResult = this->parseArg();

            IONLANG_PARSER_ASSERT(argResult.has_value(), Args)

            Arg arg = *argResult;

            // Set the argument on the symbol table.
            args[arg.second] = arg;
        }
        while (this->is(TokenKind::SymbolComma));

        return std::make_shared<Args>(args, isInfinite);
    }

    AstPtrResult<Prototype> Parser::parsePrototype(const ionshared::Ptr<Module> &parent) {
        std::optional<std::string> id = this->parseId();

        IONLANG_PARSER_ASSERT(id.has_value(), Prototype)
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL), Prototype)

        ionshared::Ptr<Args> args = std::make_shared<Args>();

        // Parse arguments if applicable.
        if (!this->is(TokenKind::SymbolParenthesesR)) {
            AstPtrResult<Args> temporaryArgs = this->parseArgs();

            IONLANG_PARSER_ASSERT(util::hasValue(temporaryArgs), Prototype)

            args = util::getResultValue(temporaryArgs);
        }

        this->tokenStream.skip();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolArrow), Prototype)

        AstPtrResult<Type> returnType = this->parseType();

        IONLANG_PARSER_ASSERT(util::hasValue(returnType), Prototype)

        return std::make_shared<Prototype>(*id, args, util::getResultValue(returnType), parent);
    }

    AstPtrResult<Extern> Parser::parseExtern(const ionshared::Ptr<Module> &parent) {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordExtern), Extern)

        AstPtrResult<Prototype> prototype = this->parsePrototype(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(prototype), Extern)
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon), Extern)

        return std::make_shared<Extern>(util::getResultValue(prototype));
    }

    AstPtrResult<Function> Parser::parseFunction(const ionshared::Ptr<Module> &parent) {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordFunction), Function)

        AstPtrResult<Prototype> prototypeResult = this->parsePrototype(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(prototypeResult), Function)

        /**
         * Create the resulting function construct here, to be provided
         * as the parent when parsing the body block.
         */
        ionshared::Ptr<Function> function = std::make_shared<Function>(
            util::getResultValue(prototypeResult),

            // To be filled below.
            nullptr
        );

        AstPtrResult<Block> bodyResult = this->parseBlock(function);

        IONLANG_PARSER_ASSERT(util::hasValue(bodyResult), Function)

        // Fill in the nullptr body.
        function->setBody(util::getResultValue(bodyResult));

        return function;
    }
}
