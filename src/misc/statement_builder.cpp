#include <ionlang/misc/statement_builder.h>

namespace ionlang {
    StatementBuilder::StatementBuilder(ionshared::Ptr<Block> basicBlock) :
        basicBlock(std::move(basicBlock)) {
        //
    }

    ionshared::Ptr<Block> StatementBuilder::getSection() const noexcept {
        return this->basicBlock;
    }

    void StatementBuilder::insert(const ionshared::Ptr<Statement> &inst) {
        this->basicBlock->getStatements().push_back(inst);
    }

    // TODO
//    ionshared::Ptr<ReturnInst> InstBuilder::createReturn(ionshared::OptPtr<Value<>> value) {
//        return this->make<ReturnInst, ReturnInstOpts>(ReturnInstOpts{
//            this->basicBlock,
//            value
//        });
//    }
}
