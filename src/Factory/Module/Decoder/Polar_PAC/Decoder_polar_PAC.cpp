#include <algorithm>
#include <memory>
#include <streampu.hpp>
#include <utility>

#include "Module/Decoder/Polar_PAC/SCL/Decoder_polar_PAC_SCL_naive.hpp"
#include "Tools/Documentation/documentation.h"
// #define API_POLAR_DYNAMIC 1
#include "Tools/Code/Polar/API/API_polar_dynamic_intra.hpp"
#include "Tools/Code/Polar/API/API_polar_dynamic_seq.hpp"
#ifdef API_POLAR_DYNAMIC
#include "Tools/Code/Polar/API/API_polar_dynamic_inter.hpp"
#include "Tools/Code/Polar/API/API_polar_dynamic_inter_8bit_bitpacking.hpp"
#else
#include "Tools/Code/Polar/API/API_polar_static_inter.hpp"
#include "Tools/Code/Polar/API/API_polar_static_inter_8bit_bitpacking.hpp"
#include "Tools/Code/Polar/API/API_polar_static_intra_16bit.hpp"
#include "Tools/Code/Polar/API/API_polar_static_intra_32bit.hpp"
#include "Tools/Code/Polar/API/API_polar_static_intra_8bit.hpp"
#include "Tools/Code/Polar/API/API_polar_static_seq.hpp"
#endif
#include "Factory/Module/Decoder/Polar_PAC/Decoder_polar_PAC.hpp"
#include "Tools/Code/Polar/Nodes_parser.hpp"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Decoder_polar_PAC_name = "Decoder Polar PAC";
const std::string aff3ct::factory::Decoder_polar_PAC_prefix = "dec";

Decoder_polar_PAC ::Decoder_polar_PAC(const std::string& prefix)
  : Decoder(Decoder_polar_PAC_name, prefix)
{
    this->type = "SCL";
    this->implem = "NAIVE";
}

Decoder_polar_PAC ::Decoder_polar_PAC(const std::string& name, const std::string& prefix)
  : Decoder(name, prefix)
{
    this->type = "SCL";
    this->implem = "NAIVE";
}

Decoder_polar_PAC*
Decoder_polar_PAC ::clone() const
{
    return new Decoder_polar_PAC(*this);
}

void
Decoder_polar_PAC ::get_description(cli::Argument_map_info& args) const
{
    Decoder::get_description(args);

    auto p = this->get_prefix();
    const std::string class_name = "factory::Decoder_polar_PAC::";

    cli::add_options(args.at({ p + "-type", "D" }), 0, "SCL");

    args.at({ p + "-implem" })->change_type(cli::Text(cli::Example_set("NAIVE")));

    tools::add_arg(args, p, class_name + "p+ite,i", cli::Integer(cli::Positive(), cli::Non_zero()));

    tools::add_arg(args, p, class_name + "p+lists,L", cli::Integer(cli::Positive(), cli::Non_zero()));

    tools::add_arg(args, p, class_name + "p+simd", cli::Text(cli::Including_set("INTRA", "INTER")));

    tools::add_arg(args, p, class_name + "p+polar-nodes", cli::Text());

    tools::add_arg(args, p, class_name + "p+partial-adaptive", cli::None());

    tools::add_arg(args, p, class_name + "p+no-sys", cli::None());
}

void
Decoder_polar_PAC ::store(const cli::Argument_map_value& vals)
{
    Decoder::store(vals);

    auto p = this->get_prefix();

    if (vals.exist({ p + "-ite", "i" })) this->n_ite = vals.to_int({ p + "-ite", "i" });
    if (vals.exist({ p + "-lists", "L" })) this->L = vals.to_int({ p + "-lists", "L" });
    if (vals.exist({ p + "-simd" })) this->simd_strategy = vals.at({ p + "-simd" });
    if (vals.exist({ p + "-polar-nodes" })) this->polar_nodes = vals.at({ p + "-polar-nodes" });
    if (vals.exist({ p + "-partial-adaptive" })) this->full_adaptive = false;

    // force 1 iteration max if not SCAN (and polar code)
    if (this->type != "SCAN") this->n_ite = 1;
}

void
Decoder_polar_PAC ::get_headers(std::map<std::string, tools::header_list>& headers, const bool full) const
{
    Decoder::get_headers(headers, full);

    if (this->type != "ML" && this->type != "CHASE")
    {
        auto p = this->get_prefix();

        if (!this->simd_strategy.empty()) headers[p].push_back(std::make_pair("SIMD strategy", this->simd_strategy));

        if (this->type == "SCAN")
            headers[p].push_back(std::make_pair("Num. of iterations (i)", std::to_string(this->n_ite)));

        if (this->type == "SCF") headers[p].push_back(std::make_pair("Num. of flips", std::to_string(this->flips)));

        if (this->type == "SCL" || this->type == "SCL_MEM")
            headers[p].push_back(std::make_pair("Num. of lists (L)", std::to_string(this->L)));

        if (this->type == "ASCL" || this->type == "ASCL_MEM")
        {
            auto adaptative_mode = this->full_adaptive ? "full" : "partial";
            headers[p].push_back(std::make_pair("Max num. of lists (L)", std::to_string(this->L)));
            headers[p].push_back(std::make_pair("Adaptative mode", adaptative_mode));
        }

        if ((this->type == "SC" || this->type == "SCL" || this->type == "ASCL" || this->type == "SCL_MEM" ||
             this->type == "ASCL_MEM") &&
            this->implem == "FAST")
            headers[p].push_back(std::make_pair("Polar node types", this->polar_nodes));
    }
}

template<typename B, typename Q, class API_polar>
module::Decoder_SIHO<B, Q>*
Decoder_polar_PAC ::_build(const std::vector<bool>& frozen_bits,
                           const module::CRC<B>* crc,
                           module::Encoder<B>*, /*encoder*/
                           const std::vector<uint8_t>& conv) const
{

    return new module::Decoder_polar_PAC_SCL_naive<B, Q, tools::f_LLR<Q>, tools::g_LLR<B, Q>>(
      this->K, this->N_cw, this->L, frozen_bits, conv);

    throw spu::tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

template<typename B, typename Q>
module::Decoder_SIHO<B, Q>*
Decoder_polar_PAC ::build(const std::vector<bool>& frozen_bits,
                          const module::CRC<B>* crc,
                          module::Encoder<B>* encoder,
                          const std::vector<uint8_t>& conv) const
{
    try
    {
        return Decoder::build<B, Q>(encoder);
    }
    catch (spu::tools::cannot_allocate const&)
    {

        if (this->simd_strategy == "INTER" && this->type == "SC" && this->implem == "FAST")
        {
            if (typeid(B) == typeid(signed char))
            {
#ifdef AFF3CT_POLAR_BIT_PACKING
#ifdef API_POLAR_DYNAMIC
                using API_polar = tools::API_polar_dynamic_inter_8bit_bitpacking<B, Q>;
#else
                using API_polar = tools::API_polar_static_inter_8bit_bitpacking<B, Q>;
#endif
#else
#ifdef API_POLAR_DYNAMIC
                using API_polar = tools::API_polar_dynamic_inter<B, Q>;
#else
                using API_polar = tools::API_polar_static_inter<B, Q>;
#endif
#endif
                return _build<B, Q, API_polar>(frozen_bits, crc, encoder, conv);
            }
            else
            {
#ifdef API_POLAR_DYNAMIC
                using API_polar = tools::API_polar_dynamic_inter<B, Q>;
#else
                using API_polar = tools::API_polar_static_inter<B, Q>;
#endif
                return _build<B, Q, API_polar>(frozen_bits, crc, encoder, conv);
            }
        }
        else if (this->simd_strategy == "INTRA" && this->implem == "FAST")
        {
            if (typeid(B) == typeid(signed char))
            {
#ifdef API_POLAR_DYNAMIC
                using API_polar = tools::API_polar_dynamic_intra<B, Q>;
#else
                using API_polar = tools::API_polar_static_intra_8bit<B, Q>;
#endif
                return _build<B, Q, API_polar>(frozen_bits, crc, encoder, conv);
            }
            else if (typeid(B) == typeid(short))
            {
#ifdef API_POLAR_DYNAMIC
                using API_polar = tools::API_polar_dynamic_intra<B, Q>;
#else
                using API_polar = tools::API_polar_static_intra_16bit<B, Q>;
#endif
                return _build<B, Q, API_polar>(frozen_bits, crc, encoder, conv);
            }
            else if (typeid(B) == typeid(int))
            {
#ifdef API_POLAR_DYNAMIC
                using API_polar = tools::API_polar_dynamic_intra<B, Q>;
#else
                using API_polar = tools::API_polar_static_intra_32bit<B, Q>;
#endif
                return _build<B, Q, API_polar>(frozen_bits, crc, encoder, conv);
            }
        }
        else if (this->simd_strategy.empty())
        {
#ifdef API_POLAR_DYNAMIC
            using API_polar = tools::API_polar_dynamic_seq<B, Q>;
#else
            using API_polar = tools::API_polar_static_seq<B, Q>;
#endif
            return _build<B, Q, API_polar>(frozen_bits, crc, encoder, conv);
        }
    }

    throw spu::tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
/*#ifdef AFF3CT_MULTI_PREC*/
/*template aff3ct::module::Decoder_SISO<B_8, Q_8>**/
/*aff3ct::factory::Decoder_polar_PAC::build_siso<B_8, Q_8>(const std::vector<bool>&, module::Encoder<B_8>*) const;*/
/*template aff3ct::module::Decoder_SISO<B_16, Q_16>**/
/*aff3ct::factory::Decoder_polar_PAC::build_siso<B_16, Q_16>(const std::vector<bool>&, module::Encoder<B_16>*) const;*/
/*template aff3ct::module::Decoder_SISO<B_32, Q_32>**/
/*aff3ct::factory::Decoder_polar_PAC::build_siso<B_32, Q_32>(const std::vector<bool>&, module::Encoder<B_32>*) const;*/
/*template aff3ct::module::Decoder_SISO<B_64, Q_64>**/
/*aff3ct::factory::Decoder_polar_PAC::build_siso<B_64, Q_64>(const std::vector<bool>&, module::Encoder<B_64>*) const;*/
/*#else*/
/*template aff3ct::module::Decoder_SISO<B, Q>**/
/*aff3ct::factory::Decoder_polar_PAC::build_siso<B, Q>(const std::vector<bool>&, module::Encoder<B>*) const;*/
/*#endif*/

#ifdef AFF3CT_MULTI_PREC
template aff3ct::module::Decoder_SIHO<B_8, Q_8>*
aff3ct::factory::Decoder_polar_PAC::build<B_8, Q_8>(const std::vector<bool>&,
                                                    const module::CRC<B_8>*,
                                                    module::Encoder<B_8>*,
                                                    const std::vector<uint8_t>& conv) const;
template aff3ct::module::Decoder_SIHO<B_16, Q_16>*
aff3ct::factory::Decoder_polar_PAC::build<B_16, Q_16>(const std::vector<bool>&,
                                                      const module::CRC<B_16>*,
                                                      module::Encoder<B_16>*,
                                                      const std::vector<uint8_t>& conv) const;
template aff3ct::module::Decoder_SIHO<B_32, Q_32>*
aff3ct::factory::Decoder_polar_PAC::build<B_32, Q_32>(const std::vector<bool>&,
                                                      const module::CRC<B_32>*,
                                                      module::Encoder<B_32>*,
                                                      const std::vector<uint8_t>& conv) const;
template aff3ct::module::Decoder_SIHO<B_64, Q_64>*
aff3ct::factory::Decoder_polar_PAC::build<B_64, Q_64>(const std::vector<bool>&,
                                                      const module::CRC<B_64>*,
                                                      module::Encoder<B_64>*,
                                                      const std::vector<uint8_t>& conv) const;
#else
template aff3ct::module::Decoder_SIHO<B, Q>*
aff3ct::factory::Decoder_polar_PAC::build<B, Q>(const std::vector<bool>&,
                                                const module::CRC<B>*,
                                                module::Encoder<B>*,
                                                const std::vector<uint8_t>& conv) const;
#endif
// ==================================================================================== explicit template instantiation
