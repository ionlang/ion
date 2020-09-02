#pragma once

#include <optional>
#include <utility>
#include <ionshared/misc/util.h>
#include <ionshared/misc/named.h>
#include <ionlang/construct/construct.h>

namespace ionlang {
    // TODO: What if 'pass.h' is never included?
    class Pass;

    template<typename T = Construct>
    class Ref : public Construct, public ionshared::Named {
    private:
        ionshared::Ptr<Construct> owner;

        ionshared::OptPtr<T> value;

    public:
        Ref(
            const std::string &id,
            ionshared::Ptr<Construct> owner,
            ionshared::OptPtr<T> value = std::nullopt
        ) :
            Construct(ConstructKind::Ref),
            Named(id),
            owner(std::move(owner)),
            value(value) {
            //
        }

        void accept(Pass &visitor) override {
            // TODO: CRITICAL: Fix 'incomplete type' problem.
            // visitor.visitRef(this->dynamicCast<Ref<T>>());
        }

        Ref<T> &operator=(ionshared::Ptr<T> value) {
            this->setValue(value);
        }

        [[nodiscard]] ionshared::Ptr<Construct> getOwner() const noexcept {
            return this->owner;
        }

        void setOwner(ionshared::Ptr<Construct> owner) noexcept {
            this->owner = std::move(owner);
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

        void setValue(ionshared::OptPtr<T> value) noexcept {
            this->value = value;
        }

        void removeValue() noexcept {
            this->setValue(std::nullopt);
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
