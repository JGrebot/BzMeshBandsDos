/**
 * @file bz_mesh.hpp
 * @author remzerrr (remi.helleboid@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-07-07
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <iostream>
#include <string>

#include "Vector3D.h"
#include "gmsh.h"
#include "BandStructure.h"



class bz_mesh_points {
 private:
    std::string                   m_filename;
    std::size_t                   m_nb_points;
    std::vector<std::size_t>      m_node_tags;
    std::vector<Vector3D<double>> m_kpoints;

 public:
    explicit bz_mesh_points(const std::string &filename) : m_filename(filename), m_nb_points(0){};
    ~bz_mesh_points() = default;

    void add_k_point(Vector3D<double> kpoint);
    void add_k_point(double k_x, double k_y, double k_z);

    void                           read_mesh();
    std::vector<Vector3D<double>>& get_kpoints() { return m_kpoints; };

    void add_band_on_mesh(const std::string& band_name, const std::vector<double>& band_values);
    void add_all_bands_on_mesh(const std::string& out_filename, const EmpiricalPseudopotential::BandStructure &my_band);
};