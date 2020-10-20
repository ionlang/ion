#pragma once

#include <utility>
#include <string>
#include <ionshared/misc/helpers.h>
#include <ionshared/tracking/symbol_table.h>
#include <ionlang/construct/type.h>

namespace ionlang {
    typedef std::pair<std::shared_ptr<Type>, std::string> Arg;

    struct Args {
        std::shared_ptr<ionshared::SymbolTable<Arg>> items;

        bool isVariable;

        explicit Args(
            std::shared_ptr<ionshared::SymbolTable<Arg>> items =
                std::make_shared<ionshared::SymbolTable<Arg>>(),

            bool isVariable = false
        );
    };
}
