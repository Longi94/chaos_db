#pragma once

#include <vector>
#include <random>
#include "fault.hpp"
#include "cxxopts.hpp"

namespace chaos
{
    namespace fault
    {
        struct stuck_bit
        {
            off_t address;
            int8_t mask;
            bool stick_to_1;
        };

        class BitSticker : public FaultInjector
        {
        public:
            BitSticker(cxxopts::ParseResult& args, std::mt19937& rng);

        private:
            double stuck_rate_;
            long byte_interval_;
            std::vector<stuck_bit> stuck_bits_;

        public:
            void inject(pid_t pid) override;

        private:
            /**
             * Get the number of stuck bits that need to be simulated.
             * @param memory_info memory info of child process id
             * @return stuck bit count
             */
            int get_stuck_bit_count(const std::unique_ptr<memory::heap_stack>& memory_info) const;
        };
    }
}
