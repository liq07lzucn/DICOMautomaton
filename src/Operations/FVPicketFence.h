// FVPicketFence.h.

#pragma once

#include <string>
#include <map>

#include "../Structs.h"


OperationDoc OpArgDocFVPicketFence(void);

Drover
FVPicketFence(Drover DICOM_data, 
              OperationArgPkg /*OptArgs*/,
              std::map<std::string, std::string> /*InvocationMetadata*/,
              std::string /*FilenameLex*/);
