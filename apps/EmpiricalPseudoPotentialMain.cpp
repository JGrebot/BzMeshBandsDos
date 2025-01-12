/**
 * @file EmpiricalPseudoPotentialMain.cpp
 * @author remzerrr (remi.helleboid@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-07-08
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <tclap/CmdLine.h>

#include <chrono>
#include <filesystem>
#include <thread>

#include "BandStructure.h"
#include "Options.h"

void print_arguments(const std::vector<std::string>& path,
                     const std::string&              material,
                     unsigned int                    nb_points,
                     unsigned int                    nb_bands,
                     unsigned int                    nearestNeighbors,
                     unsigned int                    nb_threads,
                     const std::string&              result_dir) {
    std::cout << "Path: ";
    for (auto& p : path)
        std::cout << p << " ";
    std::cout << std::endl;
    std::cout << "Material: " << material << std::endl;
    std::cout << "Number of points: " << nb_points << std::endl;
    std::cout << "Number of bands: " << nb_bands << std::endl;
    std::cout << "Number of threads: " << nb_threads << std::endl;
    std::cout << "Number of nearest neighbors: " << nearestNeighbors << std::endl;
    std::cout << "Result directory: " << result_dir << std::endl;
}

int compute_path_mat(const EmpiricalPseudopotential::Material& material,
                     const std::vector<std::string>&           path,
                     unsigned int                              nb_points,
                     unsigned int                              nb_bands,
                     unsigned int                              nearestNeighbors,
                     const std::string&                        result_dir) {
    Options my_options;
    my_options.nearestNeighbors = nearestNeighbors;
    my_options.nrPoints         = nb_points;
    my_options.nrLevels         = nb_bands;
    EmpiricalPseudopotential::BandStructure my_bandstructure;
    my_bandstructure.Initialize(material, my_options.nrLevels, path, my_options.nrPoints, my_options.nearestNeighbors);
    my_bandstructure.Compute();
    my_bandstructure.AdjustValues();
    my_bandstructure.export_result_in_file(result_dir + "/" + my_bandstructure.path_band_filename() + ".txt");
    return 0;
}

int compute_all_path_all_mat(int nb_bands, int nearestNeighbors, int nrPoints, int nb_threads, const std::string& result_dir) {
    EmpiricalPseudopotential::Materials materials;
    Options                             my_options;
    my_options.nearestNeighbors = nearestNeighbors;
    my_options.nrPoints         = nrPoints;
    my_options.nrThreads        = nb_threads;
    my_options.nrLevels         = nb_bands;
    for (auto const& [name, mat] : materials.materials) {
        std::cout << "----------------------------------------------------" << std::endl;
        std::cout << "Material: " << name << std::endl;
        for (int path_index = 0; path_index < my_options.paths.size(); path_index++) {
            std::vector<std::string> path = my_options.paths[path_index];
            std::cout << "path: ";
            for (auto& point : path) {
                std::cout << point << " ";
            }
            std::cout << std::endl;
            EmpiricalPseudopotential::BandStructure my_bandstructure;
            my_bandstructure.Initialize(mat,
                                        my_options.nrLevels,
                                        my_options.paths[path_index],
                                        my_options.nrPoints,
                                        my_options.nearestNeighbors);

            auto res = my_bandstructure.Compute_parralel(my_options.nrThreads);
            my_bandstructure.AdjustValues();
            my_bandstructure.export_result_in_file(result_dir + "/" + my_bandstructure.path_band_filename() + ".txt");
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    TCLAP::CmdLine               cmd("EPP PROGRAM. COMPUTE BAND STRUCTURE ON A BZ MESH.", ' ', "1.0");
    TCLAP::ValueArg<std::string> arg_path_sym_points("p",
                                                     "path",
                                                     "path of high symmetry points, e.g. LKWGXWLGK ",
                                                     false,
                                                     "LGXUG",
                                                     "string");
    TCLAP::ValueArg<std::string> arg_material("m", "material", "Symbol of the material to use (Si, Ge, GaAs, ...)", false, "Si", "string");
    TCLAP::ValueArg<int>         arg_nb_points("N", "npoints", "Number of points per Path", false, 80, "int");
    TCLAP::ValueArg<int>         arg_nb_bands("b", "nbands", "Number of bands to compute", false, 12, "int");
    TCLAP::ValueArg<int>         arg_nearest_neighbors("n",
                                               "nearestNeighbors",
                                               "number of nearest neiborgs to consider for the EPP calculation.",
                                               false,
                                               10,
                                               "int");
    TCLAP::ValueArg<int>         arg_nb_threads("j", "nthreads", "number of threads to use.", false, 1, "int");
    TCLAP::ValueArg<std::string> arg_res_dir("r", "resultdir", "directory to store the results.", false, "EPP_RESULTS", "str");
    TCLAP::SwitchArg             all_path_mat("A", "all", "Compute the band structure on all the paths for all the materials", false);
    cmd.add(arg_path_sym_points);
    cmd.add(arg_material);
    cmd.add(arg_nb_bands);
    cmd.add(arg_nb_points);
    cmd.add(arg_nearest_neighbors);
    cmd.add(arg_nb_threads);
    cmd.add(arg_res_dir);
    cmd.add(all_path_mat);

    cmd.parse(argc, argv);

    // Create the result directory if it doesn't exist.
    std::filesystem::create_directories(arg_res_dir.getValue());

    // Transform the string of the path into a vector of string.
    std::vector<std::string> path_list;
    if (arg_path_sym_points.isSet()) {
        path_list.resize(arg_path_sym_points.getValue().size());
        for (int i = 0; i < arg_path_sym_points.getValue().size(); i++) {
            path_list[i] = arg_path_sym_points.getValue()[i];
            std::cout << path_list[i] << std::endl;
        }
    }

    EmpiricalPseudopotential::Materials materials;
    materials.materials.at(arg_material.getValue());

    print_arguments(path_list,
                    arg_material.getValue(),
                    arg_nb_points.getValue(),
                    arg_nb_bands.getValue(),
                    arg_nearest_neighbors.getValue(),
                    arg_nb_threads.getValue(),
                    arg_res_dir.getValue());
    std::this_thread::sleep_for(std::chrono::seconds(4));

    if (all_path_mat.getValue()) {
        std::cout << "Compute the band structure on all the paths for all the materials" << std::endl;
        return compute_all_path_all_mat(arg_nb_bands.getValue(),
                                        arg_nearest_neighbors.getValue(),
                                        arg_nb_points.getValue(),
                                        arg_nb_threads.getValue(),
                                        arg_res_dir.getValue());
    } else if (arg_material.isSet() && arg_path_sym_points.isSet()) {
        std::cout << "Compute the band structure on the path " << arg_path_sym_points.getValue() << " for the material "
                  << arg_material.getValue() << std::endl;
        return compute_path_mat(materials.materials.at(arg_material.getValue()),
                                path_list,
                                arg_nb_points.getValue(),
                                arg_nb_bands.getValue(),
                                arg_nearest_neighbors.getValue(),
                                arg_res_dir.getValue());
    } else {
        std::cout << "No material or path specified" << std::endl;
        return 1;
    }

    return 0;
}