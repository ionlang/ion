#pragma once

#include <optional>
#include <string>
#include <utility>
#include <ionshared/misc/result.h>
#include <ionshared/error_handling/source_map.h>
#include <ionir/const/const_name.h>
#include <ionlang/error_handling/notice_sentinel.h>
#include <ionlang/lexical/token.h>
#include <ionlang/passes/pass.h>
#include <ionlang/misc/util.h>

#define IONLANG_PARSER_ASSERT(condition, T) if (!condition) { return AstPtrResult<T>(std::make_shared<ErrorMarker>(this->makeSyntaxRange())); }

namespace ionlang {
    // TODO
//    template<typename T>
//    using AstPtrResult = ionshared::PtrResult<T, ErrorMarker>;

    class Parser {
    private:
        TokenStream tokenStream;

        ionshared::Ptr<ionshared::NoticeStack> noticeStack;

        ionshared::Ptr<NoticeSentinel> noticeSentinel;

        ionshared::Ptr<ionshared::SourceMap<ionshared::Ptr<Construct>>> sourceMap;

        std::string filePath;

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

        std::nullopt_t makeNotice(
            const std::string &message,
            ionshared::NoticeType type = ionshared::NoticeType::Error
        );

        [[nodiscard]] ionshared::Range makeSyntaxRange() const noexcept;

        void beginSourceLocationMapping() noexcept;

        ionshared::SourceLocation makeSourceLocation();

        void mapSourceLocation(AstPtrResult<> construct);

        void finishSourceLocationMapping(const ionshared::Ptr<Construct> &construct);

        template<typename T = Construct>
        AstPtrResult<T> sourceMapCallback(const std::function<AstPtrResult<>()> &callback) {
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
        // TODO: Default value is hard-coded.
        explicit Parser(
            TokenStream stream,

            const ionshared::Ptr<ionshared::NoticeStack> &noticeStack =
                std::make_shared<ionshared::Stack<ionshared::Notice>>(),

            std::string filePath = "anonymous"/*ConstName::anonymous*/
        );

        [[nodiscard]] ionshared::Ptr<NoticeSentinel> getNoticeSentinel() const;

        [[nodiscard]] std::string getFilePath() const;

        AstPtrResult<> parseTopLevelFork(const ionshared::Ptr<Module> &parent);

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
            const ionshared::Ptr<TypeQualifiers> &qualifiers = std::make_shared<TypeQualifiers>()
        );

        AstPtrResult<IntegerType> parseIntegerType(
            const ionshared::Ptr<TypeQualifiers> &qualifiers = std::make_shared<TypeQualifiers>()
        );

        std::optional<Arg> parseArg();

        AstPtrResult<Args> parseArgs();

        AstPtrResult<Attribute> parseAttribute(const ionshared::Ptr<Construct> &parent);

        AstResult<Attributes> parseAttributes(const ionshared::Ptr<Construct> &parent);

        AstPtrResult<Prototype> parsePrototype(const ionshared::Ptr<Module> &parent);

        AstPtrResult<Extern> parseExtern(const ionshared::Ptr<Module> &parent);

        AstPtrResult<Function> parseFunction(const ionshared::Ptr<Module> &parent);

        AstPtrResult<Global> parseGlobal();

        AstPtrResult<Value<>> parseLiteralFork();

        AstPtrResult<Expression> parsePrimaryExpr(const ionshared::Ptr<Block> &parent);

        AstPtrResult<Expression> parseParenthesesExpr(const ionshared::Ptr<Block> &parent);

        AstPtrResult<Expression> parseIdExpr(const ionshared::Ptr<Block> &parent);

        AstPtrResult<BinaryOperation> parseBinaryOperation(const ionshared::Ptr<Block> &parent);

        AstPtrResult<CallExpr> parseCallExpr(const ionshared::Ptr<Block> &parent);

        AstPtrResult<Block> parseBlock(const ionshared::Ptr<Construct> &parent);

        AstPtrResult<Module> parseModule();

        AstPtrResult<Statement> parseStatement(const ionshared::Ptr<Block> &parent);

        AstPtrResult<VariableDeclStatement> parseVariableDecl(const ionshared::Ptr<Block> &parent);

        AstPtrResult<IfStatement> parseIfStatement(const ionshared::Ptr<Block> &parent);

        AstPtrResult<ReturnStatement> parseReturnStatement(const ionshared::Ptr<Block> &parent);

        AstPtrResult<AssignmentStatement> parseAssignmentStatement(const ionshared::Ptr<Block> &parent);

        std::optional<std::string> parseLine();

        // TODO: Add comment-parsing support.

        template<typename T = Construct>
        AstPtrResult<Ref<T>> parseRef(ionshared::Ptr<Construct> owner) {
            std::optional<std::string> id = this->parseId();

            IONLANG_PARSER_ASSERT(id.has_value(), Ref<T>)

            return std::make_shared<Ref<T>>(*id, owner, RefKind::Variable);
        }
    };
}
