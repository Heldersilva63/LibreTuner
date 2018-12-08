/*
 * LibreTuner
 * Copyright (C) 2018 Altenius
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FLASHABLE_H
#define FLASHABLE_H

#include <memory>
#include <string>
#include <vector>


namespace definition {
struct Model;
using ModelPtr = std::shared_ptr<Model>;
}

class TuneData;

/**
 * A representation of data able to be flashed.
 */
class Flashable {
public:
    explicit Flashable(std::vector<uint8_t> data, definition::ModelPtr model);

    // The address offset the data should be flashed to
    size_t offset() const { return offset_; }

    const std::vector<uint8_t> &data() const { return data_; }

    definition::ModelPtr model() const { return model_; }

private:
    std::vector<uint8_t> data_;
    std::size_t offset_;
    definition::ModelPtr model_;
};

#endif // FLASHABLE_H
