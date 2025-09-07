#include "common/logger.hpp"
#include "glib-object.h"
#include "gui/context.hpp"
#include "gui/record_item.hpp"
#include "gui/column_view.hpp"
#include "common/time.hpp"

#include <gtk/gtk.h>
#include <chrono>
#include <cstdio>
#include <format>


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

// I Have to create custom class, that is Gesture and contain popover
static void table_right_click( GtkGestureClick* self, gint n_press, 
            gdouble x, gdouble y, gpointer data ) {
    
    logger.log(LogLvl::Warning, "Lol, why did you pressed here?");
    // auto* popover = Context::get()->utils.table_menu.popover;

    auto* item = GTK_LIST_ITEM(data);
    // Context::get()->utils.table_menu.selected = item;

    GdkRectangle rec; rec.x = x; rec.y = y;
    // gtk_popover_set_pointing_to( popover, &rec );
    // gtk_popover_popup( popover );
    // gtk_popover_present( popover );
}

// create widget for an field of RecordItem
// @factory - GtkFactory, that will create an widget
// @item_list - here store our items for MODEL, not factor.
// so just change in list will not present on widget
static void setup_cb( GtkSignalListItemFactory* factory, GtkListItem* item_list ) {
    logger.log(LogLvl::Info, "Columnn view setup called");

    auto* eventHandler = gtk_gesture_click_new();
    gtk_gesture_single_set_button( GTK_GESTURE_SINGLE( eventHandler ), GDK_BUTTON_SECONDARY );
    g_signal_connect( eventHandler, "pressed", G_CALLBACK(table_right_click), item_list );

	GtkWidget* label = gtk_label_new( nullptr );
	gtk_label_set_xalign( GTK_LABEL( label ), 0.0f );
	gtk_list_item_set_child( item_list, label );

    gtk_widget_add_controller( label, GTK_EVENT_CONTROLLER(eventHandler) );
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

// update an created by setup widget
// @factory - GtkFactory, that will create an widget
// @item_list - here store our items by MODEL, not factor.
// so just change in list will not present on widget

// FIXME check multi connection by using id or do connect in setup funcion using static variablej
static void bind_appName_cb( GtkListItemFactory* factory, GtkListItem* listItem, gpointer data ) {
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

// ----------------------------------------

GListStore* setup_column_view( GtkBuilder* builder ) {
	auto* columnView =  GTK_COLUMN_VIEW(gtk_builder_get_object( builder, "column_view" ));

	auto* appNameCol = GTK_COLUMN_VIEW_COLUMN(gtk_builder_get_object( builder, "app_name_col" ));
	auto* uptimeCol = GTK_COLUMN_VIEW_COLUMN(gtk_builder_get_object( builder, "uptime_col" ));

    auto* store = g_list_store_new( RECORD_ITEM_TYPE );
    Context::get()->state.setStore( store );

    auto* viewSorter = gtk_column_view_get_sorter( columnView ); // allow sorting by clicking on name
    auto* model = gtk_sort_list_model_new( G_LIST_MODEL(store), viewSorter );
    GtkSingleSelection* selection = gtk_single_selection_new( G_LIST_MODEL(model) ); // do i need selection?
	gtk_column_view_set_model(columnView, GTK_SELECTION_MODEL( selection ) );

    auto* appNameSorter = GTK_SORTER(gtk_custom_sorter_new( RecordItemNameCompare, nullptr, nullptr ));
    auto* uptimeSorter = GTK_SORTER(gtk_custom_sorter_new( RecordItemUptimeCompare, nullptr, nullptr ));

    gtk_column_view_column_set_sorter( appNameCol, appNameSorter );
    gtk_column_view_column_set_sorter( uptimeCol, uptimeSorter );

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

    return store;
}
