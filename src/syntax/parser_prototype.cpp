#include <ionlang/const/const_name.h>
#include <ionlang/misc/util.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    AstPtrResult<ArgumentList> Parser::parseArgumentList(const std::shared_ptr<Construct>& parent) {
        this->beginSourceLocationMapping();

        ionshared::PtrSymbolTable<Resolvable<Type>> symbolTable =
            ionshared::util::makePtrSymbolTable<Resolvable<Type>>();

        bool isVariable = false;

        do {
            // Skip comma token if applicable.
            if (this->is(TokenKind::SymbolComma)) {
                // TODO: Only occurring when the argument list is empty, not when there's no more args to process.
                // Warn about leading, lonely comma.
                if (symbolTable->isEmpty()) {
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

            AstPtrResult<Resolvable<Type>> type = this->parseType(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(type))

            std::optional<std::string> name = this->parseName();

            IONLANG_PARSER_ASSERT(name.has_value())

            symbolTable->set(*name, util::getResultValue(type));
        }
        while (this->is(TokenKind::SymbolComma));

        std::shared_ptr<ArgumentList> argumentList =
            ArgumentList::make(symbolTable, isVariable);

        argumentList->parent = parent;

        return argumentList;
    }

    AstPtrResult<Attribute> Parser::parseAttribute(const std::shared_ptr<Construct>& parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolAt))

        std::optional<std::string> name = this->parseName();

        IONLANG_PARSER_ASSERT(name.has_value())

        std::shared_ptr<Attribute> attribute = Construct::makeChild<Attribute>(parent, *name);

        this->finishSourceLocationMapping(attribute);

        return attribute;
    }

    AstResult<Attributes> Parser::parseAttributes(const std::shared_ptr<Construct>& parent) {
        this->beginSourceLocationMapping();

        std::vector<std::shared_ptr<Attribute>> attributes = {};

        while (this->is(TokenKind::SymbolAt)) {
            AstPtrResult<Attribute> attributeResult = this->parseAttribute(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(attributeResult))

            attributes.push_back(util::getResultValue(attributeResult));
        }

        return attributes;
    }

    AstPtrResult<Prototype> Parser::parsePrototype(const std::shared_ptr<Construct>& parent) {
        // TODO: Parent module not used.

        this->beginSourceLocationMapping();

        std::optional<std::string> name = this->parseName();

        IONLANG_PARSER_ASSERT(name.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL))

        std::shared_ptr<ArgumentList> argumentList = ArgumentList::make();

        // Parse arguments if applicable.
        if (!this->is(TokenKind::SymbolParenthesesR)) {
            // TODO: Not proper parent.
            AstPtrResult<ArgumentList> temporaryArgumentList = this->parseArgumentList(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(temporaryArgumentList))

            argumentList = util::getResultValue(temporaryArgumentList);
        }

        this->tokenStream.skip();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolArrow))

        // TODO: Not proper parent.
        AstPtrResult<Resolvable<Type>> returnType = this->parseType(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(returnType))

        std::shared_ptr<Prototype> prototype = Prototype::make(
            *name,
            argumentList,
            util::getResultValue(returnType)
        );

        prototype->parent = parent;
        this->finishSourceLocationMapping(prototype);

        return prototype;
    }

    AstPtrResult<Extern> Parser::parseExtern(const std::shared_ptr<Module>& parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordExtern))

        AstPtrResult<Prototype> prototype = this->parsePrototype(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(prototype))
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        std::shared_ptr<Extern> externConstruct =
            Extern::make(util::getResultValue(prototype));

        externConstruct->parent = parent;
        this->finishSourceLocationMapping(externConstruct);

        return externConstruct;
    }

    AstPtrResult<Function> Parser::parseFunction(const std::shared_ptr<Module>& parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordFunction))

        AstPtrResult<Prototype> prototypeResult = this->parsePrototype(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(prototypeResult))

        /**
         * Create the resulting function construct here, to be provided
         * as the parent when parsing the body block.
         */
        std::shared_ptr<Function> function = std::make_shared<Function>(
            util::getResultValue(prototypeResult),

            // Body will be filled below.
            nullptr
        );

        function->parent = parent;
        function->prototype->parent = function;

        AstPtrResult<Block> bodyResult = this->parseBlock(function);

        IONLANG_PARSER_ASSERT(util::hasValue(bodyResult))

        // Fill in the nullptr body.
        function->body = util::getResultValue(bodyResult);

        this->finishSourceLocationMapping(function);

        return function;
    }
}
