#include <ionlang/passes/semantic/macro_expansion_pass.h>

namespace ionlang {
    MacroExpansionPass::MacroExpansionPass(
        std::shared_ptr<ionshared::PassContext> context
    ) noexcept :
        Pass(std::move(context)) {
        //
    }
}
