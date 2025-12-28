#pragma once

#include "demon/process_info.hpp"

#include <ostream>

std::ostream& operator<<( std::ostream& os, const ProcessInfo& info );

ProcessInfo FocusInfo();
