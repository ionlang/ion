#pragma once

#include <optional>
#include <string>
#include <ionshared/reporting/notice_factory.h>
#include <ionir/misc/helpers.h>
#include <ionir/const/const_name.h>
#include <ionlang/lexical/token.h>
#include <ionlang/construct/construct.h>
#include "scope.h"
#include "parser_helpers.h"

namespace ionlang {
    class Parser {
    private:
        TokenStream stream;

        ionshared::StackTrace stackTrace;

        std::string filePath;

        Classifier classifier;

    protected:
        bool is(TokenKind tokenKind) noexcept;

        bool isNext(TokenKind tokenKind);

        bool expect(TokenKind tokenKind);

        bool skipOver(TokenKind tokenKind);

        ionshared::NoticeFactory createNoticeFactory() noexcept;

        std::nullopt_t makeNotice(std::string message, ionshared::NoticeType type = ionshared::NoticeType::Error);

    public:
        // TODO: Default value is hard-coded.
        explicit Parser(TokenStream stream, ionshared::StackTrace stackTrace = {}, std::string filePath = "anonymous"/*ConstName::anonymous*/);

        ionshared::StackTrace getStackTrace() const;

        std::string getFilePath() const;

        ionshared::OptPtr<Construct> parseTopLevel(ionshared::Ptr<Module> parent);
    };
}
