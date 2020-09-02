#include <ionlang/passes/optimization/dead_code_elimination_pass.h>

namespace ionlang {
    void DeadCodeEliminationPass::visitBlock(ionshared::Ptr<Block> node) {
        std::vector<ionshared::Ptr<Statement>> statements = node->getStatements();
        bool erase = false;

        for (auto iterator = statements.begin(); iterator < statements.end(); ++iterator) {
            ionshared::Ptr<Statement> statement = *iterator;

            /**
             * If applicable, erase all further instructions,
             * as they are considered dead code.
             */
            if (erase) {
                statements.erase(iterator);

                continue;
            }
            /**
             * Determine whether the current instruction
             * is of kind return and it is not the last
             * item in the vector.
             */
            else if (statement->getStatementKind() == StatementKind::Return && ++iterator == statements.end()) {
                /**
                 * If so, erase all following instructions,
                 * as they are considered dead code at this
                 * point.
                 */
                erase = true;
            }
        }
    }
}
