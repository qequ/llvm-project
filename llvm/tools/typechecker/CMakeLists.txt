set(LLVM_LINK_COMPONENTS
  AllTargetsAsmParsers
  AllTargetsDescs
  AllTargetsInfos
  MC
  MCParser
  Support
  )

add_llvm_tool(typechecker
  main.cpp
  )
