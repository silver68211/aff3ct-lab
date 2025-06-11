#include <streampu.hpp>

#include "Factory/Module/Encoder/Polar_PAC/Encoder_polar_PAC.hpp"
#include "Module/Encoder/Polar_PAC/Encoder_polar_PAC.hpp"
//#include "Module/Encoder/Polar_MK/Encoder_polar_MK_sys.hpp"
#include "Tools/Documentation/documentation.h"

using namespace aff3ct;
using namespace aff3ct::factory;

const std::string aff3ct::factory::Encoder_polar_PAC_name = "Encoder Polar PAC";
const std::string aff3ct::factory::Encoder_polar_PAC_prefix = "enc";

Encoder_polar_PAC ::Encoder_polar_PAC(const std::string& prefix)
  : Encoder(Encoder_polar_PAC_name, prefix)
{
    this->type = "POLAR_PAC";
    this->systematic = false;
}

Encoder_polar_PAC*
Encoder_polar_PAC ::clone() const
{
    return new Encoder_polar_PAC(*this);
}

struct sub_same_length
{
    template<typename T>
    static void check(const std::vector<T>& v)
    {
        for (unsigned i = 0; i < v.size(); i++)
            if (v[i].size() != v[0].size()) throw std::runtime_error("all elements do not have the same length");
    }
};

void
Encoder_polar_PAC ::get_description(cli::Argument_map_info& args) const
{
    Encoder::get_description(args);

    auto p = this->get_prefix();
    const std::string class_name = "factory::Encoder_polar_PAC::";

    cli::add_options(args.at({ p + "-type" }), 0, "POLAR_PAC");

    tools::add_arg(args, p, class_name + "p+sys", cli::None());
}

void
Encoder_polar_PAC ::store(const cli::Argument_map_value& vals)
{
    Encoder::store(vals);

    auto p = this->get_prefix();

    if (vals.exist({ p + "-sys" })) this->systematic = true;
}

void
Encoder_polar_PAC ::get_headers(std::map<std::string, tools::header_list>& headers, const bool full) const
{
    Encoder::get_headers(headers, full);

    // auto p = this->get_prefix();
}

template<typename B>
module::Encoder_polar_PAC<B>*
Encoder_polar_PAC ::build(const tools::Polar_code& code, const std::vector<bool>& frozen_bits) const
{
    if (this->type == "POLAR_PAC")
    {
        /*if (this->systematic)*/
        /*    return new module::Encoder_polar_MK_sys<B>(this->K, this->N_cw, code, frozen_bits);*/
        /*else*/
        return new module::Encoder_polar_PAC<B>(this->K, this->N_cw, code, frozen_bits);
    }

    throw spu::tools::cannot_allocate(__FILE__, __LINE__, __func__);
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef AFF3CT_MULTI_PREC
template aff3ct::module::Encoder_polar_PAC<B_8>*
aff3ct::factory::Encoder_polar_PAC::build<B_8>(const tools::Polar_code&, const std::vector<bool>&) const;
template aff3ct::module::Encoder_polar_PAC<B_16>*
aff3ct::factory::Encoder_polar_PAC::build<B_16>(const tools::Polar_code&, const std::vector<bool>&) const;
template aff3ct::module::Encoder_polar_PAC<B_32>*
aff3ct::factory::Encoder_polar_PAC::build<B_32>(const tools::Polar_code&, const std::vector<bool>&) const;
template aff3ct::module::Encoder_polar_PAC<B_64>*
aff3ct::factory::Encoder_polar_PAC::build<B_64>(const tools::Polar_code&, const std::vector<bool>&) const;
#else
template aff3ct::module::Encoder_polar_PAC<B>*
aff3ct::factory::Encoder_polar_PAC::build<B>(const tools::Polar_code&, const std::vector<bool>&) const;
#endif
// ==================================================================================== explicit template instantiation
