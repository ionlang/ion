#include <ionlang/construct/pseudo/args.h>

namespace ionlang {
    Args::Args(std::shared_ptr<ionshared::SymbolTable<Arg>> items, bool isVariable) :
        items(std::move(items)),
        isVariable(isVariable) {
        //
    }
}
