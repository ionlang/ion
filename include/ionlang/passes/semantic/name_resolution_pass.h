#pragma once

#include <ionshared/diagnostics/diagnostic.h>
#include <ionshared/tracking/symbol_table.h>
#include <ionlang/misc/helpers.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    /**
     * Resolves partial constructs which reference
     * undefined symbols at the time by their identifier(s).
     */
    class NameResolutionPass : public Pass {
    private:
        std::list<ionshared::PtrSymbolTable<Construct>> scope;

        [[nodiscard]] static ionshared::OptPtr<Construct> findGlobalConstruct(
            std::string name,
            const std::shared_ptr<Construct>& owner
        );

    public:
        IONSHARED_PASS_ID;

        explicit NameResolutionPass(
            std::shared_ptr<ionshared::PassContext> context
        ) noexcept;

        void visitModule(std::shared_ptr<Module> node) override;

        void visitScopeAnchor(std::shared_ptr<Scoped<>> node) override;

        void visitResolvable(PtrResolvable<> node) override;

        [[nodiscard]] const std::list<ionshared::PtrSymbolTable<Construct>>& getScope() const;
    };
}
