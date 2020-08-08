#include <ionlang/passes/pass.h>

namespace ionlang {
    void Pass::visit(ionshared::Ptr<Construct> node) {
        node->accept(*this);

        // TODO: Hotfix for circular dep.
        if (node->getConstructKind() == ConstructKind::Ref) {
            this->visitRef(node->staticCast<Ref<>>());

            // No need to visit children for this node.
            return;
        }
        else if (node->getConstructKind() == ConstructKind::Value) {
            this->visitValue(node->staticCast<Value<>>());

            // No need to visit children for this node.
            return;
        }

        this->visitChildren(node);
    }

    void Pass::visitChildren(ionshared::Ptr<Construct> node) {
        // TODO: Will it cause StackOverflow error with large ASTs?
        /**
         * After visiting the node, attempt to
         * visit all its children as well.
         */
        for (const auto child : node->getChildNodes()) {
            // TODO: CRITICAL: What if 'child' (AstNode) is not boxed under Construct?
            this->visit(child->staticCast<Construct>());
        }
    }

    void Pass::visitModule(ionshared::Ptr<Module> node) {
        //
    }

    void Pass::visitPrototype(ionshared::Ptr<Prototype> node) {
        //
    }

    void Pass::visitExtern(ionshared::Ptr<Extern> node) {
        //
    }

    void Pass::visitBasicBlock(ionshared::Ptr<BasicBlock> node) {
        //
    }

    void Pass::visitStatement(ionshared::Ptr<Statement> node) {
        //
    }

    void Pass::visitValue(ionshared::Ptr<Value<>> node) {
        switch (node->getValueKind()) {
            // TODO: Missing boolean value kind.

            case ValueKind::Character: {
                this->visitCharValue(node->dynamicCast<CharValue>());

                break;
            }

            case ValueKind::Integer: {
                this->visitIntegerValue(node->dynamicCast<IntegerValue>());

                break;
            }

            case ValueKind::String: {
                this->visitStringValue(node->dynamicCast<StringValue>());

                break;
            }

            case ValueKind::Boolean: {
                this->visitBooleanValue(node->dynamicCast<BooleanValue>());

                break;
            }

            default: {
                throw std::runtime_error("Unknown value kind");
            }
        }
    }

    void Pass::visitType(ionshared::Ptr<Type> node) {
        //
    }
}
