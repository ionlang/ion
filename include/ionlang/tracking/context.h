#pragma once

#include <vector>
#include <ionshared/tracking/symbol_table.h>
#include <ionshared/tracking/scope_anchor.h>
#include <ionlang/construct/construct.h>

namespace ionlang {
    typedef ionshared::PtrSymbolTable<Construct> Scope;

    class Context {
    private:
        std::vector<Scope> scopes;

        Scope globalScope;

    public:
        explicit Context(
            Scope globalScope = ionshared::util::makePtrSymbolTable<Construct>(),
            std::vector<Scope> scopes = {}
        );

        [[nodiscard]] std::vector<Scope> getScopes() const noexcept;

        void setScopes(std::vector<Scope> scopes) noexcept;

        [[nodiscard]] Scope getGlobalScope() const noexcept;

        void setGlobalScope(Scope globalScope) noexcept;

        void appendScope(const Scope &scope) noexcept;

        void appendScope(
            const ionshared::Ptr<ionshared::ScopeAnchor<Construct>> &scopeAnchor
        ) noexcept;

        /**
         * Removes the lastly added scope from the scopes list.
         * Nothing will happen if the scopes list is empty.
         */
        void popScope();

        /**
         * Attempt to locate the nearest (starting from most recent
         * scopes) construct by it's id in the scope symbol tables.
         */
        [[nodiscard]] ionshared::OptPtr<Construct> findNearest(const std::string &id);
    };
}
