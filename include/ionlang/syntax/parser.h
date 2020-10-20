#pragma once

#include <optional>
#include <string>
#include <utility>
#include <ionshared/misc/result.h>
#include <ionshared/diagnostics/source_map.h>
#include <ionir/const/const_name.h>
#include <ionlang/lexical/token.h>
#include <ionlang/diagnostics/diagnostic.h>
#include <ionlang/passes/pass.h>
#include <ionlang/misc/util.h>

#define IONLANG_PARSER_ASSERT(condition) if (!condition) { return this->makeErrorMarker(); }

namespace ionlang {
    // TODO
//    template<typename T>
//    using AstPtrResult = ionshared::PtrResult<T, ErrorMarker>;

    class Parser {
    private:
        TokenStream tokenStream;

        std::shared_ptr<ionshared::DiagnosticBuilder> diagnosticBuilder;

        std::shared_ptr<ionshared::SourceMap<std::shared_ptr<Construct>>> sourceMap;

        /**
         * A stack of source location mapping beginnings, containing a
         * pair with the first item denoting the line number and the second
         * the column.
         */
        std::stack<std::pair<uint32_t, uint32_t>> sourceLocationMappingStartStack;

        // TODO
//        Classifier classifier;

        [[nodiscard]] bool is(TokenKind tokenKind) noexcept;

        [[nodiscard]] bool isNext(TokenKind tokenKind);

        bool expect(TokenKind tokenKind);

        bool skipOver(TokenKind tokenKind);

        void beginSourceLocationMapping() noexcept;

        ionshared::SourceLocation makeSourceLocation();

        ionshared::SourceLocation finishSourceLocation();

        std::shared_ptr<ErrorMarker> makeErrorMarker();

        void mapSourceLocation(const AstPtrResult<>& construct);

        void finishSourceLocationMapping(const std::shared_ptr<Construct>& construct);

        template<typename T = Construct>
        AstPtrResult<T> sourceMapCallback(const std::function<AstPtrResult<>()>& callback) {
            this->beginSourceLocationMapping();

            AstPtrResult<> result = callback();

            if (util::hasValue(result)) {
                this->finishSourceLocationMapping(util::getResultValue(result));
            }
            else {
                this->sourceLocationMappingStartStack.pop();
            }

            return result;
        }

    public:
        explicit Parser(
            TokenStream stream,

            std::shared_ptr<ionshared::DiagnosticBuilder> diagnosticBuilder =
                std::shared_ptr<ionshared::DiagnosticBuilder>()
        ) noexcept;

        [[nodiscard]] std::shared_ptr<ionshared::DiagnosticBuilder> getDiagnosticBuilder() const noexcept;

        AstPtrResult<> parseTopLevelFork(const std::shared_ptr<Module>& parent);

        /**
         * Parses a integer literal in the form of long (or integer 64).
         */
        AstPtrResult<IntegerLiteral> parseIntegerLiteral();

        AstPtrResult<BooleanLiteral> parseBooleanLiteral();

        AstPtrResult<CharLiteral> parseCharLiteral();

        AstPtrResult<StringLiteral> parseStringLiteral();

        std::optional<std::string> parseName();

        AstPtrResult<Type> parseType();

        AstPtrResult<VoidType> parseVoidType();

        AstPtrResult<BooleanType> parseBooleanType(
            const std::shared_ptr<TypeQualifiers>& qualifiers =
                std::make_shared<TypeQualifiers>()
        );

        AstPtrResult<IntegerType> parseIntegerType(
            const std::shared_ptr<TypeQualifiers>& qualifiers =
                std::make_shared<TypeQualifiers>()
        );

        AstPtrResult<UserDefinedType> parseUserDefinedType(
            const std::shared_ptr<TypeQualifiers>& qualifiers =
                std::make_shared<TypeQualifiers>()
        );

        std::optional<Arg> parseArg();

        AstPtrResult<Args> parseArgs();

        AstPtrResult<Attribute> parseAttribute(const std::shared_ptr<Construct>& parent);

        AstResult<Attributes> parseAttributes(const std::shared_ptr<Construct>& parent);

        AstPtrResult<Prototype> parsePrototype(const std::shared_ptr<Module>& parent);

        AstPtrResult<Extern> parseExtern(const std::shared_ptr<Module>& parent);

        AstPtrResult<Function> parseFunction(const std::shared_ptr<Module>& parent);

        AstPtrResult<Global> parseGlobal(const std::shared_ptr<Module>& parent);

        AstPtrResult<Struct> parseStruct(const std::shared_ptr<Module>& parent);

        AstPtrResult<Expression<>> parseLiteral();

        AstPtrResult<Expression<>> parseExpression(const std::shared_ptr<Block>& parent);

        AstPtrResult<Expression<>> parsePrimaryExpr(const std::shared_ptr<Block>& parent);

        AstPtrResult<Expression<>> parseParenthesesExpr(const std::shared_ptr<Block>& parent);

        AstPtrResult<Expression<>> parseIdExpr(const std::shared_ptr<Block>& parent);

        AstPtrResult<Expression<>> parseOperationExpr(
            uint32_t expressionPrecedence,
            std::shared_ptr<Expression<>> leftSideExpression,
            const std::shared_ptr<Block>& parent
        );

        AstPtrResult<CallExpr> parseCallExpr(const std::shared_ptr<Block>& parent);

        AstPtrResult<StructDefinition> parseStructDefinitionExpr(
            const std::shared_ptr<Block>& parent
        );

        AstPtrResult<Block> parseBlock(const std::shared_ptr<Construct>& parent);

        AstPtrResult<Module> parseModule();

        AstPtrResult<Statement> parseStatement(const std::shared_ptr<Block>& parent);

        AstPtrResult<VariableDeclStatement> parseVariableDecl(const std::shared_ptr<Block>& parent);

        AstPtrResult<IfStatement> parseIfStatement(const std::shared_ptr<Block>& parent);

        AstPtrResult<ReturnStatement> parseReturnStatement(const std::shared_ptr<Block>& parent);

        AstPtrResult<AssignmentStatement> parseAssignmentStatement(const std::shared_ptr<Block>& parent);

        std::optional<std::string> parseLine();

        // TODO: Add comment-parsing support.

        template<typename T = Construct>
        AstPtrResult<Resolvable<T>> parseResolvable(std::shared_ptr<Construct> owner) {
            this->beginSourceLocationMapping();

            std::optional<std::string> name = this->parseName();

            IONLANG_PARSER_ASSERT(name.has_value())

            // TODO: Parsing variable ref. only! Not taking in what kind in params!
            return std::make_shared<Resolvable<T>>(ResolvableKind::Variable, *name, owner);
        }
    };
}
