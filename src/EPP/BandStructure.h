#pragma once

#include <atomic>
#include <vector>

#include "Material.h"
#include "SymmetryPoints.h"
#include "Vector3D.h"

namespace EmpiricalPseudopotential {

class BandStructure {
 public:
    BandStructure() : m_nb_points(0), m_nb_bands(10), m_nearestNeighborsNumber(10){};

    /**
     * @brief Initialize the band structure with the given material and the given symmetry points that form a path
     * of nrPoints on which the energies will be computed, for nb_bands levels.
     *
     * @param material
     * @param nb_bands
     * @param path
     * @param nrPoints
     * @param nearestNeighborsNumber
     */
    void Initialize(const Material&                 material,
                    std::size_t                     nb_bands,
                    const std::vector<std::string>& path,
                    unsigned int                    nrPoints,
                    unsigned int                    nearestNeighborsNumber);

    /**
     * @brief Initialize the band structure with the given material and the k-points on which we want
     * the energies to be computed.
     *
     * @param material
     * @param nb_bands
     * @param list_k_points
     * @param nearestNeighborsNumber
     */
    void Initialize(const Material&               material,
                    std::size_t                   nb_bands,
                    std::vector<Vector3D<double>> list_k_points,
                    unsigned int                  nearestNeighborsNumber);

    const std::vector<std::string>&  GetPath() const { return m_path; }
    unsigned int                     GetPointsNumber() const { return static_cast<unsigned int>(m_kpoints.size()); }
    std::vector<std::vector<double>> Compute();
    std::vector<std::vector<double>> Compute_parralel(int nb_threads);
    double                           AdjustValues();

    void        print_results() const;
    std::string path_band_filename() const;
    void        export_kpoints_to_file(std::string filename) const;
    void        export_result_in_file(const std::string& filename) const;
    void        export_result_in_file_with_kpoints(const std::string& filename) const;

    unsigned int        get_number_of_bands() const { return m_nb_bands; }
    std::vector<double> get_band(unsigned int band_index) const;

 private:
    Materials materials;

    SymmetryPoints            symmetryPoints;
    std::vector<unsigned int> symmetryPointsPositions;
    std::vector<std::string>  m_path;
    unsigned int              m_nb_points;

    Material     m_material;
    unsigned int m_nb_bands;
    unsigned int m_nearestNeighborsNumber;

    std::vector<Vector3D<int>>       basisVectors;
    std::vector<Vector3D<double>>    m_kpoints;
    std::vector<std::vector<double>> m_results;

    static bool FindBandgap(const std::vector<std::vector<double>>& results, double& maxValValence, double& minValConduction);
    bool        GenerateBasisVectors(unsigned int nearestNeighborsNumber);
};

void export_vector_bands_result_in_file(const std::string& filename, std::vector<std::vector<double>>);

}  // namespace EmpiricalPseudopotential
