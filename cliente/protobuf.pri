#
# Qt qmake integration with Google Protocol Buffers compiler protoc
#
# To compile protocol buffers with qt qmake, specify PROTOS variable and
# include this file
#
# Example:
# PROTOS = a.proto b.proto
# include(protobuf.pri)
# LIBS += -lprotobuf
#

message("Generating protocol buffer classes from .proto files.")

protobuf_decl.name = protobuf header
protobuf_decl.input = PROTOS
protobuf_decl.output = ${QMAKE_FILE_BASE}.pb.h
protobuf_decl.commands = protoc --cpp_out="." --proto_path=${QMAKE_FILE_IN_PATH} ${QMAKE_FILE_NAME}
protobuf_decl.variable_out = HEADERS
QMAKE_EXTRA_COMPILERS += protobuf_decl

protobuf_impl.name = protobuf implementation
protobuf_impl.input = PROTOS
protobuf_impl.output = ${QMAKE_FILE_BASE}.pb.cc
protobuf_impl.depends = ${QMAKE_FILE_BASE}.pb.h
protobuf_impl.commands = $$escape_expand(\n)
protobuf_impl.variable_out = SOURCES
QMAKE_EXTRA_COMPILERS += protobuf_impl

LIBS += /usr/local/lib/libprotobuf.so
