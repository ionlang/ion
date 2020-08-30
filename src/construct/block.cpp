#include <utility>
#include <ionlang/passes/pass.h>
#include <ionlang/misc/statement_builder.h>

namespace ionlang {
    Block::Block(ionshared::Ptr<Construct> parent, std::vector<ionshared::Ptr<Statement>> statements, ionshared::PtrSymbolTable<Statement> symbolTable)
        : ChildConstruct<Construct>(std::move(parent), ConstructKind::Block), ionshared::ScopeAnchor<Statement>(std::move(symbolTable)), statements(std::move(statements)) {
        //
    }

    void Block::accept(Pass &visitor) {
        visitor.visitScopeAnchor(this->dynamicCast<ionshared::ScopeAnchor<Construct>>());
        visitor.visitBlock(this->dynamicCast<Block>());
    }

    Ast Block::getChildNodes() {
        return Construct::convertChildren(this->statements);
    }

    std::vector<ionshared::Ptr<Statement>> &Block::getStatements() noexcept {
        return this->statements;
    }

    void Block::setStatements(std::vector<ionshared::Ptr<Statement>> statements) {
        this->statements = std::move(statements);
    }

    void Block::insertStatement(const ionshared::Ptr<Statement> &statement) {
        this->statements.push_back(statement);

        bool isNamed = false;

        // Certain types of statements are named.
        if (statement->getStatementKind() == StatementKind::VariableDeclaration) {
            isNamed = true;
        }

        // Register the named statement in the symbol table, if applicable.
        if (isNamed) {
            ionshared::Ptr<ionshared::Named> namedStatement =
                statement->dynamicCast<ionshared::Named>();

            this->getSymbolTable()->insert(namedStatement->getId(), statement);
        }
    }

    uint32_t Block::relocateStatements(Block &target, uint32_t from) {
        uint32_t count = 0;

        for (uint32_t i = from; i < this->statements.size(); i++) {
            target.getStatements().push_back(this->statements[i]);
            this->statements.erase(this->statements.begin() + i - 1);
            count++;
        }

        return count;
    }

    std::optional<uint32_t> Block::locate(ionshared::Ptr<Statement> construct) const {
        return ionshared::util::locateInVector<ionshared::Ptr<Statement>>(
            this->statements,
            std::move(construct)
        );
    }

    ionshared::Ptr<StatementBuilder> Block::createBuilder() {
        return std::make_shared<StatementBuilder>(this->dynamicCast<Block>());
    }

    ionshared::OptPtr<Statement> Block::findTerminalStatement() const {
        for (const auto &statement : this->statements) {
            if (statement->isTerminal()) {
                return statement;
            }
        }

        return std::nullopt;
    }

    bool Block::isFunctionBody() const {
        ionshared::Ptr<Construct> parent = this->getParent();

        // TODO: Better way to know if this is a function body.
        if (parent == nullptr) {
            throw std::runtime_error("Unexpected parent construct to be nullptr");
        }

        /**
         * A block is considered a function body if it's parent is
         * a function construct.
         */
        return parent->getConstructKind() == ConstructKind::Function;
    }
}
