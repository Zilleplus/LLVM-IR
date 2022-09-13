#pragma once
#include<string>

namespace infra
{
    struct NotImplemented
    {
        std::string location;
    };

    #define NOT_IMPLEMENTED throw NotImplemented{__FILE__}
}