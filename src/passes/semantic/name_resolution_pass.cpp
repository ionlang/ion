#include <utility>
#include <ionlang/passes/semantic/name_resolution_pass.h>

namespace ionlang {
    ionshared::OptPtr<Construct> NameResolutionPass::findFunctionLikeTarget(
        std::string name,
        const ionshared::Ptr<Construct>& owner
    ) {
        if (owner->constructKind != ConstructKind::Block) {
            // TODO: Better error.
            throw std::runtime_error("Cannot resolve function reference when owner is not a block");
        }

        ionshared::OptPtr<Function> parentFunction =
            owner->dynamicCast<Block>()->findParentFunction();

        if (!ionshared::util::hasValue(parentFunction)) {
            // TODO: Use diagnostics.
            throw std::runtime_error("Could not find parent function of block");
        }

        auto rootModuleSymbolTable =
            parentFunction->get()->getUnboxedParent()->context->getGlobalScope();

        auto lookupResult = rootModuleSymbolTable->lookup(std::move(name));

        if (!ionshared::util::hasValue(lookupResult)) {
            return std::nullopt;
        }

        ConstructKind lookupResultConstructKind = lookupResult->get()->constructKind;

        if (lookupResultConstructKind != ConstructKind::Function && lookupResultConstructKind != ConstructKind::Extern) {
            // TODO: Should be internal error?
            throw std::runtime_error("Expected construct to be a function or extern");
        }

        return *lookupResult;
    }

    NameResolutionPass::NameResolutionPass(
        ionshared::Ptr<ionshared::PassContext> context
    ) :
        Pass(std::move(context)),
        scope() {
        //
    }

    void NameResolutionPass::visitModule(ionshared::Ptr<Module> node) {
        // TODO: Is it push_back() or push_front()?
        this->scope.push_back(node->context->getGlobalScope());
    }

    void NameResolutionPass::visitRef(PtrResolvable<> node) {
        // Node is already resolved, no need to continue.
        if (node->isResolved()) {
            return;
        }

        /**
         * NOTE: If the resolvable is not resolved, it's guaranteed
         * to have its kind, name and context defined.
         */
        ionshared::Ptr<Construct> owner = *node->context;
        std::string name = *node->name;

        auto throwUndefinedReference = [name]{
            throw std::runtime_error("Undefined reference to '" + name + "'");
        };

        switch (*node->resolvableKind) {
            case ResolvableKind::Variable: {
                // TODO: Must use flow graph to find variable declarations from other blocks (remember blocks can be nested).

                if (owner->constructKind != ConstructKind::Block) {
                    // TODO: Better error.
                    throw std::runtime_error("Cannot resolve variable declaration when owner is not a block");
                }

                auto ownerBlockSymbolTable = owner->dynamicCast<Block>()->symbolTable;
                auto valueLookupResult = ownerBlockSymbolTable->lookup(name);

                if (!ionshared::util::hasValue(valueLookupResult)) {
                    throwUndefinedReference();
                }

                node->resolve(*valueLookupResult);

                break;
            }

            case ResolvableKind::Prototype: {
                ionshared::OptPtr<Construct> lookupResult =
                    NameResolutionPass::findFunctionLikeTarget(name, owner);

                if (!ionshared::util::hasValue(lookupResult)) {
                    throwUndefinedReference();
                }

                node->resolve(*lookupResult);

                break;
            }

            case ResolvableKind::PrototypeReturnType: {
                ionshared::Ptr<Prototype> prototype;

                ionshared::OptPtr<Construct> functionLikeTargetResult =
                    NameResolutionPass::findFunctionLikeTarget(name, owner);

                if (!ionshared::util::hasValue(functionLikeTargetResult)) {
                    throwUndefinedReference;
                }

                ionshared::Ptr<Construct> functionLikeTarget =
                    *functionLikeTargetResult;

                if (functionLikeTarget->constructKind == ConstructKind::Function) {
                    prototype = functionLikeTarget->dynamicCast<Function>()->prototype;
                }
                else {
                    prototype = functionLikeTarget->dynamicCast<Extern>()->prototype;
                }

                node->resolve(prototype->returnType);

                break;
            }

            default: {
                // TODO: Better error.
                throw std::runtime_error("Unsupported construct kind to resolve");
            }
        }

        // TODO: CRITICAL: Recently solved the problem which was that it was using the basic block's own symbol table instead of the function's to find the basic block (Dummy mistake). Verify that this is actually how it should be.

        // TODO: Commented out as migrated from IonIR.
//        switch (owner->getConstructKind()) {
//            case ConstructKind::Inst: {
//                ionshared::Ptr<Inst> inst = owner->dynamicCast<Inst>();
//                ionshared::Ptr<BasicBlock> basicBlock = inst->getParent();
//                ionshared::PtrSymbolTable<Inst> basicBlockSymbolTable = basicBlock->getSymbolTable();
//                ionshared::Ptr<FunctionBody> functionBody = basicBlock->getParent();
//                PtrSymbolTable<BasicBlock> functionSymbolTable = functionBody->getSymbolTable();
//                ionshared::Ptr<Module> module = functionBody->getParent()->getPrototype()->getParent();
//                ionshared::PtrSymbolTable<Construct> moduleSymbolTable = module->getContext()->getGlobalScope();
//
//                /**
//                 * Check on the section's symbol table. It should take precedence
//                 * before the function's symbol table. This will look for instructions.
//                 */
//                if (basicBlockSymbolTable->contains(id)) {
//                    node->setValue(basicBlockSymbolTable->lookup(id));
//
//                    return;
//                }
//                    /**
//                     * Check on the function's symbol table for the referenced entity.
//                     * This will look for sections.
//                     */
//                else if (functionSymbolTable->contains(id)) {
//                    node->setValue(functionSymbolTable->lookup(id));
//
//                    return;
//                }
//                    /**
//                     * Finally, check on the module's symbol table for the referenced
//                     * entity. This will look for functions, externs, and globals.
//                     */
//                else if (moduleSymbolTable->contains(id)) {
//                    node->setValue(moduleSymbolTable->lookup(id));
//
//                    return;
//                }
//
//                // Otherwise, report an undefined reference error.
//                // TODO: Create NoticeContext using some sort of factory, for now throw error.
//                throw std::runtime_error("Undefined reference to '" + id + "'");
//            }
//
//                // TODO: Finish implementation.
//            default: {
//                throw std::runtime_error("Unhandled construct kind when trying to resolve reference");
//            }
//        }
    }

    void NameResolutionPass::visitScopeAnchor(ionshared::Ptr<ionshared::Scoped<Construct>> node) {
        // TODO: ScopeStack should be pushed & popped, but its never popped.
        // TODO: CRITICAL: Throwing SEGFAULT because node is NULL (casting fails).
        //        this->scopeStack.add(node->getSymbolTable());
    }

    const std::list<ionshared::PtrSymbolTable<Construct>> &NameResolutionPass::getScope() const {
        return this->scope;
    }
}
