#pragma once

#include <ionshared/misc/named.h>
#include <ionlang/construct/argument_list.h>
#include <ionlang/construct/pseudo/construct_with_parent.h>
#include "type.h"

namespace ionlang {
    struct Pass;

    /**
     * Prototype's parent is either a function or extern construct.
     */
    struct Prototype : Construct, ionshared::Named {
        static std::shared_ptr<Prototype> make(
            const std::string& name,
            const std::shared_ptr<ArgumentList>& argumentList,
            const PtrResolvable<Type>& returnType
        ) noexcept;

        std::shared_ptr<ArgumentList> argumentList;

        PtrResolvable<Type> returnType;

        Prototype(
            std::string name,
            std::shared_ptr<ArgumentList> argumentList,
            PtrResolvable<Type> returnType
        );

        void accept(Pass& visitor) override;

        Ast getChildNodes() override;

        // TODO: Move this method to IonIR, since it has no use here?
        /**
         * Returns a mangled identifier if the parent is either an
         * extern or function.
         */
        [[nodiscard]] std::optional<std::string> getMangledName();
    };
}
