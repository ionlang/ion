#include <string>
#include <ionlang/passes/semantic/entry_point_check_pass.h>
#include <ionlang/const/const_name.h>

namespace ionlang {
    void EntryPointCheckPass::finish() {
        /**
         * An entry function could not be found.
         */
        if (!ionshared::util::hasValue(this->entryFunction)) {
            throw std::runtime_error("No suitable entry point could be found");
        }
            /**
             * At this point, the entry point exists. Clear up the entry
             * function as it is no longer needed after the pass has ran.
             */
        else {
            this->entryFunction.reset();
        }
    }

    void EntryPointCheckPass::visitFunction(ionshared::Ptr<Function> node) {
        Pass::visitFunction(node);

        if (node->getPrototype()->getId() == ConstName::main) {
            /**
             * Make sure there was not an entry point function
             * previously encountered.
             */
            if (ionshared::util::hasValue(this->entryFunction)) {
                throw std::runtime_error("Entry point function had already previously been declared");
            }

            this->entryFunction = node;
        }
    }
}
