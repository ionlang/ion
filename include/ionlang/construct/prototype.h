#pragma once

#include <ionshared/misc/named.h>
#include <ionlang/construct/pseudo/args.h>
#include <ionlang/construct/pseudo/child_construct.h>
#include "type.h"

namespace ionlang {
    class Pass;

    /**
     * Prototype's parent is either a function or extern construct.
     */
    struct Prototype : ConstructWithParent<>, ionshared::Named {
        std::shared_ptr<Args> args;

        std::shared_ptr<Type> returnType;

        Prototype(
            std::string id,
            std::shared_ptr<Args> args,
            std::shared_ptr<Type> returnType,

            // TODO: Make parent first arg.
            std::shared_ptr<Construct> parent
        );

        void accept(Pass &visitor) override;

        // TODO: Get child constructs?

        // TODO: Move this method to IonIR, since it has no use here.
        /**
         * Returns a mangled identifier if the parent is either an
         * extern or function.
         */
        [[nodiscard]] std::optional<std::string> getMangledId();
    };
}
