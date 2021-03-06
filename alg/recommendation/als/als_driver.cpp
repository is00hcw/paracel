/**
 * Copyright (c) 2014, Douban Inc. 
 *   All rights reserved. 
 *
 * Distributed under the BSD License. Check out the LICENSE file for full text.
 *
 * Paracel - A distributed optimization framework with parameter server.
 *
 * Downloading
 *   git clone https://github.com/douban/paracel.git
 *
 * Authors: Hong Wu <xunzhangthu@gmail.com>
 *
 */

#include <string>
#include <iostream>

#include <mpi.h>
#include <google/gflags.h>

#include "als.hpp"
#include "utils.hpp"

DEFINE_string(server_info,
              "host1:7777PARACELhost2:8888",
              "hosts name string of paracel-servers.\n");

DEFINE_string(cfg_file,
              "",
              "config json file with absolute path.\n");

int main(int argc, char *argv[])
{
  paracel::main_env comm_main_env(argc, argv);
  paracel::Comm comm(MPI_COMM_WORLD);

  google::SetUsageMessage("[options]\n\t--server_info\n\t--cfg_file\n");
  google::ParseCommandLineFlags(&argc, &argv, true);
  
  paracel::json_parser pt(FLAGS_cfg_file);
  std::string rating_input = pt.check_parse<std::string>("rating_input");
  std::string factor_input = pt.check_parse<std::string>("factor_input");
  std::string validate_input = pt.check_parse<std::string>("validate_input");
  std::string output = pt.parse<std::string>("output");
  std::string pattern = pt.parse<std::string>("pattern");
  double lambda = pt.parse<double>("lambda");
  
  paracel::alg::alternating_least_square_standard H_solver(comm, FLAGS_server_info,
                                                           rating_input, factor_input, validate_input,
                                                           output,
                                                           pattern, lambda);
  H_solver.solve();
  H_solver.dump_result();
  double train_err = H_solver.cal_rmse();
  if(comm.get_rank() == 0) std::cout << "train error: " << train_err << std::endl;
  double test_err = H_solver.validate();
  if(comm.get_rank() == 0) std::cout << "test error:" << test_err << std::endl;
  return 0;
}
