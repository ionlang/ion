#pragma once

#include <optional>
#include <string>
#include <ionshared/error_handling/notice_factory.h>
#include <ionlang/error_handling/notice_sentinel.h>
#include <ionshared/syntax/parser_helpers.h>
#include <ionir/misc/helpers.h>
#include <ionir/const/const_name.h>
#include <ionlang/lexical/token.h>
#include <ionlang/construct/construct.h>
#include <ionlang/construct/module.h>
#include <ionlang/construct/type/void_type.h>
#include "scope.h"

namespace ionlang {
    class Parser {
    private:
        TokenStream stream;

        ionshared::Ptr<ionshared::NoticeStack> noticeStack;

        ionshared::Ptr<NoticeSentinel> noticeSentinel;

        std::string filePath;

        // TODO
//        Classifier classifier;

    protected:
        bool is(TokenKind tokenKind) noexcept;

        bool isNext(TokenKind tokenKind);

        bool expect(TokenKind tokenKind);

        bool skipOver(TokenKind tokenKind);

        ionshared::NoticeFactory createNoticeFactory() noexcept;

        std::nullopt_t makeNotice(std::string message, ionshared::NoticeType type = ionshared::NoticeType::Error);

    public:
        // TODO: Default value is hard-coded.
        explicit Parser(
            TokenStream stream,

            const ionshared::Ptr<ionshared::NoticeStack> &noticeStack =
                std::make_shared<ionshared::Stack<ionshared::Notice>>(),

            std::string filePath = "anonymous"/*ConstName::anonymous*/
        );

        ionshared::Ptr<NoticeSentinel> getNoticeSentinel() const;

        std::string getFilePath() const;

        ionshared::OptPtr<Construct> parseTopLevel(const ionshared::Ptr<Module> &parent);

        /**
         * Parses a integer literal in the form of long (or integer 64).
         */
        ionshared::OptPtr<IntegerValue> parseInt();

        ionshared::OptPtr<CharValue> parseChar();

        ionshared::OptPtr<StringValue> parseString();

        std::optional<std::string> parseId();

        ionshared::OptPtr<Type> parseType();

        ionshared::OptPtr<VoidType> parseVoidType();

        ionshared::OptPtr<IntegerType> parseIntegerType();

        std::optional<Arg> parseArg();

        ionshared::OptPtr<Args> parseArgs();

        ionshared::OptPtr<Prototype> parsePrototype(ionshared::Ptr<Module> parent);

        ionshared::OptPtr<Extern> parseExtern(ionshared::Ptr<Module> parent);

        ionshared::OptPtr<Function> parseFunction(ionshared::Ptr<Module> parent);

        ionshared::OptPtr<Global> parseGlobal();

        ionshared::OptPtr<Value<>> parseValue();

        ionshared::OptPtr<Construct> parsePrimaryExpr(ionshared::Ptr<Construct> parent);

        ionshared::OptPtr<BasicBlock> parseBasicBlock(ionshared::Ptr<FunctionBody> parent);

        ionshared::OptPtr<FunctionBody> parseFunctionBody(ionshared::Ptr<Function> parent);

        ionshared::OptPtr<Module> parseModule();

        ionshared::OptPtr<VariableDeclaration> parseVariableDecl(ionshared::Ptr<BasicBlock> parent);

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
