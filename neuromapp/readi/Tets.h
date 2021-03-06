/*
 * Neuromapp - Tets.h, Copyright (c), 2015,
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
 * @file neuromapp/readi/Tets.h
 * \brief Mesh for  Readi Miniapp
 */

#ifndef MAPP_READI_TETS_
#define MAPP_READI_TETS_

#include <fstream>
#include <string>
#include <cassert>
#include <algorithm>
#include <numeric>

#include "rng_utils.h"


namespace readi {

template<class IntType, class FloatType >
class Tets{
public:
    using idx_type = IntType;
    using real_type = FloatType;



    // get number of tetrahedra
    inline IntType get_n_tets() const {
        return n_tets_;
    }

    
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
    inline FloatType& shape(IntType i, IntType j) {
        assert(i>=0 && i<n_tets_);
        assert(j>=0 && j<=3);
        return shapes_[4*i+j];
    }
    inline FloatType shape(IntType i, IntType j) const {
        assert(i>=0 && i<n_tets_);
        assert(j>=0 && j<=3);
        return shapes_[4*i+j];
    }


    // access sum of neigh shapes of i-th tetrahedron
    inline FloatType& shape_sum(IntType i) {
        assert(i>=0 && i<n_tets_);
        return shapes_sums_[i];
    }
    inline FloatType shape_sum(IntType i) const {
        assert(i>=0 && i<n_tets_);
        return shapes_sums_[i];
    }


    // access molecule for s-th species in i-th tetrahedron
    inline IntType& molecule_count(IntType s, IntType i) {
        assert(s>=0 && s<n_species_);
        assert(i>=0 && i<n_tets_);
        return mol_counts_[n_tets_*s + i];
    }
    inline IntType molecule_count(IntType s, IntType i) const {
        assert(s>=0 && s<n_species_);
        assert(i>=0 && i<n_tets_);
        return mol_counts_[n_tets_*s + i];
    }


    // access occupancy of s-th species in i-th tetrahedron
    inline FloatType& molecule_occupancy_count(IntType s, IntType i) {
        assert(s>=0 && s<n_species_);
        assert(i>=0 && i<n_tets_);
        return mol_occupancy_counts_[n_tets_*s + i];
    }
    inline FloatType molecule_occupancy_count(IntType s, IntType i) const {
        assert(s>=0 && s<n_species_);
        assert(i>=0 && i<n_tets_);
        return mol_occupancy_counts_[n_tets_*s + i];
    }


    // access last update time of s-th species in i-th tetrahedron
    inline FloatType& molecule_occupancy_last_update_time(IntType s, IntType i) {
        assert(s>=0 && s<n_species_);
        assert(i>=0 && i<n_tets_);
        return mol_occupancy_lastupdtime_[n_tets_*s + i];
    }
    inline FloatType molecule_occupancy_last_update_time(IntType s, IntType i) const {
        assert(s>=0 && s<n_species_);
        assert(i>=0 && i<n_tets_);
        return mol_occupancy_lastupdtime_[n_tets_*s + i];
    }


    // compute max shape d_K, so that tau = D_max * d_K
    FloatType get_max_shape() {
        return *std::max_element(shapes_sums_.begin(),shapes_sums_.end());
    }


    // compute total volume Omega
    FloatType get_tot_volume() {
        return std::accumulate(volumes_.begin(), volumes_.end(), 0.);
    }
    


    // read mesh + model and constructs internal objects
    template <class Generator>
    void read_from_file(std::string const& filename_mesh, std::string const& filename_model, Generator& g) {
                 
        std::ifstream file_mesh(filename_mesh);
        std::ifstream file_model(filename_model);
        
        try {

            std::string discard;

            // --- [MESH_FILE] READ VOLUME + NEIGHBORS + SHAPES ---
            file_mesh >> discard >> n_tets_;            // read
            std::getline(file_mesh, discard);           // skip '\n'
            std::getline(file_mesh, discard);           // skip headers
            volumes_.resize(n_tets_);                   // each tet has a volume
            neighbors_.resize(n_tets_*4);               // each tet has (up to) 4 neighbors
            shapes_.resize(n_tets_*4);                  // each connect to neighb has a shape
            shapes_sums_.resize(n_tets_);               // each tet has tot sum of neighb shapes

            for (IntType i=0; i<n_tets_; ++i) {
                file_mesh >> discard >> volume(i);      // read volume
                for (IntType j=0; j<4; ++j)
                   file_mesh >> neighbor(i, j);         // read idxes of neighbors
                for (IntType j=0; j<4; ++j) {
                   file_mesh >> shape(i, j);            // read shapes of neighbors
                   if (neighbor(i,j) == -1)
                       shape(i, j) = 0.;
                }
                shape_sum(i) = 0;
                for (IntType j=0; j<4; ++j)
                    shape_sum(i) += shape(i, j);
            }


            // --- [MODEL_FILE] READ N. SPECIES + N. INITIAL MOLECULES ---
            file_model >> discard >> n_species_;                // read n. of species
            mol_counts_.resize(n_tets_*n_species_);             // each tet knows how many mol of each species it contains
            mol_occupancy_counts_.resize(n_tets_*n_species_);
            mol_occupancy_lastupdtime_.resize(n_tets_*n_species_);
            mol_counts_bucket_.resize(n_tets_);                 // bucket containing molecules received from diffusion
            std::getline(file_model, discard);                           // read \n
            std::getline(file_model, discard);                           // read description line
            IntType tot_mol_per_spec;
            for (IntType i=0; i<n_species_; ++i) {
                file_model >> discard >> tot_mol_per_spec;                        // read species name and total count
                distribute_molecules(i, tot_mol_per_spec, g);      // distribute these molecules in the mesh
            }
            file_model.close();

            printf("----  TETS info ---------------------------------------------\n");
            printf("\t n. of tetrahedra :%10d\n", n_tets_);
            printf("\t n. of species    :%10d\n", n_species_);
            printf("-------------------------------------------------------------\n");

        }
        catch(const std::exception& ex) {
            file_mesh.close();
            file_model.close();
            throw;
        }

    }        


    // distribute tot number of molecules on each tetrahedron, used at initialization of mol counts
    template <class Generator>
    void distribute_molecules(IntType species_idx, IntType n_molecules_tot, Generator& g) {
        // TODO: actually the distribution of molecules should be done by implementing an adjusted Pareto sampler in two phases:
        //  1. attribute to each tet the relative rounded down fraction of molecules
        //  2. attribute the remaining molecules through Pareto sampling
        IntType n_molecules_partial = 0; // molecules that have been placed until now
        FloatType tot_volume = get_tot_volume();
        for (IntType i=0; i<n_tets_; ++i) {
            FloatType volume_ratio =  volume(i) / tot_volume;
            IntType mols =  readi::rand_round<IntType, FloatType>(n_molecules_tot * volume_ratio, g);
            n_molecules_partial += mols;
            molecule_count(species_idx, i) = mols;
        }
//        printf("---- Distribution of molecules for species %d ---------------\n", species_idx);
//        FloatType err_distr = 100*(double(n_molecules_tot-n_molecules_partial)/n_molecules_tot);
//        printf("\t Theoretical:%d,  Distributed:%d, Error:%5.2f%%\n", n_molecules_tot, n_molecules_partial, err_distr);
//        printf("-------------------------------------------------------------\n");

    }




    // add to buckets during diffusion
    inline void add_to_bucket(IntType tet_idx, IntType neighb_idx, IntType diffusing_count) {
        mol_counts_bucket_[neighbor(tet_idx, neighb_idx)] += diffusing_count;
    }

    // empty buckets after diffusion
    void empty_buckets(IntType s) {
        for (IntType i=0; i<n_tets_; ++i) {
            molecule_count(s, i) += mol_counts_bucket_[i];
            mol_counts_bucket_[i] = 0;
        }
        return;
    }


private:
    IntType n_tets_;
    IntType n_species_;
    std::vector<FloatType> volumes_;
    std::vector<IntType> neighbors_;
    // the shape(i,j) of tetrahedron i w.r.t to neighbor j=0..3 represent this geometrical value:
    //      shape = surface_separating(i,j) / (volume(i) * distance_of_barycenters(i,j))
    std::vector<FloatType> shapes_;
    std::vector<FloatType> shapes_sums_;
    std::vector<IntType> mol_counts_;
    std::vector<IntType> mol_counts_bucket_;
    std::vector<FloatType> mol_occupancy_counts_;
    std::vector<FloatType> mol_occupancy_lastupdtime_;
};

}

#endif// MAPP_READI_TETS_
