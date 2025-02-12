add_subdirectory(VerilogServerImpl)

add_circt_library(CIRCTVerilogLspServerLib
  LSPServer.cpp
  LSPUtils.cpp
  CirctVerilogLspServerMain.cpp

  ADDITIONAL_HEADER_DIRS
  ${MLIR_MAIN_INCLUDE_DIR}/circt/Tools/crct-verilog-lsp-server

  DEPENDS
  slang_slang

  LINK_LIBS PUBLIC
  MLIRLspServerSupportLib
  CIRCTVerilogLspServerImpl
  PRIVATE
  slang_slang
)
