#pragma once

#include <ionshared/misc/helpers.h>
#include <ionshared/passes/base_pass.h>
#include <ionlang/construct/pseudo/ref.h>
#include <ionlang/construct/construct.h>
#include <ionlang/construct/module.h>
#include <ionlang/construct/prototype.h>
#include <ionlang/construct/extern.h>
#include <ionlang/construct/basic_block.h>
#include <ionlang/construct/statement.h>
#include <ionlang/construct/value.h>
#include <ionlang/construct/value/boolean_value.h>
#include <ionlang/construct/value/char_value.h>
#include <ionlang/construct/value/integer_value.h>
#include <ionlang/construct/value/string_value.h>
#include <ionlang/construct/type.h>
#include <ionlang/construct/type/void_type.h>
#include <ionlang/construct/function.h>
#include <ionlang/construct/variable_declaration.h>
#include <ionlang/construct/global.h>

namespace ionlang {
    class Pass : public ionshared::BasePass<Construct> {
    public:
        virtual void visit(ionshared::Ptr<Construct> node);

        virtual void visitChildren(ionshared::Ptr<Construct> node);

        virtual void visitModule(ionshared::Ptr<Module> node);

        virtual void visitPrototype(ionshared::Ptr<Prototype> node);

        virtual void visitExtern(ionshared::Ptr<Extern> node);

        virtual void visitBasicBlock(ionshared::Ptr<BasicBlock> node);

        virtual void visitStatement(ionshared::Ptr<Statement> node);

        virtual void visitValue(ionshared::Ptr<Value<>> node);

        virtual void visitIntegerValue(ionshared::Ptr<IntegerValue> node);

        virtual void visitCharValue(ionshared::Ptr<CharValue> node);

        virtual void visitStringValue(ionshared::Ptr<StringValue> node);

        virtual void visitBooleanValue(ionshared::Ptr<BooleanValue> node);

        virtual void visitType(ionshared::Ptr<Type> node);

        virtual void visitVoidType(ionshared::Ptr<VoidType> node);

        virtual void visitRef(PtrRef<> node);

        virtual void visitFunction(ionshared::Ptr<Function> node);

        virtual void visitVariableDecl(ionshared::Ptr<VariableDeclaration> node);

        virtual void visitScopeAnchor(ionshared::Ptr<ionshared::ScopeAnchor<Construct>> node);

        virtual void visitFunctionBody(ionshared::Ptr<FunctionBody> node);

        virtual void visitIntegerType(ionshared::Ptr<IntegerType> node);

        virtual void visitGlobal(ionshared::Ptr<Global> node);
    };
}
