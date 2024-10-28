//===- CIRCTModule.cpp - Main pybind module -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "circt/Bindings/Python/CIRCTModules.h"

#include "circt-c/Conversion.h"
#include "circt-c/Dialect/Comb.h"
#include "circt-c/Dialect/Debug.h"
#include "circt-c/Dialect/ESI.h"
#include "circt-c/Dialect/Emit.h"
#include "circt-c/Dialect/FSM.h"
#include "circt-c/Dialect/HW.h"
#include "circt-c/Dialect/HWArith.h"
#include "circt-c/Dialect/Handshake.h"
#include "circt-c/Dialect/LTL.h"
#include "circt-c/Dialect/MSFT.h"
#include "circt-c/Dialect/OM.h"
#include "circt-c/Dialect/RTG.h"
#include "circt-c/Dialect/RTGTest.h"
#include "circt-c/Dialect/SV.h"
#include "circt-c/Dialect/Seq.h"
#include "circt-c/Dialect/Verif.h"
#include "circt-c/ExportVerilog.h"
#include "circt-c/RTGPipeline.h"
#include "mlir-c/Bindings/Python/Interop.h"
#include "mlir-c/Dialect/Arith.h"
#include "mlir-c/IR.h"
#include "mlir-c/Transforms.h"
#include "mlir/Bindings/Python/PybindAdaptors.h"

#include "llvm-c/ErrorHandling.h"
#include "llvm/Support/Signals.h"

#include "PybindUtils.h"
#include <pybind11/pybind11.h>
namespace py = pybind11;

static void registerPasses() {
  registerCombPasses();
  registerSeqPasses();
  registerSVPasses();
  registerFSMPasses();
  registerHWArithPasses();
  registerHandshakePasses();
  mlirRegisterConversionPasses();
  mlirRegisterTransformsPasses();
}

PYBIND11_MODULE(_circt, m) {
  m.doc() = "CIRCT Python Native Extension";
  registerPasses();
  llvm::sys::PrintStackTraceOnErrorSignal(/*argv=*/"");
  LLVMEnablePrettyStackTrace();

  m.def(
      "register_dialects",
      [](py::object capsule) {
        // Get the MlirContext capsule from PyMlirContext capsule.
        auto wrappedCapsule = capsule.attr(MLIR_PYTHON_CAPI_PTR_ATTR);
        MlirContext context = mlirPythonCapsuleToContext(wrappedCapsule.ptr());

        // Collect CIRCT dialects to register.
        MlirDialectHandle comb = mlirGetDialectHandle__comb__();
        mlirDialectHandleRegisterDialect(comb, context);
        mlirDialectHandleLoadDialect(comb, context);

        MlirDialectHandle debug = mlirGetDialectHandle__debug__();
        mlirDialectHandleRegisterDialect(debug, context);
        mlirDialectHandleLoadDialect(debug, context);

        MlirDialectHandle emit = mlirGetDialectHandle__emit__();
        mlirDialectHandleRegisterDialect(emit, context);
        mlirDialectHandleLoadDialect(emit, context);

        MlirDialectHandle esi = mlirGetDialectHandle__esi__();
        mlirDialectHandleRegisterDialect(esi, context);
        mlirDialectHandleLoadDialect(esi, context);

        MlirDialectHandle msft = mlirGetDialectHandle__msft__();
        mlirDialectHandleRegisterDialect(msft, context);
        mlirDialectHandleLoadDialect(msft, context);

        MlirDialectHandle hw = mlirGetDialectHandle__hw__();
        mlirDialectHandleRegisterDialect(hw, context);
        mlirDialectHandleLoadDialect(hw, context);

        MlirDialectHandle hwarith = mlirGetDialectHandle__hwarith__();
        mlirDialectHandleRegisterDialect(hwarith, context);
        mlirDialectHandleLoadDialect(hwarith, context);

        MlirDialectHandle om = mlirGetDialectHandle__om__();
        mlirDialectHandleRegisterDialect(om, context);
        mlirDialectHandleLoadDialect(om, context);

        MlirDialectHandle rtg = mlirGetDialectHandle__rtg__();
        mlirDialectHandleRegisterDialect(rtg, context);
        mlirDialectHandleLoadDialect(rtg, context);

        MlirDialectHandle rtgtest = mlirGetDialectHandle__rtgtest__();
        mlirDialectHandleRegisterDialect(rtgtest, context);
        mlirDialectHandleLoadDialect(rtgtest, context);

        MlirDialectHandle seq = mlirGetDialectHandle__seq__();
        mlirDialectHandleRegisterDialect(seq, context);
        mlirDialectHandleLoadDialect(seq, context);

        MlirDialectHandle sv = mlirGetDialectHandle__sv__();
        mlirDialectHandleRegisterDialect(sv, context);
        mlirDialectHandleLoadDialect(sv, context);

        MlirDialectHandle fsm = mlirGetDialectHandle__fsm__();
        mlirDialectHandleRegisterDialect(fsm, context);
        mlirDialectHandleLoadDialect(fsm, context);

        MlirDialectHandle handshake = mlirGetDialectHandle__handshake__();
        mlirDialectHandleRegisterDialect(handshake, context);
        mlirDialectHandleLoadDialect(handshake, context);

        MlirDialectHandle ltl = mlirGetDialectHandle__ltl__();
        mlirDialectHandleRegisterDialect(ltl, context);
        mlirDialectHandleLoadDialect(ltl, context);

        MlirDialectHandle verif = mlirGetDialectHandle__verif__();
        mlirDialectHandleRegisterDialect(verif, context);
        mlirDialectHandleLoadDialect(verif, context);

        MlirDialectHandle arith = mlirGetDialectHandle__arith__();
        mlirDialectHandleRegisterDialect(arith, context);
        mlirDialectHandleLoadDialect(arith, context);
      },
      "Register CIRCT dialects on a PyMlirContext.");

  m.def("export_verilog", [](MlirModule mod, py::object fileObject) {
    circt::python::PyFileAccumulator accum(fileObject, false);
    py::gil_scoped_release();
    mlirExportVerilog(mod, accum.getCallback(), accum.getUserData());
  });

  m.def("export_split_verilog", [](MlirModule mod, std::string directory) {
    auto cDirectory = mlirStringRefCreateFromCString(directory.c_str());
    mlirExportSplitVerilog(mod, cDirectory);
  });

  m.def("generate_random_tests",
        [](MlirModule mod, bool verifyPasses, bool verbosePassExecution,
           bool hasSeed, unsigned seed, py::list unsupportedInstructions,
           std::string unsupportedInstructionsFile, std::string outputFormat,
           py::object fileObject) {
          circt::python::PyFileAccumulator accum(fileObject, false);
          py::gil_scoped_release();

          char **c_array = new char *[unsupportedInstructions.size() + 1];

          for (size_t i = 0; i < unsupportedInstructions.size(); ++i)
            c_array[i] = strdup(unsupportedInstructions[i]
                                    .cast<std::string>()
                                    .c_str()); // strdup allocates and copies

          c_array[unsupportedInstructions.size()] = nullptr;

          auto stringToOutputFormat = [](std::string s) {
            if (s == "mlir")
              return CirctRTGOutputMLIR;
            if (s == "rendered")
              return CirctRTGOutputRenderedMLIR;
            if (s == "asm")
              return CirctRTGOutputASM;
            if (s == "elf")
              return CirctRTGOutputELF;

            // Set ASM as default
            return CirctRTGOutputASM;
          };

          circtGenerateRandomTests(mod, verifyPasses, verbosePassExecution,
                                   hasSeed, seed,
                                   unsupportedInstructions.size(), c_array,
                                   unsupportedInstructionsFile.c_str(),
                                   stringToOutputFormat(outputFormat),
                                   accum.getCallback(), accum.getUserData());

          for (size_t i = 0; c_array[i] != nullptr; ++i)
            free(c_array[i]);

          delete[] c_array;
        });

  py::module esi = m.def_submodule("_esi", "ESI API");
  circt::python::populateDialectESISubmodule(esi);
  py::module msft = m.def_submodule("_msft", "MSFT API");
  circt::python::populateDialectMSFTSubmodule(msft);
  py::module hw = m.def_submodule("_hw", "HW API");
  circt::python::populateDialectHWSubmodule(hw);
  py::module seq = m.def_submodule("_seq", "Seq API");
  circt::python::populateDialectSeqSubmodule(seq);
  py::module om = m.def_submodule("_om", "OM API");
  circt::python::populateDialectOMSubmodule(om);
  py::module rtg = m.def_submodule("_rtg", "RTG API");
  circt::python::populateDialectRTGSubmodule(rtg);
  py::module sv = m.def_submodule("_sv", "SV API");
  circt::python::populateDialectSVSubmodule(sv);
  py::module support = m.def_submodule("_support", "CIRCT support");
  circt::python::populateSupportSubmodule(support);
}
