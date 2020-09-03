#pragma once

#include <ionshared/error_handling/notice.h>
#include <ionshared/tracking/symbol_table.h>
#include <ionlang/misc/helpers.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    class MacroExpansionPass : public Pass {
    public:
        MacroExpansionPass();
    };
}
