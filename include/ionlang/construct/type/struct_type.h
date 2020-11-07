#pragma once

#include <ionshared/misc/named.h>
#include <ionlang/construct/method.h>
#include <ionlang/construct/type.h>

namespace ionlang {
    struct Pass;

    typedef ionshared::PtrSymbolTable<Resolvable<Type>> Fields;

    struct StructType : ConstructWithParent<Module, Type, std::string, TypeKind> {
        static std::shared_ptr<StructType> make(
            const std::string& name,
            const Fields& fields,
            const ionshared::PtrSymbolTable<Method>& methods
        ) noexcept;

        Fields fields;

        ionshared::PtrSymbolTable<Method> methods;

        StructType(
            std::string name,
            Fields fields,
            ionshared::PtrSymbolTable<Method> methods
        );

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
