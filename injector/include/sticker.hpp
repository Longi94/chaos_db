#pragma once

#include <random>
#include "fault.hpp"
#include "cxxopts.hpp"

namespace chaos
{
    namespace fault
    {
        class BitSticker : public FaultInjector
        {
        public:
            BitSticker(cxxopts::ParseResult& args, std::mt19937& rng);

        private:
            double stuck_rate_;

        public:
            void inject(pid_t pid) override;
        };
    }
}
