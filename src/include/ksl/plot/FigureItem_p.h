/*
 * Copyright (C) 2017  Elvis Teixeira
 *
 * This source code is free software: you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General
 * Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any
 * later version.
 *
 * This source code is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <ksl/plot/FigureItem.h>

namespace ksl {
namespace plot {

class FigureItemPrivate: public ksl::ObjectPrivate
{
public:

    FigureItemPrivate(FigureItem *publ)
        : ksl::ObjectPrivate(publ)
        , visible(true)
        , figure(nullptr)
        , scale(nullptr)
    {}

    bool visible;
    Figure *figure;
    FigureScale *scale;
    QString title;
};
}}
