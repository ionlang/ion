#include <ionlang/construct/pseudo/args.h>

namespace ionlang {
    Args::Args(ionshared::Ptr<ionshared::SymbolTable<Arg>> items, bool isVariable) :
        items(std::move(items)),
        isVariable(isVariable) {
        //
    }
}
