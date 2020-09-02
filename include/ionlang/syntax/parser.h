#pragma once

#include <optional>
#include <string>
#include <ionshared/misc/result.h>
#include <ionshared/error_handling/notice_factory.h>
#include <ionshared/syntax/parser_helpers.h>
#include <ionir/misc/helpers.h>
#include <ionir/const/const_name.h>
#include <ionlang/error_handling/notice_sentinel.h>
#include <ionlang/lexical/token.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    // TODO
//    template<typename T>
//    using AstPtrResult = ionshared::PtrResult<T, ErrorMarker>;

    class Parser {
    private:
        TokenStream tokenStream;

        ionshared::Ptr<ionshared::NoticeStack> noticeStack;

        ionshared::Ptr<NoticeSentinel> noticeSentinel;

        std::string filePath;

        // TODO
//        Classifier classifier;

        [[nodiscard]] bool is(TokenKind tokenKind) noexcept;

        [[nodiscard]] bool isNext(TokenKind tokenKind);

        bool expect(TokenKind tokenKind);

        bool skipOver(TokenKind tokenKind);

        [[nodiscard]] ionshared::NoticeFactory createNoticeFactory() noexcept;

        std::nullopt_t makeNotice(
            std::string message,
            ionshared::NoticeType type = ionshared::NoticeType::Error
        );

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

        AstPtrResult<Construct> parseTopLevel(const ionshared::Ptr<Module> &parent);

        /**
         * Parses a integer literal in the form of long (or integer 64).
         */
        AstPtrResult<IntegerLiteral> parseIntegerLiteral();

        AstPtrResult<CharLiteral> parseCharLiteral();

        AstPtrResult<StringLiteral> parseStringLiteral();

        std::optional<std::string> parseId();

        AstPtrResult<Type> parseType();

        AstPtrResult<VoidType> parseVoidType();

        AstPtrResult<IntegerType> parseIntegerType();

        std::optional<Arg> parseArg();

        AstPtrResult<Args> parseArgs();

        AstPtrResult<Prototype> parsePrototype(const ionshared::Ptr<Module> &parent);

        AstPtrResult<Extern> parseExtern(const ionshared::Ptr<Module> &parent);

        AstPtrResult<Function> parseFunction(const ionshared::Ptr<Module> &parent);

        AstPtrResult<Global> parseGlobal();

        AstPtrResult<Value<>> parseLiteral();

        AstPtrResult<Construct> parsePrimaryExpr(const ionshared::Ptr<Block> &parent);

        AstPtrResult<Construct> parseParenthesesExpr(const ionshared::Ptr<Block> &parent);

        AstPtrResult<Construct> parseIdExpr(const ionshared::Ptr<Block> &parent);

        AstPtrResult<BinaryOperation> parseBinaryOperation(const ionshared::Ptr<Block> &parent);

        AstPtrResult<CallExpr> parseCallExpr(const ionshared::Ptr<Block> &parent);

        AstPtrResult<Block> parseBlock(const ionshared::Ptr<Construct> &parent);

        AstPtrResult<Module> parseModule();

        AstPtrResult<Statement> parseStatement(const ionshared::Ptr<Block> &parent);

        AstPtrResult<VariableDecl> parseVariableDecl(const ionshared::Ptr<Block> &parent);

        AstPtrResult<IfStatement> parseIfStatement(const ionshared::Ptr<Block> &parent);

        AstPtrResult<ReturnStatement> parseReturnStatement(const ionshared::Ptr<Block> &parent);

        std::optional<std::string> parseLine();

        // TODO: Add comment-parsing support.

        template<typename T = Construct>
        ionshared::OptPtr<Ref<T>> parseRef(ionshared::Ptr<Construct> owner) {
            std::optional<std::string> id = this->parseId();

            IONIR_PARSER_ASSURE(id)

            return std::make_shared<Ref<T>>(*id, owner);
        }
    };
}
