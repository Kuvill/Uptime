#include <gtk/gtk.h>

static const char* APP_COL_ID = "appName";
static const char* UPTIME_COL_ID = "uptime";

GListStore* setup_column_view( GtkBuilder* builder );
