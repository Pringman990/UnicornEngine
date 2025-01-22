#include "pch.h"
#include "Application/Windows/WindowsApplicationMisc.h"
#include "Application/Windows/WindowsApplication.h"

GenericApplication* WindowsApplicationMisc::Create()
{
    return new WindowsApplication();
}
