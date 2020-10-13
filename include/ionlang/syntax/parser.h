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

        ionshared::Ptr<ionshared::DiagnosticBuilder> diagnosticBuilder;

        ionshared::Ptr<ionshared::SourceMap<ionshared::Ptr<Construct>>> sourceMap;

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

        ionshared::Ptr<ErrorMarker> makeErrorMarker();

        void mapSourceLocation(const AstPtrResult<>& construct);

        void finishSourceLocationMapping(const ionshared::Ptr<Construct>& construct);

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

            ionshared::Ptr<ionshared::DiagnosticBuilder> diagnosticBuilder =
                ionshared::Ptr<ionshared::DiagnosticBuilder>()
        ) noexcept;

        [[nodiscard]] ionshared::Ptr<ionshared::DiagnosticBuilder> getDiagnosticBuilder() const noexcept;

        AstPtrResult<> parseTopLevelFork(const ionshared::Ptr<Module>& parent);

        /**
         * Parses a integer literal in the form of long (or integer 64).
         */
        AstPtrResult<IntegerLiteral> parseIntegerLiteral();

        AstPtrResult<BooleanLiteral> parseBooleanLiteral();

        AstPtrResult<CharLiteral> parseCharLiteral();

        AstPtrResult<StringLiteral> parseStringLiteral();

        std::optional<std::string> parseId();

        AstPtrResult<Type> parseType();

        AstPtrResult<VoidType> parseVoidType();

        AstPtrResult<BooleanType> parseBooleanType(
            const ionshared::Ptr<TypeQualifiers>& qualifiers =
                std::make_shared<TypeQualifiers>()
        );

        AstPtrResult<IntegerType> parseIntegerType(
            const ionshared::Ptr<TypeQualifiers>& qualifiers =
                std::make_shared<TypeQualifiers>()
        );

        AstPtrResult<UserDefinedType> parseUserDefinedType(
            const ionshared::Ptr<TypeQualifiers>& qualifiers =
                std::make_shared<TypeQualifiers>()
        );

        std::optional<Arg> parseArg();

        AstPtrResult<Args> parseArgs();

        AstPtrResult<Attribute> parseAttribute(const ionshared::Ptr<Construct>& parent);

        AstResult<Attributes> parseAttributes(const ionshared::Ptr<Construct>& parent);

        AstPtrResult<Prototype> parsePrototype(const ionshared::Ptr<Module>& parent);

        AstPtrResult<Extern> parseExtern(const ionshared::Ptr<Module>& parent);

        AstPtrResult<Function> parseFunction(const ionshared::Ptr<Module>& parent);

        AstPtrResult<Global> parseGlobal(const ionshared::Ptr<Module>& parent);

        AstPtrResult<Struct> parseStruct(const ionshared::Ptr<Module>& parent);

        AstPtrResult<Expression<>> parseLiteral();

        AstPtrResult<Expression<>> parseExpression(const ionshared::Ptr<Block>& parent);

        AstPtrResult<Expression<>> parsePrimaryExpr(const ionshared::Ptr<Block>& parent);

        AstPtrResult<Expression<>> parseParenthesesExpr(const ionshared::Ptr<Block>& parent);

        AstPtrResult<Expression<>> parseIdExpr(const ionshared::Ptr<Block>& parent);

        AstPtrResult<Expression<>> parseOperationExpr(
            uint32_t expressionPrecedence,
            ionshared::Ptr<Expression<>> leftSideExpression,
            const ionshared::Ptr<Block>& parent
        );

        AstPtrResult<CallExpr> parseCallExpr(const ionshared::Ptr<Block>& parent);

        AstPtrResult<Block> parseBlock(const ionshared::Ptr<Construct>& parent);

        AstPtrResult<Module> parseModule();

        AstPtrResult<Statement> parseStatement(const ionshared::Ptr<Block>& parent);

        AstPtrResult<VariableDeclStatement> parseVariableDecl(const ionshared::Ptr<Block>& parent);

        AstPtrResult<IfStatement> parseIfStatement(const ionshared::Ptr<Block>& parent);

        AstPtrResult<ReturnStatement> parseReturnStatement(const ionshared::Ptr<Block>& parent);

        AstPtrResult<AssignmentStatement> parseAssignmentStatement(const ionshared::Ptr<Block>& parent);

        std::optional<std::string> parseLine();

        // TODO: Add comment-parsing support.

        template<typename T = Construct>
        AstPtrResult<Resolvable<T>> parseResolvable(ionshared::Ptr<Construct> owner) {
            this->beginSourceLocationMapping();

            std::optional<std::string> id = this->parseId();

            IONLANG_PARSER_ASSERT(id.has_value())

            // TODO: Parsing variable ref. only! Not taking in what kind in params!
            return std::make_shared<Resolvable<T>>(ResolvableKind::Variable, *id, owner);
        }
    };
}
