#pragma once

#include <ionshared/diagnostics/diagnostic_builder.h>

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
        ionshared::DiagnosticKind::InternalError,
        "Unexpected token",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        syntaxConversionFailed,
        ionshared::DiagnosticKind::Error,
        "Could not convert string to value",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        syntaxCharLengthInvalid,
        ionshared::DiagnosticKind::Error,
        "Character value length must be at most 1 character",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        syntaxIntegerValueTypeUnknown,
        ionshared::DiagnosticKind::Error,
        "Integer value's type kind could not be determined",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        syntaxLeadingCommaInArgs,
        ionshared::DiagnosticKind::Warning,
        "Leading comma in argument list",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        syntaxUnexpectedToken,
        ionshared::DiagnosticKind::Error,
        "Expected token '%s' but got '%s'",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        syntaxUnexpectedEof,
        ionshared::DiagnosticKind::Error,
        "Expected token '%s' but got end of file",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        structFieldRedefinition,
        ionshared::DiagnosticKind::Error,
        "Field '%s' in struct '%s' was already previously defined",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        functionRedefinition,
        ionshared::DiagnosticKind::Error,
        "Function '%s' was already previously defined",
        std::nullopt
    );

    IONLANG_NOTICE_DEFINE(
        intrinsicUnknownModule,
        ionshared::DiagnosticKind::Error,
        "There is no intrinsic module named '%s'",
        std::nullopt
    );
}
