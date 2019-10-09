#include "fault.hpp"
#include "args.hpp"
#include "cxxopts.hpp"
#include "flipper.hpp"
#include "sticker.hpp"
#include <random>

using namespace std;

namespace chaos
{
    namespace fault
    {
        unique_ptr<FaultInjector> get_injector(const fault_type fault_type, cxxopts::ParseResult& args, mt19937& rng)
        {
            switch (fault_type)
            {
            case flip:
                return unique_ptr<FaultInjector>(new BitFlipper(args, rng));
            case stuck:
                return unique_ptr<FaultInjector>(new BitSticker(args, rng));
            default:
                return nullptr;
            }
        }

        FaultInjector::FaultInjector(cxxopts::ParseResult& args, std::mt19937& rng) : rng_(rng)
        {
            inject_space_ = args::get_memory_space(args);

            if (args.count("mean-runtime"))
            {
                mean_runtime_ = args["mean-runtime"].as<long>();
            }
            else
            {
                mean_runtime_ = 0;
            }
        }
    }
}
