#pragma once

#include <inc/stack_pages.hpp>

#include <gtkmm/applicationwindow.h>
#include <gtkmm/application.h>
#include <gtkmm/stack.h>

#include <libadwaitamm.h>

namespace kvl
{
class Uptimer;
class MainWindow;

class MainWindow final : public Adw::ApplicationWindow {
protected:
    Uptimer& app;
    Gtk::Window window_;
    Gtk::Stack stack_;
    
    // really should be pointers to save memory
    // also for SOLID they mb shuld be spliced to an struct
    ChartPage chart_;
    TablePage table_;
    AboutPage about_;

public:
    MainWindow( Uptimer& app );

};

class Uptimer final : public Adw::Application{
protected:
    // mb should be an pointer?
    kvl::MainWindow mainWindow_;

public:
    Uptimer();

    // i have to override tihs func to correct create
    static Glib::RefPtr<Uptimer> create();

protected:
    void on_startup() override;
};

}
