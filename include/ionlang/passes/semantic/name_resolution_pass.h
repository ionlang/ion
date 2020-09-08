#pragma once

#include <ionshared/error_handling/notice.h>
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
        ionshared::Ptr<ionshared::NoticeStack> noticeStack;

        std::list<ionshared::PtrSymbolTable<Construct>> scope;

    public:
        explicit NameResolutionPass(
            ionshared::Ptr<ionshared::NoticeStack> noticeStack =
                std::make_shared<ionshared::NoticeStack>()
        );

        void visitModule(ionshared::Ptr<Module> node) override;

        void visitScopeAnchor(ionshared::Ptr<ionshared::Scoped<Construct>> node) override;

        void visitRef(PtrRef<> node) override;

        [[nodiscard]] ionshared::Ptr<ionshared::NoticeStack> getNoticeStack() const;

        [[nodiscard]] const std::list<ionshared::PtrSymbolTable<Construct>> &getScope() const;
    };
}
