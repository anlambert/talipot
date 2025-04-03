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

#ifndef DIMENSION_BASE_H
#define DIMENSION_BASE_H

#include <vector>
#include <string>

struct DimensionBase {
    virtual ~DimensionBase() = default;
    virtual uint numberOfItems() const = 0;
    virtual uint numberOfValues() const = 0;
    virtual std::string getItemLabelAtRank(const uint rank) const = 0;
    virtual std::string getItemLabel(const uint itemId) const = 0;
    virtual double getItemValue(const uint itemId) const = 0;
    virtual double getItemValueAtRank(const uint rank) const = 0;
    virtual uint getItemIdAtRank(const uint rank) = 0;
    virtual uint getRankForItem(const uint itemId) = 0;
    virtual double minValue() const = 0;
    virtual double maxValue() const = 0;
    virtual std::vector<uint> links(const uint itemId) const = 0;
    virtual std::string getDimensionName() const = 0;
};

#endif // DIMENSION_BASE_H
