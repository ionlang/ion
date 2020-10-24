#pragma once

#include <ionshared/misc/named.h>
#include <ionlang/construct/pseudo/args.h>
#include <ionlang/construct/pseudo/child_construct.h>
#include "type.h"

namespace ionlang {
    struct Pass;

    /**
     * Prototype's parent is either a function or extern construct.
     */
    struct Prototype : ConstructWithParent<>, ionshared::Named {
        static std::shared_ptr<Prototype> make(
            const std::string& id,
            const std::shared_ptr<Args>& args,
            const std::shared_ptr<Type>& returnType
        ) noexcept;

        std::shared_ptr<Args> args;

        std::shared_ptr<Type> returnType;

        Prototype(
            std::string id,
            std::shared_ptr<Args> args,
            std::shared_ptr<Type> returnType
        );

        void accept(Pass& visitor) override;

        // TODO: Get child constructs?

        // TODO: Move this method to IonIR, since it has no use here.
        /**
         * Returns a mangled identifier if the parent is either an
         * extern or function.
         */
        [[nodiscard]] std::optional<std::string> getMangledId();
    };
}
