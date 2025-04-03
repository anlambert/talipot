/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_EXCEPTION_H
#define TALIPOT_EXCEPTION_H

#include <talipot/config.h>
#include <string>

namespace tlp {

//=======================================
/**
 * @class Exception
 * @brief Exception is a basic class to build exceptions from string
 **/
class TLP_SCOPE Exception : public std::exception {
  public:
    Exception(const std::string &desc);
    ~Exception() throw() override;
    const char *what() const throw() override;

  private:
    std::string desc;
};
}
#endif // TALIPOT_EXCEPTION_H
