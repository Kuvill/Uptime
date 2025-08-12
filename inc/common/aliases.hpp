#include <memory>

// RAII wrapper
// use constr for init
// @note this .get so annoying
using C_string = std::unique_ptr<char[]>;
