#include "Utility/FileDialog.h"

#if defined (OS_LINUX)
#include <unistd.h>
#include <linux/limits.h>
#endif

#include <vector>

namespace FileDialog {

std::string Open()
{
    return "";
}

std::string Open (char *buffer, size_t size)
{
    return "";

}

std::string Save()
{
    return "";

}


}
