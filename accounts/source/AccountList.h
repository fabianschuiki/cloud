/*
 * Copyright © 2012 Fabian Schuiki
 */

#pragma once
#include <gtkmm/widget.h>


class AccountList : public Gtk::Widget
{
public:
	AccountList();
	virtual ~AccountList();
	
protected:
	virtual void on_realize();
	virtual void on_unrealize();
	virtual void on_size_allocate(Gtk::Allocation &allocation);
	virtual bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr);
	
	Glib::RefPtr<Gdk::Window> gdkWindow;
};
