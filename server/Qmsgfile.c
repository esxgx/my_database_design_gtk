#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct temp_data {
	char clientip[64];
	int len;
	char msg[0];
};

_Qmsg_put(gpointer _data)
{
	struct temp_data *data = (struct temp_data *)_data;
	time_t tt = time(NULL);
	struct tm *t = localtime(&tt);
	additem(asctime(t), data->clientip, data->msg);
	free(_data);
	return G_SOURCE_REMOVE;
}

#include <fcntl.h>

#ifndef O_BINARY
#define O_BINARY	0
#endif

#include <glib/gi18n.h>

_Qfile_put(gpointer _data)
{
	struct temp_data *data = (struct temp_data *)_data;
	GtkWidget *dialog = gtk_file_chooser_dialog_new(
		"Save file",
		NULL,
		GTK_FILE_CHOOSER_ACTION_SAVE,
		_("_Cancel"),
		GTK_RESPONSE_CANCEL,
		_("_Save"),
		GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		char *fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		int fd = open(fname, O_WRONLY | O_BINARY | O_CREAT, 0777);
		write(fd, data->msg, data->len);
		close(fd);
		g_free(fname);
	}
	gtk_widget_destroy(dialog);

	return G_SOURCE_REMOVE;
}

Qmsg_put(char *clientip, char *msg, int len)
{
	struct temp_data *data = (struct temp_data *)malloc(sizeof(struct temp_data) + len);
	strcpy(data->clientip, clientip);
	memcpy(data->msg, msg, data->len = len);
	data->msg[len-1] = '\0';
	db_addmsg(data->clientip, data->msg);
	gdk_threads_add_idle(_Qmsg_put, (gpointer)data);
}

#define NOTIFICATION	"[post_file]"

Qfile_put(char *clientip, char *buf, int len)
{
	Qmsg_put(clientip, NOTIFICATION, sizeof(NOTIFICATION));

	struct temp_data *data = (struct temp_data *)malloc(sizeof(struct temp_data) + len);
	strcpy(data->clientip, clientip);
	memcpy(data->msg, buf, data->len = len);
	gdk_threads_add_idle(_Qfile_put, (gpointer)data);
}
