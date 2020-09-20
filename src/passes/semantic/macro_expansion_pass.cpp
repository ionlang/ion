#include <ionlang/passes/semantic/macro_expansion_pass.h>

namespace ionlang {
    MacroExpansionPass::MacroExpansionPass(
        ionshared::Ptr<ionshared::PassContext> context
    ) :
        Pass(std::move(context)) {
        //
    }
}
