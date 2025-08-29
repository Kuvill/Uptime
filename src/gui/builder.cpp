#include "gui/builder.hpp"

#include <filesystem>
#include <common/logger.hpp>

#include <gui/column_view.hpp>

void setup_builder( GtkBuilder *builder ) {
    if( builder == nullptr ) {
        logger.log(LogLvl::Error, "main.ui file was not found! use ninja install or copy it manualy into ", std::filesystem::current_path() );

        throw std::runtime_error("");
    }

    setup_column_view( builder );
    setup_stack( builder );
}
