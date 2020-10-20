#pragma once

#include <ionshared/misc/named.h>
#include "construct.h"

namespace ionlang {
    struct Pass;

    typedef ionshared::PtrSymbolTable<Type> Fields;

    struct Struct : ConstructWithParent<Module>, ionshared::Named {
        Fields fields;

        Struct(
            std::shared_ptr<Module> parent,
            std::string name,
            Fields fields
        );

        void accept(Pass &visitor) override;

        [[nodiscard]] Ast getChildNodes() override;

        [[nodiscard]] bool containsField(std::string name) const;

        [[nodiscard]] ionshared::OptPtr<Type> lookupField(std::string name);

        void setField(std::string name, std::shared_ptr<Type> field);
    };
}
