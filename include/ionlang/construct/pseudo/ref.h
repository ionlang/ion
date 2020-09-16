#pragma once

#include <optional>
#include <utility>
#include <ionshared/misc/util.h>
#include <ionshared/misc/named.h>
#include <ionlang/construct/expression.h>

namespace ionlang {
    // TODO: What if 'pass.h' is never included?
    class Pass;

    enum class RefKind {
        Variable,

        Function
    };

    template<typename T = Construct>
    class Ref : public Construct, public ionshared::Named {
    private:
        ionshared::Ptr<Construct> owner;

        RefKind kind;

        ionshared::OptPtr<T> value;

    public:
        Ref(
            const std::string &id,
            ionshared::Ptr<Construct> scope, // TODO: Change type to Scope (or Context for deeper lookup?).
            RefKind kind,
            ionshared::OptPtr<T> value = std::nullopt
        ) :
            Construct(ConstructKind::Ref),
            Named(id),
            owner(std::move(scope)),
            kind(kind),
            value(value) {
            //
        }

        void accept(Pass &visitor) override {
            // TODO: CRITICAL: Fix 'incomplete type' problem.
            // visitor.visitRef(this->dynamicCast<Ref<T>>());
        }

        Ref<T> &operator=(ionshared::Ptr<T> value) {
            this->resolve(value);
        }

        [[nodiscard]] ionshared::Ptr<Construct> getOwner() const noexcept {
            return this->owner;
        }

        void setOwner(ionshared::Ptr<Construct> owner) noexcept {
            this->owner = std::move(owner);
        }

        [[nodiscard]] RefKind getRefKind() const noexcept {
            return this->kind;
        }

        [[nodiscard]] ionshared::OptPtr<T> getValue() const noexcept {
            return this->value;
        }

        template<typename TValue>
        [[nodiscard]] ionshared::OptPtr<TValue> getValueAs() const {
            // TODO: Ensure T is or derives from Construct.
            return this->isResolved()
                ? std::dynamic_pointer_cast<TValue>(this->getValue()->get())
                : std::nullopt;
        }

        void resolve(ionshared::OptPtr<T> value) noexcept {
            this->value = value;
        }

        void removeValue() noexcept {
            this->resolve(std::nullopt);
        }

        [[nodiscard]] bool isResolved() noexcept {
            return ionshared::util::hasValue(this->value);
        }
    };

    template<typename T = Construct>
    using PtrRef = ionshared::Ptr<Ref<T>>;

    template<typename T = Construct>
    using OptPtrRef = std::optional<PtrRef<T>>;
}
