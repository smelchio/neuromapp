/*
 * Neuromapp - tets.h, Copyright (c), 2015,
 * Francesco Casalegno - Swiss Federal Institute of technology in Lausanne,
 * francesco.casalegno@epfl.ch,
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
 * @file neuromapp/readi/readi.h
 * \brief Readi Miniapp
 */

#ifndef MAPP_READI_TETS_
#define MAPP_READI_TETS_

#include <fstream>
#include <string>
#include <cassert>

template<class IntType, class FloatType >
class Tets{
public:
    using idx_type = IntType;
    using real_type = FloatType;

    
    // access volume of i-th tetrahedron
    inline FloatType& volume(IntType i) {
        assert(i>=0 && i<n_tets_);
        return volumes_[i];
    }
    inline FloatType volume(IntType i) const {
        assert(i>=0 && i<n_tets_);
        return volumes_[i];
    }


    // access idx of j-th neighbor (j=0..3) of i-th tetrahedron
    inline IntType& neighbor(IntType i, IntType j) {
        assert(i>=0 && i<n_tets_);
        assert(j>=0 && j<=3);
        return neighbors_[4*i+j];
    }
    inline IntType neighbor(IntType i, IntType j) const {
        assert(i>=0 && i<n_tets_);
        assert(j>=0 && j<=3);
        return neighbors_[4*i+j];
    }

    
    // access shape of j-th neighbor (j=0..3) of i-th tetrahedron
    inline IntType& shape(IntType i, IntType j) {
        assert(i>=0 && i<n_tets_);
        assert(j>=0 && j<=3);
        return neighbors_[4*i+j];
    }
    inline IntType shape(IntType i, IntType j) const {
        assert(i>=0 && i<n_tets_);
        assert(j>=0 && j<=3);
        return neighbors_[4*i+j];
    }

    // access sum of neigh shapes of i-th tetrahedron
    inline IntType& shape_tot(IntType i) {
        assert(i>=0 && i<n_tets_);
        return shapes_sums_[i];
    }
    inline IntType shape_tot(IntType i) const {
        assert(i>=0 && i<n_tets_);
        return shapes_sums_[i];
    }

    
    // constructor, requires mesh and model filenames
    Tets(std::string filename_mesh, std::string filename_model) {
        std::ifstream file_mesh(filename_mesh);
        std::ifstream file_model(filename_model);
        
        try {
            std::string discard;

            file_mesh >> discard >> n_tets_;
            std::cout << "N tets: " <<  n_tets_ << std::endl; // how many tets?
            std::getline(file_mesh, discard);
            volumes_.resize(n_tets_);      // each tet has a volume
            neighbors_.resize(n_tets_*4);  // each tet has (up to) 4 neighbors
            shapes_.resize(n_tets_*4);     // each connect to neighb has a shape
            shapes_sums_.resize(n_tets_);       // each tet has tot sum of neighb shapes

            file_model >> discard >> n_species_;     // how many species?
            std::cout << "N species: " <<  n_species_ << std::endl;
            file_model.close();
            mol_counts_.resize(n_tets_*n_species_); // each tet knows how many mol of each species it contains
            mol_counts_bucket_.resize(n_tets_*n_species_); // bucket containing molecules received from diffusion 
            
            
            for (IntType i=0; i<n_tets; ++i) {
                file_mes >> discard >> volume(i);
                for (IntType j=0; j<4; ++j) 
                   file_mes >> neighbor(i, j);
                for (IntType j=0; j<4; ++j) {
                   double shape_times_vol;
                   file_mes >> shape_times_vol;
                   shape(i, j) = shape_times_vol / volume(i);
                }
                shape_tot(i) = 0;
                for (IntType j=0; j<4)
                    if (neighbor(i, j) != -1)
                        shape_tot(i) += shape(i, j)
            }
             


        }
        catch(const std::exception& ex) {
            file_mesh.close();
            file_model.close();
            throw;
        }

    }        



private:
    IntType n_tets_;
    IntType n_species_;
    std::vector<FloatType> volumes_;
    std::vector<IntType> neighbors_;
    std::vector<FloatType> shapes_;
    std::vector<FloatType> shapes_sums_;
    std::vector<FloatType> mol_counts_;
    std::vector<FloatType> mol_counts_bucket_; 

};



#endif// MAPP_READI_TETS_
