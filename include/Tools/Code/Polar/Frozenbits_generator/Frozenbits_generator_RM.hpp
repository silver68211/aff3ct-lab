
/*!
 * \file
 * \brief Class tools::Frozenbits_generator_RM.
 */
#ifndef FROZENBITS_GENERATOR_RM_HPP_
#define FROZENBITS_GENERATOR_RM_HPP_

#include "Tools/Code/Polar/Frozenbits_generator/Frozenbits_generator_file.hpp"

namespace aff3ct
{
namespace tools
{
class Frozenbits_generator_RM : public Frozenbits_generator_file
{
  private:
    const int m;
    const int N_max = 65536;

  public:
    Frozenbits_generator_RM(const int K, const int N);

    ~Frozenbits_generator_RM();

    virtual Frozenbits_generator_RM* clone() const;

  private:
    void evaluate();
};
}
}

#endif /* FROZENBITS_GENERATOR_RM_HPP_ */
