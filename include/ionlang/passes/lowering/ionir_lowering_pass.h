#pragma once

#include <ionshared/container/stack.h>
#include <ionir/construct/basic_block.h>
#include <ionlang/misc/ionir_emitted_entities.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    class IonIrLoweringPass : public Pass {
    private:
        struct Buffers {
            ionshared::OptPtr<ionir::Module> module = std::nullopt;

            ionshared::OptPtr<ionir::Function> function = std::nullopt;

            ionshared::OptPtr<ionir::BasicBlock> basicBlock = std::nullopt;

            ionshared::OptPtr<ionir::InstBuilder> builder = std::nullopt;
        };

        [[nodiscard]] static ionshared::Ptr<ionir::Type> processTypeQualifiers(
            ionshared::Ptr<ionir::Type> type,
            const ionshared::Ptr<TypeQualifiers> &qualifiers
        );

        ionshared::PtrSymbolTable<ionir::Module> modules;

        ionshared::Stack<ionshared::Ptr<ionir::Construct>> constructStack;

        ionshared::Stack<ionshared::Ptr<ionir::Type>> typeStack;

        Buffers buffers;

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

        void lockBuffers(const std::function<void()> &callback);

        [[nodiscard]] uint32_t getNameCounter() noexcept;

    public:
        explicit IonIrLoweringPass(
            ionshared::Ptr<ionshared::PassContext> context,

            ionshared::PtrSymbolTable<ionir::Module> modules =
                std::make_shared<ionshared::SymbolTable<ionshared::Ptr<ionir::Module>>>()
        );

        ~IonIrLoweringPass();

        [[nodiscard]] ionshared::Ptr<ionshared::SymbolTable<ionshared::Ptr<ionir::Module>>> getModules() const;

        [[nodiscard]] ionshared::Stack<ionshared::Ptr<ionir::Construct>> getConstructStack() const noexcept;

        [[nodiscard]] ionshared::Stack<ionshared::Ptr<ionir::Type>> getTypeStack() const noexcept;

        [[nodiscard]] ionshared::OptPtr<ionir::Module> getModuleBuffer() const;

        bool setModuleBuffer(const std::string &id);

        void visit(ionshared::Ptr<Construct> node) override;

        void visitModule(ionshared::Ptr<Module> node) override;

        void visitFunction(ionshared::Ptr<Function> node) override;

        void visitExtern(ionshared::Ptr<Extern> node) override;

        void visitPrototype(ionshared::Ptr<Prototype> node) override;

        void visitBlock(ionshared::Ptr<Block> node) override;

        void visitIntegerLiteral(ionshared::Ptr<IntegerLiteral> node) override;

        void visitCharLiteral(ionshared::Ptr<CharLiteral> node) override;

        void visitStringLiteral(ionshared::Ptr<StringLiteral> node) override;

        void visitBooleanLiteral(ionshared::Ptr<BooleanLiteral> node) override;

        void visitGlobal(ionshared::Ptr<Global> node) override;

        void visitIntegerType(ionshared::Ptr<IntegerType> node) override;

        void visitBooleanType(ionshared::Ptr<BooleanType> node) override;

        void visitVoidType(ionshared::Ptr<VoidType> node) override;

        void visitIfStatement(ionshared::Ptr<IfStatement> node) override;

        void visitReturnStatement(ionshared::Ptr<ReturnStatement> node) override;

        void visitAssignmentStatement(ionshared::Ptr<AssignmentStatement> node) override;

        void visitVariableDecl(ionshared::Ptr<VariableDeclStatement> node) override;

        void visitCallExpr(ionshared::Ptr<CallExpr> node) override;

        void visitBinaryOperation(ionshared::Ptr<BinaryOperation> node) override;

        void visitStruct(ionshared::Ptr<Struct> node) override;

        // TODO: visitRef() if !isResolved() error, else this->visit(ref->getValue()); Is this a good idea? It may be required for example for if statement condition (if it's a ref). Investigate.
    };
}
