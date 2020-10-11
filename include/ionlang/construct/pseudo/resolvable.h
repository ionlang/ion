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

    /**
     * A wrapper for a value that may be present or may need
     * to be resolved. Resolution can only occur once, and attempts
     * to re-resolve a value will be denied but no exception will
     * be thrown.
     */
    template<typename T = Construct>
    class Resolvable : public Construct {
    private:
        ionshared::OptPtr<T> value;

    public:
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

        ionshared::OptPtr<T> getValue() const noexcept {
            return this->value;
        }

        template<typename TValue>
        [[nodiscard]] ionshared::OptPtr<TValue> getValueAs() const {
            // TODO: Ensure T is or derives from Construct.
            return this->isResolved()
                ? std::dynamic_pointer_cast<TValue>(this->getValue()->get())
                : std::nullopt;
        }

        [[nodiscard]] bool isResolved() noexcept {
            return ionshared::util::hasValue(this->value);
        }

        /**
         * Returns true if the value had not previously been resolved,
         * and was now resolved, false otherwise. Resolution can only
         * occur once.
         */
        bool resolve(ionshared::Ptr<T> value) noexcept {
            if (this->isResolved()) {
                return false;
            }

            this->value = value;

            return true;
        }
    };

    template<typename T = Construct>
    using PtrResolvable = ionshared::Ptr<Resolvable<T>>;

    template<typename T = Construct>
    using OptPtrRef = std::optional<PtrResolvable<T>>;
}
