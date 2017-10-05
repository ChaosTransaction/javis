//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#include "file/file_util.h"
#include "file/file_path.h"
#include "future_file.h"

namespace future_logic {

FutureData::FutureData() {

}

FutureData::~FutureData() {

}

bool FutureData::ReadStaticFile(const std::string& sec,
                                const std::string& symbol, const int32 year,
                                const int32 month, const int32 day) {
  std::string dir = "/home/data/pb/future/" + sec + "/static/"
      + std::string(year) + "/" + std::string(month) + "/";

  std::string file_name = sec + "_" + symbol + "_" + std::string(year)
      + std::string(month) + std::string(day) + ".stc.chspb";

  return true;
}

bool FutureData::ReadFile(const std::string& sec, const std::string& data_type,
                           const std::string& shuffix,
                           const std::string& symbol, const int32 year,
                           const int32 month, const int32 day) {

  const std::string dir = "/home/data/pb/future/" + sec + data_type
      + std::string(year) + "/" + std::string(month) + "/";

  const std::string file_name = sec + "_" + symbol + "_" + std::string(year)
      + std::string(month) + std::string(day) + shuffix + ".chspb";

  ReadFile(dir, file_name);
  return true;
}

bool FutureData::ReadFile(const std::string& dir, const std::string& file,
                          std::string& content) {
  std::string full_dir_file = dir + "/" + file;
  file::FilePath file_path(full_dir_file);
  bool r = file::ReadFileToString(file_path, &content);
  if (!r)
    return false;
  return true;
}


}

