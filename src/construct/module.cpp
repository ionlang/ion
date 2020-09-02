#include <ionlang/passes/pass.h>

namespace ionlang {
    Module::Module(std::string id, ionshared::Ptr<Context> context) :
        Construct(ConstructKind::Module),
        ionshared::Named(std::move(id)),
        context(std::move(context)) {
        //
    }

    void Module::accept(Pass &visitor) {
        visitor.visitModule(this->dynamicCast<Module>());
    }

    Ast Module::getChildNodes() {
        return Construct::convertChildren(
            // TODO: What about normal scopes? Merge that with global scope. Or actually, module just uses global context, right?
            this->context->getGlobalScope()
        );
    }

    ionshared::Ptr<Context> Module::getContext() const noexcept {
        return this->context;
    }

    void Module::setContext(ionshared::Ptr<Context> context) noexcept {
        this->context = std::move(context);
    }
}
