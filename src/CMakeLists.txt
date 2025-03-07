set(SOURCES
    main.cpp
    lexer/lexer.cpp
    parser/parser.cpp
    ast/ast.cpp
    semantic/semantic_analyzer.cpp
    ir/ir_builder.cpp
    optimizer/optimizer.cpp
    codegen/code_generator.cpp
)

add_executable(minicompiler ${SOURCES})

target_include_directories(minicompiler PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 安装目标
install(TARGETS minicompiler DESTINATION bin) 