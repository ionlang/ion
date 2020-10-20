#pragma once

#include <ionshared/diagnostics/diagnostic.h>
#include <ionshared/tracking/symbol_table.h>
#include <ionlang/misc/helpers.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    struct MacroExpansionPass : Pass {
        IONSHARED_PASS_ID;

        explicit MacroExpansionPass(
            std::shared_ptr<ionshared::PassContext> context
        ) noexcept;
    };
}
