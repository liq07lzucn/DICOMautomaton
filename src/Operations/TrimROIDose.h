// TrimROIDose.h.

#pragma once

#include <string>
#include <map>

#include "../Structs.h"


OperationDoc OpArgDocTrimROIDose(void);

Drover
TrimROIDose(Drover DICOM_data, 
                              OperationArgPkg /*OptArgs*/,
                              std::map<std::string, std::string> /*InvocationMetadata*/,
                              std::string /*FilenameLex*/);
