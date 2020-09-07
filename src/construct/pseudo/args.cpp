#include <ionlang/construct/pseudo/args.h>

namespace ionlang {
    Args::Args(ionshared::Ptr<ionshared::SymbolTable<Arg>> items, bool isVariable) :
        items(std::move(items)),
        isVariable(isVariable) {
        //
    }

    ionshared::Ptr<ionshared::SymbolTable<Arg>> Args::getItems() const noexcept {
        return this->items;
    }

    void Args::setItems(ionshared::Ptr<ionshared::SymbolTable<Arg>> items) noexcept {
        this->items = std::move(items);
    }

    bool Args::getIsVariable() const noexcept {
        return this->isVariable;
    }

    void Args::setIsVariable(bool isInfinite) noexcept {
        this->isVariable = isInfinite;
    }
}
