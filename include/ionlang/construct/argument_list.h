#pragma once

#include <utility>
#include <string>
#include <ionshared/misc/helpers.h>
#include <ionshared/tracking/symbol_table.h>
#include <ionlang/construct/type.h>

namespace ionlang {
    struct ArgumentList : Construct, Scoped<Resolvable<Type>> {
        static std::shared_ptr<ArgumentList> make(
            const ionshared::PtrSymbolTable<Resolvable<Type>>& symbolTable =
                ionshared::util::makePtrSymbolTable<Resolvable<Type>>(),

            bool isVariable = false
        ) noexcept;

        bool isVariable;

        explicit ArgumentList(
            const ionshared::PtrSymbolTable<Resolvable<Type>>& symbolTable =
                ionshared::util::makePtrSymbolTable<Resolvable<Type>>(),

            bool isVariable = false
        );

        void accept(Pass& visitor) override;

        Ast getChildNodes() override;
    };
}
