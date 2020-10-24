#include <ionlang/const/const_name.h>
#include <ionlang/misc/util.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    AstPtrResult<Args> Parser::parseArgs(std::shared_ptr<Construct> parent) {
        this->beginSourceLocationMapping();

        std::shared_ptr<ionshared::SymbolTable<Arg>> args =
            std::make_shared<ionshared::SymbolTable<Arg>>();

        bool isVariable = false;

        do {
            // Skip comma token if applicable.
            if (this->is(TokenKind::SymbolComma)) {
                // TODO: Only occurring when the argument list is empty, not when there's no more args to process.
                // Warn about leading, lonely comma.
                if (args->isEmpty()) {
                    this->diagnosticBuilder
                        ->bootstrap(diagnostic::syntaxLeadingCommaInArgs)
                        ->setSourceLocation(this->makeSourceLocation())
                        ->finish();
                }

                // Skip over comma token.
                this->tokenStream.skip();
            }
            else if (this->is(TokenKind::SymbolEllipsis)) {
                isVariable = true;
                this->tokenStream.skip();

                // Variable arguments symbol should be the last argument.
                break;
            }

            std::optional<Arg> argResult = this->parseArg(parent);

            IONLANG_PARSER_ASSERT(argResult.has_value())

            Arg arg = *argResult;

            args->set(arg.second, arg);
        }
        while (this->is(TokenKind::SymbolComma));

        return std::make_shared<Args>(args, isVariable);
    }

    AstPtrResult<Attribute> Parser::parseAttribute(const std::shared_ptr<Construct> &parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolAt))

        std::optional<std::string> name = this->parseName();

        IONLANG_PARSER_ASSERT(name.has_value())

        std::shared_ptr<Attribute> attribute = Construct::makeChild<Attribute>(parent, *name);

        this->finishSourceLocationMapping(attribute);

        return attribute;
    }

    AstResult<Attributes> Parser::parseAttributes(const std::shared_ptr<Construct> &parent) {
        this->beginSourceLocationMapping();

        std::vector<std::shared_ptr<Attribute>> attributes = {};

        while (this->is(TokenKind::SymbolAt)) {
            AstPtrResult<Attribute> attributeResult = this->parseAttribute(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(attributeResult))

            attributes.push_back(util::getResultValue(attributeResult));
        }

        return attributes;
    }

    AstPtrResult<Prototype> Parser::parsePrototype(const std::shared_ptr<Module> &parent) {
        this->beginSourceLocationMapping();

        std::optional<std::string> name = this->parseName();

        IONLANG_PARSER_ASSERT(name.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL))

        std::shared_ptr<Args> args = std::make_shared<Args>();

        // Parse arguments if applicable.
        if (!this->is(TokenKind::SymbolParenthesesR)) {
            // TODO: Not proper parent.
            AstPtrResult<Args> temporaryArgs = this->parseArgs(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(temporaryArgs))

            args = util::getResultValue(temporaryArgs);
        }

        this->tokenStream.skip();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolArrow))

        // TODO: Not proper parent.
        AstPtrResult<Type> returnType = this->parseType(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(returnType))

        std::shared_ptr<Prototype> prototype = Construct::makeChild<Prototype>(
            parent,
            *name,
            args,
            util::getResultValue(returnType)
        );

        this->finishSourceLocationMapping(prototype);

        return prototype;
    }

    AstPtrResult<Extern> Parser::parseExtern(const std::shared_ptr<Module> &parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordExtern))

        AstPtrResult<Prototype> prototype = this->parsePrototype(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(prototype))
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        std::shared_ptr<Extern> externConstruct =
            Construct::makeChild<Extern>(parent, util::getResultValue(prototype));

        this->finishSourceLocationMapping(externConstruct);

        return externConstruct;
    }

    AstPtrResult<Function> Parser::parseFunction(const std::shared_ptr<Module> &parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordFunction))

        AstPtrResult<Prototype> prototypeResult = this->parsePrototype(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(prototypeResult))

        /**
         * Create the resulting function construct here, to be provided
         * as the parent when parsing the body block.
         */
        std::shared_ptr<Function> function = Construct::makeChild<Function>(
            parent,
            util::getResultValue(prototypeResult),

            // To be filled below.
            nullptr
        );

        AstPtrResult<Block> bodyResult = this->parseBlock(function);

        IONLANG_PARSER_ASSERT(util::hasValue(bodyResult))

        // Fill in the nullptr body.
        function->body = util::getResultValue(bodyResult);

        this->finishSourceLocationMapping(function);

        return function;
    }
}
