#include <iostream>
#include <sstream>
#include <string>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

// TODO: either make a class or output interoperable format for Python
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
    // TODO: fix error handling
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
    //std::cout << " Target triple: " << m->getTargetTriple() << std::endl;

    //  bb refers to the basic block number
    // globalVarState[std:make_pair("body", "y")] = Add(globalVarState[("l0","x")], 1)
    std::map< std::pair<std::string, std::string>, std::string> varState;
    // next_bb[body] = { 2, 3, 5 }
    std::map<std::string, std::set<std::string>> next_bb;
    std::map<std::string, std::set<std::string>> pred_bb;


    std::cout << " Global Variables: " << std::endl;
    for (Module::global_iterator iter = m->global_begin();
         iter != m->global_end(); iter++) {
      std::cout << "   Name: " << iter->getName().str() << std::endl;
    }

    int tmp_index = 0;
    int bb_index = 0;
    std::ostringstream sbuf;
    sbuf.str("");
    sbuf.clear();
    std::cout << " Function: main" << std::endl;
    Function *mainFunc = m->getFunction("main");
    // Loop over basic blocks in a function
    // Function &Func = ...
    for (BasicBlock &BB : *mainFunc) {
      // Print out the name of the basic block if it has one, and then the
      // number of instructions that it contains
        if (!BB.hasName()) {
            sbuf << "l" << bb_index++;
            BB.setName(sbuf.str());
            sbuf.str("");
            sbuf.clear();
        }
        std::string bb_name = BB.getName().str();
        errs() << "   Basic block (name=" << bb_name << ") has "
                   << BB.size() << " instructions.\n";

        // Loop over instructions in a basic block
        for (Instruction &I : BB) {
            // The next statement works since operator<<(ostream&,...)
            // is overloaded for Instruction&

            auto opcode = I.getOpcode();

            if (opcode != BinaryOperator::Store &&
                opcode != BinaryOperator::Br) {
                if (!I.hasName()) {
                    sbuf << "t" << tmp_index++;
                    I.setName(sbuf.str());
                    sbuf.str("");
                    sbuf.clear();
                }
                //errs() << "       Dest: " << I.getName() << "\n";

                for (int i=0; i < I.getNumOperands(); i++) {
                    auto operand = I.getOperand(i);
                    if (!operand->hasName()) {
                        sbuf << "t" << tmp_index++;
                        operand->setName(sbuf.str());
                        sbuf.str("");
                        sbuf.clear();
                    }
                    //errs() << "       Op " << i << ": " << operand->getName() << "\n";
                }
            }
            //errs() << "   " << I << "\n";
 
            if (opcode == BinaryOperator::Add ||
                opcode == BinaryOperator::Mul ||
                opcode == BinaryOperator::Sub) {
                auto left_val = I.getOperand(0);
                auto right_val = I.getOperand(1);

                std::string dest = I.getName().str();
                std::string left;
                std::string right;
                std::string op;

                if (ConstantInt* ci = dyn_cast<ConstantInt>(left_val)) {
                    left = std::to_string(ci->getSExtValue());
                    /*
                } else if (GlobalValue* gv = dyn_cast<GlobalValue>(left_val)) {
                    sbuf << "state[" << bb_name << "][" << left_val->getName().str() << "]";
                    left = sbuf.str();
                    sbuf.str("");
                    sbuf.clear();
                    */
                } else {
                    left = left_val->getName().str();
                }
                if (ConstantInt* ci = dyn_cast<ConstantInt>(right_val)) {
                    right = std::to_string(ci->getSExtValue());
                    /*
                } else if (GlobalValue* gv = dyn_cast<GlobalValue>(right_val)) {
                    sbuf << "state[" << bb_name << "][" << right_val->getName().str() << "]";
                    right = sbuf.str();
                    sbuf.str("");
                    sbuf.clear();
                    */
                } else {
                    right = right_val->getName().str();
                }
                
                if (opcode == BinaryOperator::Add) {
                    errs() << dest << " == Add(" << left << ", " << right << ")\n";
                } else if (opcode == BinaryOperator::Mul) {
                    errs() << dest << " == Mult(" << left << ", " << right << ")\n";
                } else if (opcode == BinaryOperator::Sub) {
                    errs() << dest << " == Sub(" << left << ", " << right << ")\n";
                }
            } else if (opcode == BinaryOperator::Load) {
                auto val = I.getOperand(0);
                std::string dest = I.getName().str();
                std::string global;
                sbuf << "state[" << bb_name << "][" << val->getName().str() << "]";
                global = sbuf.str();
                sbuf.str("");
                sbuf.clear();
                errs() << dest << " == " << global << "\n";
            } else if (opcode == BinaryOperator::Store) {
                auto val = I.getOperand(0);
                auto dest_val = I.getOperand(1);
                std::string dest = dest_val->getName().str();
                std::string val_str;
                if (ConstantInt* ci = dyn_cast<ConstantInt>(val)) {
                    val_str = std::to_string(ci->getSExtValue());
                } else {
                    val_str = val->getName().str();
                }
                errs() << "state[" << bb_name << "][" << dest << "] == " << val_str << "\n";
            } else if (opcode == BinaryOperator::Br) {
                if (I.getNumOperands() == 1) {
                    // unconditional jump
                    std::string dest = I.getOperand(0)->getName().str();
                    next_bb[bb_name].insert(dest);
                    pred_bb[dest].insert(bb_name);
                } else if (I.getNumOperands() == 3) {
                    // conditional jump
                    std::string left = I.getOperand(1)->getName().str();
                    std::string right = I.getOperand(2)->getName().str();
                    next_bb[bb_name].insert(left);
                    next_bb[bb_name].insert(right);
                    pred_bb[left].insert(bb_name);
                    pred_bb[right].insert(bb_name);
                }
            } else {
                // ignore instruction
                // other opcodes:
                // https://llvm.org/doxygen/group__LLVMCCoreTypes.html
                // https://github.com/llvm-mirror/llvm/blob/master/include/llvm/IR/Instruction.def
            }
        } // endfor
    } //endfor
}

/*
int badmain(int argc, char *argv[]) {

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
