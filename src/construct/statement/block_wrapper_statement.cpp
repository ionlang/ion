#include <ionlang/passes/pass.h>

namespace ionlang {
    BlockWrapperStatement::BlockWrapperStatement(const BlockWrapperStatementOpts &opts) :
        Statement(opts.parent, StatementKind::BlockWrapper),
        block(opts.block) {
        //
    }

    void BlockWrapperStatement::accept(Pass &visitor) {
        visitor.visitBlockWrapperStatement(this->dynamicCast<BlockWrapperStatement>());
    }

    Ast BlockWrapperStatement::getChildNodes() {
        return {
            this->block
        };
    }
}
