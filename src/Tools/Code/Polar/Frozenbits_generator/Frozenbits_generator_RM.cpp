#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cli.hpp>
#include <cmath>
#include <cstdint>
#include <streampu.hpp>
#include <string>
#include <vector>

#include "Tools/Code/Polar/Frozenbits_generator/Frozenbits_generator_RM.hpp"

using namespace aff3ct::tools;

Frozenbits_generator_RM ::Frozenbits_generator_RM(const int K, const int N)
  : Frozenbits_generator_file(K, N, cli::modify_path<cli::Is_file>("conf/cde/awgn_polar_codes/RM/N_65536.pc"))
  , m((int)std::log2(N))
{
    std::string message = "RM frozen bits generation undefined for N > " + std::to_string(N_max);
    if (N > N_max) throw spu::tools::invalid_argument(__FILE__, __LINE__, __func__, message.c_str());
}

Frozenbits_generator_RM ::~Frozenbits_generator_RM() {}

Frozenbits_generator_RM*
Frozenbits_generator_RM ::clone() const
{
    auto t = new Frozenbits_generator_RM(*this);
    return t;
}

void
Frozenbits_generator_RM ::evaluate()
{
    std::vector<uint32_t> best_channels_mother(N_max);
    std::cout << "Inside the RMPolar frozen bits\n";

    if (!load_channels_file(filename, best_channels_mother))
        throw spu::tools::invalid_argument(__FILE__, __LINE__, __func__, "'" + filename + "' file does not exist.");

    int j = 0;

    for (unsigned i = 0; i != best_channels_mother.size(); i++)
        if (best_channels_mother[i] < (unsigned)this->N) this->best_channels[j++] = best_channels_mother[i];

    std::cout << "Inside the RM frozenbit generation: ";
    for (int i = 0; i < this->best_channels.size(); i++)
    {
        std::cout << this->best_channels[i] << ",";
    }
    std::cout << std::endl;

    if (j != this->N) throw spu::tools::runtime_error(__FILE__, __LINE__, __func__, "Wrong size.");
}
