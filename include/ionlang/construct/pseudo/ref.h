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

        /**
         * A reference to a function or extern.
         */
        Function
    };

    template<typename T = Construct>
    struct Ref : public Construct, public ionshared::Named {
        ionshared::Ptr<Construct> owner;

        const RefKind refKind;

        ionshared::OptPtr<T> value;

    public:
        Ref(
            const std::string &id,
            ionshared::Ptr<Construct> scope, // TODO: Change type to Scope (or Context for deeper lookup?).
            RefKind kind,
            ionshared::OptPtr<T> value = std::nullopt
        ) :
            Construct(ConstructKind::Ref),
            Named{id},
            owner(std::move(scope)),
            refKind(kind),
            value(value) {
            //
        }

        void accept(Pass &visitor) override {
            // TODO: CRITICAL: Fix 'incomplete type' problem.
            // visitor.visitRef(this->dynamicCast<Ref<T>>());
        }

        ionshared::Ptr<T> operator*() {
            if (!this->isResolved()) {
                throw std::runtime_error("Value is not resolved but being accessed");
            }

            return *this->value;
        }

        Ref<T> &operator=(ionshared::Ptr<T> value) {
            this->resolve(value);
        }

        template<typename TValue>
        [[nodiscard]] ionshared::OptPtr<TValue> getValueAs() const {
            // TODO: Ensure T is or derives from Construct.
            return this->isResolved()
                ? std::dynamic_pointer_cast<TValue>(this->getValue()->get())
                : std::nullopt;
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
