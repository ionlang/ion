#pragma once

#include "../../../../../../../../../Program Files (x86)/ionshared/include/ionshared/misc/named.h"
#include "../construct.h"

namespace ionlang {
    struct Pass;

    typedef ionshared::PtrSymbolTable<Resolvable<Type>> Fields;

    struct StructType : ConstructWithParent<Module, Type, std::string, TypeKind> {
        static std::shared_ptr<StructType> make(
            const std::string& name,
            const Fields& fields
        ) noexcept;

        Fields fields;

        StructType(
            std::string name,
            Fields fields
        );

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
