#pragma once

#include <gtkmm/box.h>
#include <gtkmm/dialog.h>

namespace kvl {

class ChartPage final : public Gtk::Box {};
class TablePage final : public Gtk::Box {};
class AboutPage final : public Gtk::Box {};
class SettingsDialog final : public Gtk::Dialog {};

}
