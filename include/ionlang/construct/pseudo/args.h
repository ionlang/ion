#pragma once

#include <utility>
#include <string>
#include <ionshared/misc/helpers.h>
#include <ionshared/tracking/symbol_table.h>
#include <ionlang/construct/type.h>

namespace ionlang {
    typedef std::pair<ionshared::Ptr<Type>, std::string> Arg;

    struct Args {
        ionshared::Ptr<ionshared::SymbolTable<Arg>> items;

        bool isVariable;

        explicit Args(
            ionshared::Ptr<ionshared::SymbolTable<Arg>> items =
                std::make_shared<ionshared::SymbolTable<Arg>>(),

            bool isVariable = false
        );
    };
}
