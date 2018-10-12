/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "EASTLTest.h"
#include <EASTL/system_error.h>

using namespace eastl;

int TestSystemError()
{
	int nErrorCount = 0;
    
    VERIFY(static_cast<int>(errc::invalid_argument) == EINVAL);

    return nErrorCount;
}
