#pragma once
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

namespace shell
{

inline int exec(const std::string cmd)
{
    int ret_code = std::system(("set -euo pipefail\n" + cmd + " >/dev/null 2>&1").c_str());
    if (ret_code != 0)
    {
        printf("ERROR: failed to execute %s\n", cmd.c_str());
    }
    return ret_code;
}

inline std::string valueof(const std::string cmd)
{
    std::string result;

    FILE* pipe = popen(("set -euo pipefail\n" + cmd).c_str(), "r");
    if (pipe == NULL)
    {
        printf("ERROR: failed to execute %s\n", cmd.c_str());
        return "";
    }

    char buffer[256];
    // without checking feof, reading EOF will cause SEGV.
    while(!feof(pipe))
    {
        if(fgets(buffer, sizeof(buffer), pipe) != NULL)
        {
            result.append(buffer);
        }
    }
    pclose(pipe);

    if (result[result.size() - 1] == '\n')
    {
        return result.substr(0, result.size() - 1);
    }
    else
    {
        return result;
    }
}

} // namespace shell