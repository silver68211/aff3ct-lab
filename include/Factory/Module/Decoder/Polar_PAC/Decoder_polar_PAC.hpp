/*!
 * \file
 * \brief Class factory::Decoder_polar_PAC.
 */
#ifndef FACTORY_DECODER_POLAR_PAC_HPP
#define FACTORY_DECODER_POLAR_PAC_HPP

#include <cli.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Factory/Module/Decoder/Decoder.hpp"
#include "Module/CRC/CRC.hpp"
#include "Module/Decoder/Decoder_SIHO.hpp"
#include "Module/Encoder/Encoder.hpp"
#include "Tools/Code/Polar/Polar_code.hpp"
#include "Tools/Factory/Header.hpp"

namespace aff3ct
{
namespace factory
{
extern const std::string Decoder_polar_PAC_name;
extern const std::string Decoder_polar_PAC_prefix;
class Decoder_polar_PAC : public Decoder
{
  public:
    // ----------------------------------------------------------------------------------------------------- PARAMETERS
    // optional parameters
    int L = 8;
    std::string node_type = "MS";

    // -------------------------------------------------------------------------------------------------------- METHODS
    explicit Decoder_polar_PAC(const std::string& p = Decoder_polar_PAC_prefix);
    virtual ~Decoder_polar_PAC() = default;
    Decoder_polar_PAC* clone() const;

    // parameters construction
    virtual void get_description(cli::Argument_map_info& args) const;
    virtual void store(const cli::Argument_map_value& vals);
    virtual void get_headers(std::map<std::string, tools::header_list>& headers, const bool full = true) const;

    // builder
    template<typename B = int, typename Q = float>
    module::Decoder_SIHO<B, Q>* build(const tools::Polar_code& code,
                                      const std::vector<bool>& frozen_bits,
                                      const module::CRC<B>* crc = nullptr,
                                      module::Encoder<B>* encoder = nullptr) const;

  protected:
    Decoder_polar_PAC(const std::string& n, const std::string& p);
};
}
}

#endif /* FACTORY_DECODER_POLAR_PAC_HPP */
