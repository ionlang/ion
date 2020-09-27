#include <ionlang/const/const_name.h>
#include <ionlang/misc/util.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    AstPtrResult<Args> Parser::parseArgs() {
        this->beginSourceLocationMapping();

        ionshared::Ptr<ionshared::SymbolTable<Arg>> args =
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
                        ->setLocation(this->makeSourceLocation())
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

            std::optional<Arg> argResult = this->parseArg();

            IONLANG_PARSER_ASSERT(argResult.has_value())

            Arg arg = *argResult;

            args->set(arg.second, arg);
        }
        while (this->is(TokenKind::SymbolComma));

        return std::make_shared<Args>(args, isVariable);
    }

    AstPtrResult<Attribute> Parser::parseAttribute(const ionshared::Ptr<Construct> &parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolAt))

        std::optional<std::string> id = this->parseId();

        IONLANG_PARSER_ASSERT(id.has_value())

        ionshared::Ptr<Attribute> attribute = std::make_shared<Attribute>(parent, *id);

        this->finishSourceLocationMapping(attribute);

        return attribute;
    }

    AstResult<Attributes> Parser::parseAttributes(const ionshared::Ptr<Construct> &parent) {
        this->beginSourceLocationMapping();

        std::vector<ionshared::Ptr<Attribute>> attributes = {};

        while (this->is(TokenKind::SymbolAt)) {
            AstPtrResult<Attribute> attributeResult = this->parseAttribute(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(attributeResult))

            attributes.push_back(util::getResultValue(attributeResult));
        }

        return attributes;
    }

    AstPtrResult<Prototype> Parser::parsePrototype(const ionshared::Ptr<Module> &parent) {
        this->beginSourceLocationMapping();

        std::optional<std::string> id = this->parseId();

        IONLANG_PARSER_ASSERT(id.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL))

        ionshared::Ptr<Args> args = std::make_shared<Args>();

        // Parse arguments if applicable.
        if (!this->is(TokenKind::SymbolParenthesesR)) {
            AstPtrResult<Args> temporaryArgs = this->parseArgs();

            IONLANG_PARSER_ASSERT(util::hasValue(temporaryArgs))

            args = util::getResultValue(temporaryArgs);
        }

        this->tokenStream.skip();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolArrow))

        AstPtrResult<Type> returnType = this->parseType();

        IONLANG_PARSER_ASSERT(util::hasValue(returnType))

        ionshared::Ptr<Prototype> prototype =
            std::make_shared<Prototype>(*id, args, util::getResultValue(returnType), parent);

        this->finishSourceLocationMapping(prototype);

        return prototype;
    }

    AstPtrResult<Extern> Parser::parseExtern(const ionshared::Ptr<Module> &parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordExtern))

        AstPtrResult<Prototype> prototype = this->parsePrototype(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(prototype))
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        ionshared::Ptr<Extern> externConstruct =
            std::make_shared<Extern>(parent, util::getResultValue(prototype));

        this->finishSourceLocationMapping(externConstruct);

        return externConstruct;
    }

    AstPtrResult<Function> Parser::parseFunction(const ionshared::Ptr<Module> &parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordFunction))

        AstPtrResult<Prototype> prototypeResult = this->parsePrototype(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(prototypeResult))

        /**
         * Create the resulting function construct here, to be provided
         * as the parent when parsing the body block.
         */
        ionshared::Ptr<Function> function = std::make_shared<Function>(
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
