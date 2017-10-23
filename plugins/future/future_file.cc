//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#include "file/file_util.h"
#include "file/file_path.h"
#include "basic/basic_util.h"
#include "future_file.h"

namespace future_logic {

FutureFile::FutureFile() {

}

FutureFile::~FutureFile() {

}

bool FutureFile::ReadStaticFile(const std::string& sec,
                                const std::string& symbol,
                                const std::string& stk_type, const int32 year,
                                const int32 month, const int32 day,
                                std::string* content) {
  std::string dir = "/home/stm/" + stk_type + "/" + sec + "/" + symbol
      + "/STATIC/" + base::BasicUtil::StringUtil::Int64ToString(year) + "/"
      + base::BasicUtil::StringUtil::Int64ToString(month) + "/";

  std::string file_name = sec + "_" + symbol + "_"
      + base::BasicUtil::StringUtil::Int64ToString(year)
      + base::BasicUtil::StringUtil::Int64ToString(month)
      + base::BasicUtil::StringUtil::Int64ToString(day) + ".stc.chspb";

  return ReadFile(dir, file_name, content);
}

bool FutureFile::ReadFile(const std::string& sec, const std::string& data_type,
                          const std::string& stk_type,
                          const std::string& shuffix, const std::string& symbol,
                          const int32 year, const int32 month, const int32 day,
                          std::string* content) {

  ///home/d/ftr_idx/CZCE/SR0000/STATIC/2010/2
  const std::string dir = "/home/stm/" + stk_type + "/" + sec + "/" + symbol + "/"
      + data_type + "/" + base::BasicUtil::StringUtil::Int64ToString(year) + "/"
      + base::BasicUtil::StringUtil::Int64ToString(month) + "/";

  const std::string file_name = sec + "_" + symbol + "_"
      + base::BasicUtil::StringUtil::Int64ToString(year)
      + base::BasicUtil::StringUtil::Int64ToString(month)
      + base::BasicUtil::StringUtil::Int64ToString(day) + shuffix + ".chspb";

  return ReadFile(dir, file_name, content);
}

bool FutureFile::ReadFile(const std::string& dir, const std::string& file,
                          std::string* content) {
  std::string full_dir_file = dir + "/" + file;
  file::FilePath file_path(full_dir_file);
  bool r = file::ReadFileToString(file_path, content);
  if (!r)
    return false;
  return true;
}

}

