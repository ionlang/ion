#pragma once

#include <ionshared/error_handling/diagnostic_builder.h>

#define IONLANG_NOTICE_DEFINE(name, type, message, example) \
    const static inline ionshared::Diagnostic name = ionshared::Diagnostic{ionshared::Notice(type, message), notice::errorCounter++, example}

namespace ionlang::notice {
    static uint32_t errorCounter = 0;

    IONLANG_NOTICE_DEFINE(
        internalUnexpectedToken,
        ionshared::NoticeType::Error,
        "Unexpected token",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        syntaxConversionFailed,
        ionshared::NoticeType::Error,
        "Could not convert string to value",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        syntaxCharLengthInvalid,
        ionshared::NoticeType::Error,
        "Character value length must be at most 1 character",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        syntaxIntegerValueTypeUnknown,
        ionshared::NoticeType::Error,
        "Integer value's type kind could not be determined",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        syntaxLeadingCommaInArgs,
        ionshared::NoticeType::Warning,
        "Leading comma in argument list",
        std::nullopt
    );
}
