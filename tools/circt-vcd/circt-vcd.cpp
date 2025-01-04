//===- circt-vcd.cpp - The vcd driver ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
//===----------------------------------------------------------------------===//

#include "circt/InitAllDialects.h"
#include "circt/Support/Version.h"

#include "circt/Dialect/HW/HWInstanceGraph.h"
#include "circt/Dialect/SV/SVOps.h"
#include "circt/Support/InstanceGraph.h"
#include "circt/Support/VCD.h"
#include "mlir/IR/Diagnostics.h"
#include "mlir/IR/OwningOpRef.h"
#include "mlir/Parser/Parser.h"
#include "mlir/Support/FileUtilities.h"
#include "mlir/Support/LogicalResult.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/ToolOutputFile.h"

#define DEBUG_TYPE "circt-vcd"

using namespace llvm;
using namespace mlir;
using namespace circt;

//===----------------------------------------------------------------------===//
// Command-line options declaration
//===----------------------------------------------------------------------===//

static cl::OptionCategory mainCategory("circt-generate-lec-mapping Options");

static cl::opt<std::string> inputVCD(cl::Positional,
                                    cl::desc("path to dut module"),
                                    cl::cat(mainCategory));
static cl::opt<std::string> outputFilename("o", cl::desc("Output name"),
                                           cl::value_desc("name"),
                                           cl::init("-"),
                                           cl::cat(mainCategory));
static cl::opt<bool> emitVCDDataStructure("emit-vcd-data-structure",
                                          cl::desc("Emit VCD data structure"),
                                          cl::init(false),
                                          cl::cat(mainCategory));

//===----------------------------------------------------------------------===//
// Tool implementation
//===----------------------------------------------------------------------===//



/// This functions initializes the various components of the tool and
/// orchestrates the work to be done.
static LogicalResult execute(MLIRContext &context) {

  std::string errorMessage;
  auto input = mlir::openInputFile(inputVCD, &errorMessage);
  if (!input) {
    errs() << errorMessage << "\n";
    return failure();
  }

  llvm::SourceMgr vcdSourceMgr;
  vcdSourceMgr.AddNewSourceBuffer(std::move(input), llvm::SMLoc());
  SourceMgrDiagnosticVerifierHandler sourceMgrHandler(vcdSourceMgr, &context);
  context.printOpOnDiagnostic(false);

  auto vcdFile = circt::vcd::importVCDFile(vcdSourceMgr, &context);
  if (!vcdFile) {
    errs() << "failed to parse input vcd file `" << inputVCD << "`\n";
    return failure();
  }
  auto output = openOutputFile(outputFilename, &errorMessage);
  if (!output) {
    errs() << errorMessage;
    return failure();
  }


  mlir::raw_indented_ostream os(output->os());
  if (emitVCDDataStructure) {
    vcdFile->dump(os);
    output->keep();
    return success();
  }

  vcdFile->printVCD(os);
  output->keep();
  return success();
}

/// The entry point for the `circt-vcd` tool:
/// configures and parses the command-line options,
/// registers all dialects within a MLIR context,
/// and calls the `execute` function to do the actual work.
int main(int argc, char **argv) {
  // Configure the relevant command-line options.
  cl::HideUnrelatedOptions(mainCategory);
  registerMLIRContextCLOptions();
  cl::AddExtraVersionPrinter(
      [](llvm::raw_ostream &os) { os << circt::getCirctVersion() << '\n'; });

  // Parse the command-line options provided by the user.
  cl::ParseCommandLineOptions(argc, argv,
                              "circt-vcd - parse vcd "
                              "waveforms\n\n");

  // Set the bug report message to indicate users should file issues on
  // llvm/circt and not llvm/llvm-project.
  llvm::setBugReportMsg(circt::circtBugReportMsg);

  // Register the supported CIRCT dialects and create a context to work with.
  MLIRContext context;
  context.loadDialect<comb::CombDialect>();
  context.loadDialect<emit::EmitDialect>();
  context.loadDialect<hw::HWDialect>();
  context.loadDialect<om::OMDialect>();
  context.loadDialect<sv::SVDialect>();
  context.loadDialect<ltl::LTLDialect>();
  // context.loadDialect<verif::VerifDialect>();

  // Setup of diagnostic handling.
  llvm::SourceMgr sourceMgr;
  SourceMgrDiagnosticHandler sourceMgrHandler(sourceMgr, &context);
  // Avoid printing a superfluous note on diagnostic emission.
  context.printOpOnDiagnostic(false);

  exit(failed(execute(context)));
}
