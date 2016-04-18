/*
 * Neuromapp - presyn_maker.h, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/coreneuron_1.0/environment/presyn_maker.h
 * \brief Contains presyn_maker class declaration.
 */

#ifndef MAPP_PRESYN_MAKER_H
#define MAPP_PRESYN_MAKER_H

#include <map>

#include "coreneuron_1.0/event_passing/environment/generator.h"

namespace environment {

typedef std::pair<int, std::vector<int> > input_presyn;

/** presyn_maker
 * creates input and output presyns required for spike exchange
 */
class presyn_maker {
private:
    int n_out_;
    int n_in_;
    int nets_per_;
    std::vector<int> outputs_;
    std::map<int, std::vector<int> > inputs_;
public:
    /** \fn presyn_maker(int out, int in, int netconsper)
     *  \brief creates the presyn_maker and sets member variables
     *  \param out number of output presyns
     *  \param in number of input presyns
     *  \param netconsper number of netcons per input presyn
     */
    explicit presyn_maker(int out=0, int in=0, int netcons_per=0):
    n_out_(out), n_in_(in), nets_per_(netcons_per) {}

    /** \fn void operator()(int nprocs, int ngroups, int rank)
     *  \brief generates both the input and output presyns.
     *  \param nprocs the number of processes in the simulation
     *  \param ngroups the number of cell groups per process
     *  \param rank the rank of the current process
     */
    void operator()(int nprocs, int ngroups, int rank);

//GETTERS
    /** \fn int operator[](int id)
     *  \param index the index used to retrieve the output presyn
     *  \return the value stored in output_presyns_ at index id
     */
    int operator[](int index) const;

    /** \fn int get_nout()
     *  \return the number of output presyns
     */
    int get_nout() const { return n_out_; }

    /** \fn int get_nin()
     *  \return the number of input presyns
     */
    int get_nin() const { return n_in_; }

    /** \fn find_input(int id, input_presyn& presyn)
     *  \brief searches for an input presyn(IP) matching the parameter key. If
     *  IP is found, the param presyn is modified to reference the desired IP.
     *  \param key integer key used to find the input presyn
     *  \param presyn used to return the matching input_presyn val by reference
     *  only valid if find_input returns true.
     *  \return true if matching presyn is found, else false
     */
    bool find_input(int key, input_presyn& presyn) const;
};

} //end of namespace

#endif