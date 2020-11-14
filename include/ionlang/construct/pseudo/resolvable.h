#pragma once

#include <optional>
#include <utility>
#include <ionshared/misc/util.h>
#include <ionshared/misc/named.h>
#include <ionlang/construct/identifier.h>
#include <ionlang/construct/construct.h>

namespace ionlang {
    // TODO: What if 'pass.h' is never included?
    struct Pass;

    enum struct ResolvableKind {
        /**
         * The nearest variable declaration, argument, or global
         * variable.
         */
        VariableLike,

        /**
         * A reference to a function or extern.
         */
        FunctionLike,

        PrototypeReturnType,

        /**
         * A struct declaration.
         */
        StructType
    };

    /**
     * A wrapper for a value that may be present or may need
     * to be resolved. Resolution can only occur once, and attempts
     * to re-resolve a value will be denied but no exception will
     * be thrown.
     */
    template<typename T = Construct>
        requires std::derived_from<T, Construct>
    class Resolvable : public Construct {
    private:
        ionshared::OptPtr<T> value;

        ionshared::OptPtr<Construct> cachedParent;

    public:
        [[nodiscard]] static std::shared_ptr<Resolvable<T>> make(
            std::shared_ptr<T> value
        ) noexcept {
            return std::make_shared<Resolvable<T>>(value);
        }

        [[nodiscard]] static std::shared_ptr<Resolvable<T>> make(
            ResolvableKind kind,
            std::shared_ptr<Identifier> id,
            std::shared_ptr<Construct> context
        ) noexcept {
            // TODO: Should identifier be made a child?
            return std::make_shared<Resolvable<T>>(kind, id, context);
        }

        const std::optional<ResolvableKind> resolvableKind;

        // TODO: Should the identifier be a child?
        const std::optional<std::shared_ptr<Identifier>> id;

        const ionshared::OptPtr<Construct> context;

        Resolvable(
            ResolvableKind kind,
            std::shared_ptr<Identifier> id,
            std::shared_ptr<Construct> context // TODO: Change type to Scope (or Context for deeper lookup?).
        ) noexcept :
            Construct(ConstructKind::Resolvable),
            resolvableKind(kind),
            id(std::move(id)),
            context(std::move(context)),
            value(std::nullopt),
            cachedParent(std::nullopt) {
            //
        }

        explicit Resolvable(std::shared_ptr<T> value) noexcept :
            Construct(ConstructKind::Resolvable),
            resolvableKind(std::nullopt),
            id(std::nullopt),
            context(std::nullopt),
            value(value) {
            //
        }

        void accept(Pass& visitor) override {
            // TODO: CRITICAL: Fix 'incomplete type' problem.
            //            visitor.visitResolvable(this->staticCast<Resolvable<>>());
//            this->value->get().accept(visitor); What about this? Still need to visit Resolvable tho.
        }

        /**
         * Cache a parent construct to be applied once the resolvable
         * is resolved, or if it is already resolved, apply it to the
         * value immediately. Will also update the resolvable's parent.
         */
        void setParent(std::optional<std::shared_ptr<Construct>> parent) noexcept override {
            if (this->isResolved()) {
                this->value->get()->setParent(parent);
            }

            BaseConstruct::setParent(parent);
            this->cachedParent = parent;
        }

        [[nodiscard]] std::shared_ptr<T> operator*() {
            if (!this->isResolved()) {
                throw std::runtime_error("Value is not resolved but being accessed");
            }

            return *this->value;
        }

        Resolvable<T>& operator=(std::shared_ptr<T> value) {
            this->resolve(value);
        }

        ionshared::OptPtr<T> getValue() const noexcept {
            return this->value;
        }

        [[nodiscard]] std::shared_ptr<T> forceGetValue() const {
            if (!ionshared::util::hasValue(this->value)) {
                throw std::runtime_error("Value is not set or nullptr");
            }

            return *this->value;
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
        bool resolve(std::shared_ptr<T> value) noexcept {
            if (this->isResolved()) {
                return false;
            }

            this->value = value;

            if (ionshared::util::hasValue(this->cachedParent)) {
                value->setParent(*this->cachedParent);
            }

            return true;
        }

        std::shared_ptr<Resolvable<>> flattenResolvable() {
            return this->template staticCast<Resolvable<>>();
        }
    };

    template<typename T = Construct>
    using PtrResolvable = std::shared_ptr<Resolvable<T>>;

    template<typename T = Construct>
    using OptPtrRef = std::optional<PtrResolvable<T>>;
}
