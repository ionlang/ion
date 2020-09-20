#pragma once

#include <ionshared/error_handling/diagnostic_builder.h>

#define IONLANG_NOTICE_DEFINE(name, type, message, example) \
    const static inline ionshared::Diagnostic name =\
        ionshared::Diagnostic{\
            type,\
            message,\
            std::nullopt,\
            diagnostic::errorCounter++,\
            example\
        }

namespace ionlang::diagnostic {
    static uint32_t errorCounter = 0;

    IONLANG_NOTICE_DEFINE(
        internalUnexpectedToken,
        ionshared::DiagnosticType::Error,
        "Unexpected token",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        syntaxConversionFailed,
        ionshared::DiagnosticType::Error,
        "Could not convert string to value",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        syntaxCharLengthInvalid,
        ionshared::DiagnosticType::Error,
        "Character value length must be at most 1 character",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        syntaxIntegerValueTypeUnknown,
        ionshared::DiagnosticType::Error,
        "Integer value's type kind could not be determined",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        syntaxLeadingCommaInArgs,
        ionshared::DiagnosticType::Warning,
        "Leading comma in argument list",
        std::nullopt
    );
}
