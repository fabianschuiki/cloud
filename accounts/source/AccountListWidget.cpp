/*
 * Copyright © 2012 Fabian Schuiki
 */

#include <gdkmm/general.h>
#include <iostream>

#include "AccountListWidget.h"


AccountListWidget::AccountListWidget():
	Glib::ObjectBase("AccountListWidget"),
	Gtk::Widget()
{
	set_has_window(true);
}

AccountListWidget::~AccountListWidget()
{
}

void AccountListWidget::on_realize()
{
	set_realized();
	
	if (!gdkWindow) {
		GdkWindowAttr attributes;
		memset(&attributes, 0, sizeof attributes);
		
		Gtk::Allocation allocation = get_allocation();
		attributes.x = allocation.get_x();
		attributes.y = allocation.get_y();
		attributes.width = allocation.get_width();
		attributes.height = allocation.get_height();
		
		attributes.event_mask = get_events() | Gdk::EXPOSURE_MASK;
		attributes.window_type = GDK_WINDOW_CHILD;
		attributes.wclass = GDK_INPUT_OUTPUT;
		
		gdkWindow = Gdk::Window::create(get_parent_window(), &attributes, GDK_WA_X | GDK_WA_Y);
		gdkWindow->set_user_data(gobj());
		set_window(gdkWindow);
	}
}

void AccountListWidget::on_unrealize()
{
	gdkWindow.reset();
	Gtk::Widget::on_unrealize();
}

void AccountListWidget::on_size_allocate(Gtk::Allocation &allocation)
{
	set_allocation(allocation);
	
	if (gdkWindow) {
		gdkWindow->move_resize(
			allocation.get_x(),
			allocation.get_y(),
			allocation.get_width(),
			allocation.get_height());
	}
}

bool AccountListWidget::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
}
