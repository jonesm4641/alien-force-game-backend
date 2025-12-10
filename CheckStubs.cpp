#include <string>
#include <map>

// Dummy implementations to satisfy unresolved externals coming from MySQL sample code.
// They do nothing and are safe to leave in your project.

void check(const std::string&)
{
    // no-op
}

void check(const std::map<std::string, std::string>&)
{
    // no-op
}
