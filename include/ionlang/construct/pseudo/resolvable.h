#pragma once

#include <optional>
#include <utility>
#include <ionshared/misc/util.h>
#include <ionshared/misc/named.h>
#include <ionlang/construct/construct.h>

namespace ionlang {
    // TODO: What if 'pass.h' is never included?
    class Pass;

    enum struct ResolvableKind {
        Variable,

        /**
         * A reference to a function or extern.
         */
        Prototype,

        PrototypeReturnType
    };

    template<typename T = Construct>
    struct Resolvable : Construct {
        [[nodiscard]] static ionshared::Ptr<Resolvable<T>> make(
            ionshared::Ptr<T> value
        ) noexcept {
            return std::make_shared<Resolvable<T>>(value);
        }

        [[nodiscard]] static ionshared::Ptr<Resolvable<T>> make(
            ResolvableKind kind,
            std::string name,
            ionshared::Ptr<Construct> context
        ) noexcept {
            return std::make_shared<Resolvable<T>>(kind, name, context);
        }

        const std::optional<ResolvableKind> resolvableKind;

        const std::optional<std::string> name;

        const ionshared::OptPtr<Construct> context;

        ionshared::OptPtr<T> value;

        Resolvable(
            ResolvableKind kind,
            std::string name,
            ionshared::Ptr<Construct> context // TODO: Change type to Scope (or Context for deeper lookup?).
        ) noexcept :
            Construct(ConstructKind::Ref),
            resolvableKind(kind),
            name(std::move(name)),
            context(std::move(context)),
            value(std::nullopt) {
            //
        }

        explicit Resolvable(ionshared::Ptr<T> value) noexcept :
            Construct(ConstructKind::Ref),
            resolvableKind(std::nullopt),
            name(std::nullopt),
            context(std::nullopt),
            value(value) {
            //
        }

        void accept(Pass& visitor) override {
            // TODO: CRITICAL: Fix 'incomplete type' problem.
            // visitor.visitRef(this->dynamicCast<Ref<T>>());
        }

        [[nodiscard]] ionshared::Ptr<T> operator*() {
            if (!this->isResolved()) {
                throw std::runtime_error("Value is not resolved but being accessed");
            }

            return *this->value;
        }

        Resolvable<T>& operator=(ionshared::Ptr<T> value) {
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
    using PtrResolvable = ionshared::Ptr<Resolvable<T>>;

    template<typename T = Construct>
    using OptPtrRef = std::optional<PtrResolvable<T>>;
}
