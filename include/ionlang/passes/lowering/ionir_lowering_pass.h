#pragma once

#include <ionshared/container/stack.h>
#include <ionir/construct/basic_block.h>
#include <ionlang/misc/ionir_emitted_entities.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    class IonIrLoweringPass : public Pass {
    private:
        ionshared::PtrSymbolTable<ionir::Module> modules;

        ionshared::Stack<ionshared::Ptr<ionir::Construct>> constructStack;

        ionshared::Stack<ionshared::Ptr<ionir::Type>> typeStack;

        /**
         * The currently active IonIR module.
         */
        ionshared::OptPtr<ionir::Module> moduleBuffer;

        ionshared::OptPtr<ionir::Function> functionBuffer;

        ionshared::OptPtr<ionir::BasicBlock> basicBlockBuffer;

        ionshared::OptPtr<ionir::InstBuilder> builderBuffer;

        IonIrEmittedEntities symbolTable;

        uint32_t nameCounter;

        ionshared::Ptr<ionir::Module> requireModule();

        ionshared::Ptr<ionir::Function> requireFunction();

        ionshared::Ptr<ionir::BasicBlock> requireBasicBlock();

        ionshared::Ptr<ionir::InstBuilder> requireBuilder();

        /**
         * Set the currently active builder if any. Modifying the builder
         * will also set/update the active IonIR basic block buffer.
         */
        void setBuilder(ionshared::Ptr<ionir::BasicBlock> basicBlock);

        void lockBuilder(const std::function<void()> &callback);

        [[nodiscard]] uint32_t getNameCounter() noexcept;

    public:
        explicit IonIrLoweringPass(
            ionshared::PtrSymbolTable<ionir::Module> modules =
                std::make_shared<ionshared::SymbolTable<ionshared::Ptr<ionir::Module>>>()
        );

        ~IonIrLoweringPass();

        ionshared::Ptr<ionshared::SymbolTable<ionshared::Ptr<ionir::Module>>> getModules() const;

        ionshared::Stack<ionshared::Ptr<ionir::Construct>> getConstructStack() const noexcept;

        ionshared::Stack<ionshared::Ptr<ionir::Type>> getTypeStack() const noexcept;

        ionshared::OptPtr<ionir::Module> getModuleBuffer() const;

        bool setModuleBuffer(const std::string &id);

        void visit(ionshared::Ptr<Construct> node) override;

        void visitModule(ionshared::Ptr<Module> node) override;

        void visitFunction(ionshared::Ptr<Function> node) override;

        void visitExtern(ionshared::Ptr<Extern> node) override;

        void visitPrototype(ionshared::Ptr<Prototype> node) override;

        void visitBlock(ionshared::Ptr<Block> node) override;

        void visitIntegerValue(ionshared::Ptr<IntegerLiteral> node) override;

        void visitCharValue(ionshared::Ptr<CharLiteral> node) override;

        void visitStringValue(ionshared::Ptr<StringLiteral> node) override;

        void visitBooleanValue(ionshared::Ptr<BooleanLiteral> node) override;

        void visitGlobal(ionshared::Ptr<Global> node) override;

        void visitType(ionshared::Ptr<Type> node) override;

        void visitIntegerType(ionshared::Ptr<IntegerType> node) override;

        void visitBooleanType(ionshared::Ptr<BooleanType> node) override;

        void visitVoidType(ionshared::Ptr<VoidType> node) override;

        void visitIfStatement(ionshared::Ptr<IfStatement> node) override;

        void visitReturnStatement(ionshared::Ptr<ReturnStatement> node) override;

        void visitAssignmentStatement(ionshared::Ptr<AssignmentStatement> node) override;

        void visitVariableDecl(ionshared::Ptr<VariableDeclStatement> node) override;

        void visitCallExpr(ionshared::Ptr<CallExpr> node) override;

        // TODO: visitRef() if !isResolved() error, else this->visit(ref->getValue()); Is this a good idea? It may be required for example for if statement condition (if it's a ref). Investigate.
    };
}
