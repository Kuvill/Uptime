#include <memory>

// RAII wrapper
// use constr for init
// @note this .get so annoying
using c_string = std::unique_ptr<const char[]>;
using ulong = unsigned long;
