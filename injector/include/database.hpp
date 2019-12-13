#pragma once

#include "fault.hpp"

namespace chaos
{
    namespace database
    {
        void save_result(std::string& db_name, std::unique_ptr<fault::result>& result);
    }
}
