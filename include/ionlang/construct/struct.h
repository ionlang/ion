#pragma once

#include <ionshared/misc/named.h>
#include "construct.h"

namespace ionlang {
    struct Pass;

    typedef ionshared::PtrSymbolTable<Type> Fields;

    struct Struct : ConstructWithParent<Module>, ionshared::Named {
        static std::shared_ptr<Struct> make(
            const std::string& name,
            const Fields& fields
        ) noexcept;

        Fields fields;

        Struct(
            std::string name,
            Fields fields
        );

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
