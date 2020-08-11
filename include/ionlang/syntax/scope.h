#pragma once

#include <ionshared/misc/wrapper.h>
#include <ionshared/misc/helpers.h>
#include <ionlang/construct/construct.h>

namespace ionlang {
    enum class ScopeKind {
        Block
    };

    class Scope : public ionshared::Wrapper<ionshared::Ptr<Construct>> {
    private:
        ScopeKind kind;

    public:
        Scope(ionshared::Ptr<Construct> value, ScopeKind kind);

        ScopeKind getKind() const;
    };
}
