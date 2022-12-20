#include <iostream>
#include <string>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

int main(int argc, char *argv[]) {
    // Error and produce help message if user did not provide a filename to translate.
    if (argc != 2) {
      std::cerr << "Usage: " << argv[0] << "bitcode_filename" << std::endl;
      return 1;
    }

    StringRef filename = argv[1];
    LLVMContext context;

    ErrorOr<std::unique_ptr<MemoryBuffer>> fileOrErr =
      MemoryBuffer::getFileOrSTDIN(filename);
    /*
    if (std::error_code ec = fileOrErr.getError()) {
      std::cerr << " Error opening input file: " + ec.message() << std::endl;
      return 2;
    }
    */
    Expected<std::unique_ptr<Module>> moduleOrErr =
        parseBitcodeFile(fileOrErr.get()->getMemBufferRef(), context);
    /*
    if (std::error_code ec = fileOrErr.getError()) {
      std::cerr << "Error reading Moduule: " + ec.message() << std::endl;
      return 3;
    }
    */
    std::unique_ptr<Module> m (std::move(moduleOrErr.get()));
    std::cout << "Successfully read Module:" << std::endl;
    std::cout << " Name: " << m->getName().str() << std::endl;
    std::cout << " Target triple: " << m->getTargetTriple() << std::endl;

    //  bb refers to the basic block number
    // globalVarState[std:make_pair(bb, "y")] = Add(globalVarState[(bb_prev,"x")], 1)
    //std::map< std::pair<BasicBlock &, std::string>, SMT_expr> globalVarState;
    // nextState[1] = { 2, 3, 5 }
    //std::map<BasicBlock &, std::set<BasicBlock &>> nextState;

    std::cout << " Global Variables: " << std::endl;
    for (Module::global_iterator iter = m->global_begin();
         iter != m->global_end(); iter++) {
      std::cout << "   Name: " << iter->getName().str() << std::endl;
    }

    std::cout << " Function: main" << std::endl;
    Function *mainFunc = m->getFunction("main");
    // Loop over basic blocks in a function
    // Function &Func = ...
    for (BasicBlock &BB : *mainFunc) {
      // Print out the name of the basic block if it has one, and then the
      // number of instructions that it contains
      errs() << "   Basic block (name=" << BB.getName() << ") has "
                 << BB.size() << " instructions.\n";

        // Loop over instructions in a basic block
        for (Instruction &I : BB) {
           // The next statement works since operator<<(ostream&,...)
           // is overloaded for Instruction&
           errs() << "   " << I << "\n";
        }
    }
}

/*
int badmain(int argc, char *argv[]) {

  Module *m = moduleOrErr.get();
  std::cout << "Successfully read Module:" << std::endl;
  std::cout << " Name: " << m->getName().str() << std::endl;
  std::cout << " Target triple: " << m->getTargetTriple() << std::endl;

  for (auto iter1 = m->getFunctionList().begin();
       iter1 != m->getFunctionList().end(); iter1++) {
    Function &f = *iter1;
    std::cout << " Function: " << f.getName().str() << std::endl;
    for (auto iter2 = f.getBasicBlockList().begin();
         iter2 != f.getBasicBlockList().end(); iter2++) {
      BasicBlock &bb = *iter2;
      std::cout << "  BasicBlock: " << bb.getName().str() << std::endl;
      for (auto iter3 = bb.begin(); iter3 != bb.end(); iter3++) {
        Instruction &inst = *iter3;
        std::cout << "   Instruction " << &inst << " : " << inst.getOpcodeName();

    unsigned int  i = 0;
    unsigned int opnt_cnt = inst.getNumOperands();
        for(; i < opnt_cnt; ++i)
        {
          Value *opnd = inst.getOperand(i);
          std::string o;
          //          raw_string_ostream os(o);
          //         opnd->print(os);
          //opnd->printAsOperand(os, true, m);
          if (opnd->hasName()) {
            o = opnd->getName();
            std::cout << " " << o << "," ;
          } else {
            std::cout << " ptr" << opnd << ",";
          }
        }
        std:: cout << std::endl;
      }
    }
  }
  return 0;
}
*/
