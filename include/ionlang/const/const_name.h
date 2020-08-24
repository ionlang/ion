#pragma once

#include <string>

namespace ionlang {
    class ConstName {
    public:
        static const std::string anonymous;

        static const std::string main;

        static const std::string typeVoid;

        static const std::string typeBool;

        static const std::string typeInt8;

        static const std::string typeInt16;

        static const std::string typeInt32;

        static const std::string typeInt64;

        static const std::string typeUnsignedInt16;

        static const std::string typeUnsignedInt32;

        static const std::string typeUnsignedInt64;

        static const std::string typeFloat16;

        static const std::string typeFloat32;

        static const std::string typeFloat64;

        static const std::string typeChar;

        static const std::string typeString;

        static const std::string statementReturn;

        static const std::string statementIf;
    };
}
