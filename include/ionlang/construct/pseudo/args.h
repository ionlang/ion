#pragma once

#include <utility>
#include <string>
#include <ionshared/misc/helpers.h>
#include <ionshared/tracking/symbol_table.h>
#include <ionlang/construct/type.h>

namespace ionlang {
    typedef std::pair<ionshared::Ptr<Type>, std::string> Arg;

    class Args {
    private:
        ionshared::SymbolTable<Arg> items;

        bool isVariable;

    public:
        explicit Args(ionshared::SymbolTable<Arg> items = {}, bool isVariable = false);

        ionshared::SymbolTable<Arg> getItems() const noexcept;

        void setItems(ionshared::SymbolTable<Arg> items) noexcept;

        bool getIsVariable() const noexcept;

        void setIsVariable(bool isVariable) noexcept;
    };
}
