#include "common/logger.hpp"
#include "gui/record_item.hpp"
#include "gui/column_view.hpp"
#include "gui/lazy_load.hpp"
#include "common/time.hpp"

#include <gtk/gtk.h>
#include <chrono>
#include <cstdio>
#include <format>


/* 
   // Should be in context 
   struct Resources {
    GtkWidget* stack;
    GtkWidget* page1;
    GtkWidget* page2;
    gboolean page1_loaded;
    gboolean page2_loaded;
   }
*/

static std::string oneUnitTime( const recTime_t& time ) {
    using namespace std::literals::chrono_literals;

    std::chrono::hh_mm_ss<std::chrono::seconds> date( time );
    if( date.hours() >= 1h )
        return std::format( "{} {}", date.hours(), date.minutes() );

    if( date.minutes() >= 1min )
        return std::format( "{} {}", date.minutes(), date.seconds() );

    return std::format("{}", date.seconds() );
}

// add second field info (like min::sec)
static std::string timeToStr( const recTime_t& time ) {
    return oneUnitTime( time );
}

static void on_year_selected( Context* data ) {
    logger.log(LogLvl::Error, "mb double: changed to year");
    data->state.changeModel(Duration::ByYear);
}


// create widget for an field of RecordItem
// @factory - GtkFactory, that will create an widget
// @item_list - here store our items for MODEL, not factor.
// so just change in list will not present on widget
static void setup_cb( GtkSignalListItemFactory* factory, GtkListItem* item_list ) {
    logger.log(LogLvl::Info, "Columnn view setup called");
	GtkWidget* label = gtk_label_new( nullptr );
	gtk_label_set_xalign( GTK_LABEL( label ), 0.0f );
	gtk_list_item_set_child( item_list, label );
}

static void on_name_changed(GObject *object, GParamSpec *pspec, GtkWidget *label) {
    const gchar *new_name = static_cast<gchar*>( g_object_get_data(object, "name") );
    g_object_set(label, "label", new_name, NULL);
}

static void on_uptime_changed(GObject *object, GParamSpec *pspec, GtkWidget *label) {
    guint64 new_name;
    g_object_get(object, "uptime", &new_name, NULL);

    g_object_set(label, "label", timeToStr( recTime_t( new_name ) ).c_str(), NULL);
}

// fill an created by setup widget
// @factory - GtkFactory, that will create an widget
// @item_list - here store our items for MODEL, not factor.
// so just change in list will not present on widget
static void bind_appName_cb( GtkListItemFactory* factory, GtkListItem* listItem ) {
    logger.log(LogLvl::Info, "an record(appname) was binded to ColumnView");
    GtkWidget* label = gtk_list_item_get_child( listItem );
    RecordItem* rec = static_cast<RecordItem*>( gtk_list_item_get_item( listItem ) );

    g_signal_connect(rec, "notify::name", G_CALLBACK(on_name_changed), label); 

    gtk_label_set_text( GTK_LABEL( label ), rec->appName );

}

// fill an created by setup widget
// @factory - GtkFactory, that will create an widget
// @item_list - here store our items for MODEL, not factor.
// so just change in list will not present on widget
static void bind_update_cb( GtkListItemFactory* factory, GtkListItem* listItem ) {
    logger.log(LogLvl::Info, "an record(uptime) was binded to ColumnView");
    GtkWidget* label = gtk_list_item_get_child( listItem );
    RecordItem* rec = static_cast<RecordItem*>( gtk_list_item_get_item( listItem ) );

    g_signal_connect(rec, "notify::uptime", G_CALLBACK(on_uptime_changed), label); 

    gtk_label_set_text( GTK_LABEL( label ), timeToStr( rec->uptime ).c_str() );
}

GListStore* setup_column_view( GtkBuilder* builder, Context& context ) {
	auto* columnView =  GTK_COLUMN_VIEW(gtk_builder_get_object( builder, "column_view" ));

	auto* appNameCol = GTK_COLUMN_VIEW_COLUMN(gtk_builder_get_object( builder, "app_name_col" ));
	auto* uptimeCol = GTK_COLUMN_VIEW_COLUMN(gtk_builder_get_object( builder, "uptime_col" ));

    GListStore* store = g_list_store_new( RECORD_ITEM_TYPE );
    context.state.setStore( store );

    GtkSingleSelection* model = gtk_single_selection_new( G_LIST_MODEL(store) );
	gtk_column_view_set_model(columnView, GTK_SELECTION_MODEL( model ) );

	// App name factory
	GtkListItemFactory* appNameFactory = gtk_signal_list_item_factory_new();
	g_signal_connect( appNameFactory, "setup", G_CALLBACK(setup_cb), nullptr );
	g_signal_connect( appNameFactory, "bind", G_CALLBACK(bind_appName_cb), nullptr );

	gtk_column_view_column_set_factory( appNameCol, appNameFactory );

	// Uptime factory
	GtkListItemFactory* uptimeFactory = gtk_signal_list_item_factory_new();
	g_signal_connect( uptimeFactory, "setup", G_CALLBACK(setup_cb), nullptr );
	g_signal_connect( uptimeFactory, "bind", G_CALLBACK(bind_update_cb), nullptr );

	gtk_column_view_column_set_factory( uptimeCol, uptimeFactory );

    // Fill table from db
    auto* stack = GTK_STACK( gtk_builder_get_object( builder, "body" ) );
    g_signal_connect( stack, "notify::visible-child", G_CALLBACK(on_stack_page_changed), &context );
    on_stack_page_changed(stack, 0, nullptr);

    // setup menu
    // here is invalide pointer instance and type check errors
    // auto* year_item = G_MENU_ITEM( gtk_builder_get_object( builder, "years" ) );
    // g_signal_connect(year_item, "activate", G_CALLBACK(on_year_selected), &context);


    return store;
}
