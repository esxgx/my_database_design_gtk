#include <gtk/gtk.h>
#include <glib.h>

GtkListStore *msgls;

gint
on_presslist(GtkWidget *widget,
	GdkEventButton *event, gpointer func_data
) {
	if (event->type == GDK_2BUTTON_PRESS) {
		db_deleteall();
		if (msgls != NULL) gtk_list_store_clear(msgls);
	}
	return FALSE;
}

int additem(gchar *time, gchar *clientip, gchar *msg)
{
	GtkTreeIter iter;
	gtk_list_store_append(msgls, &iter);
	gtk_list_store_set(msgls, &iter,
		0, time,
		1, clientip,
		2, msg, -1);
}

extern gpointer server_init();
#include "cfg.h"

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
	GtkWidget *w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(w), "[Hsiang Kao 2012217000] QQServer PORT: " QPORT);
	gtk_widget_set_size_request(w, 400, 300);
	g_signal_connect(w, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	msgls = gtk_list_store_new(3,
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	
	GtkWidget *treeview =gtk_tree_view_new_with_model(GTK_TREE_MODEL(msgls));
	// Time
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes(
		"Time", renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
	// Client IP
	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
		"Client IP", renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
	// Message
	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes(
		"Message", renderer, "text", 2, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
	
	gtk_widget_add_events(treeview, GDK_BUTTON_PRESS_MASK);
	g_signal_connect(treeview, "button_press_event", G_CALLBACK(on_presslist), NULL);

	gtk_container_add(GTK_CONTAINER(w), treeview);


	gtk_widget_show_all(w);

	// connect the database
	db_init(MYSQL_HOST, MYSQL_USER, MYSQL_PASSWORD, MYSQL_PORT);
	// query saved data
	db_querydata();

	g_thread_new(NULL, server_init, NULL);
	gtk_main();
	return 0;
}