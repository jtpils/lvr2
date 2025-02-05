/**
 * Copyright (c) 2018, University Osnabrück
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University Osnabrück nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL University Osnabrück BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Random.hpp
 *
 *  @date 14.07.2017
 *  @author Johan M. von Behren <johan@vonbehren.eu>
 */

#ifndef LVR2_UTIL_RANDOM_H_
#define LVR2_UTIL_RANDOM_H_

#include  <random>
#include  <iterator>

namespace lvr2
{

/**
 * Select a random element between start and end
 *
 * @tparam Iter Type for the start and end iterator
 * @tparam RandomGenerator Type for the random generator
 *
 * @param start Start of the range to pick random element between
 * @param end End of the range to pick random element between
 * @param g random generator for selecting process
 *
 * @return iterator at random position between start and end
 */
template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g);

/**
 * Select a random element between start and end using the std::mt19937 random generator
 *
 * @tparam Iter Iter Type for the start and end iterator
 *
 * @param start Start of the range to pick random element between
 * @param end End of the range to pick random element between
 *
 * @return iterator at random position between start and end
 */
template<typename Iter>
Iter select_randomly(Iter start, Iter end);

} // namespace lvr2

#include <lvr2/util/Random.tcc>

#endif // LVR2_UTIL_RANDOM_H_
