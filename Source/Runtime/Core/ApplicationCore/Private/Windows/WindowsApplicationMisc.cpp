#include "pch.h"
#include "Windows/WindowsApplicationMisc.h"
#include "Windows/WindowsApplication.h"

GenericApplication* WindowsApplicationMisc::Create()
{
    return new WindowsApplication();
}
