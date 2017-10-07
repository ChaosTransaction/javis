//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#ifndef FUTURE_FUTURE_FILE_H_
#define FUTURE_FUTURE_FILE_H_

#include "basic/basictypes.h"
#include <string>
namespace future_logic {
class FutureFile {
  friend class FutureManager;
  friend class FutureEngine;
 protected:
  FutureFile();
  virtual ~FutureFile();
 protected:
  static bool ReadStaticFile(const std::string& sec, const std::string& symbol,
                             const int32 year, const int32 month,
                             const int32 day);

  static bool ReadFile(const std::string& sec, const std::string& data_type,
                const std::string& shuffix, const std::string& symbol,
                const int32 year, const int32 month, const int32 day,
                std::string* content);
 private:

  static bool ReadFile(const std::string& dir, const std::string& file,
                       std::string* content);
};

}

#endif /* PLUGINS_FUTURE_FUTURE_DATA_H_ */
