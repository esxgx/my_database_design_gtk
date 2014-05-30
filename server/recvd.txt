#include <gtk/gtk.h>

gint on_sendmsg(GtkButton *sendbtn, GtkWidget *args[])
{
	GtkTextBuffer *ipbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(args[0]));
	GtkTextBuffer *portbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(args[1]));
	GtkTextBuffer *msgbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(args[2]));

	GtkTextIter start,end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ipbuf),&start,&end);
	char *clientip = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(ipbuf),&start,&end,0);
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(portbuf),&start,&end);
	char *port = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(portbuf),&start,&end,0);
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(msgbuf),&start,&end);
	char *msg = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(msgbuf),&start,&end,0);

	printf("sendmsg[%s, %s] begin->\n%s\n<-end\n", clientip, port, msg);

	Qmsg_send(clientip, port, msg);
	return FALSE;
}

#include <glib.h>
#include <glib/gi18n.h>

gint on_sendfile(GtkButton *sendbtn, GtkWidget *args[])
{
	GtkTextBuffer *ipbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(args[0]));
	GtkTextBuffer *portbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(args[1]));

	GtkTextIter start,end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ipbuf),&start,&end);
	char *clientip = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(ipbuf),&start,&end,0);
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(portbuf),&start,&end);
	char *port = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(portbuf),&start,&end,0);
	GtkWidget *dialog = gtk_file_chooser_dialog_new(
		"Choose file",
		NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN,
		_("_Cancel"),
		GTK_RESPONSE_CANCEL,
		_("_Open"),
		GTK_RESPONSE_ACCEPT, NULL);
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		char *fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		Qfile_send(clientip, port, fname);
		g_free(fname);
	}
	gtk_widget_destroy(dialog);
	return FALSE;
}

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
	GtkWidget *w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(w), "[Hsiang Kao 2012217000] QQClient");
	gtk_widget_set_size_request(w, 250, 300);
	g_signal_connect(w, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	GtkWidget *HBox1, *HBox2, *HBox3, *HBox4, *VBox;
	GtkWidget *label1 = gtk_label_new("Server IP: ");
	GtkWidget *label2 = gtk_label_new("Server Port: ");
	GtkWidget *label3 = gtk_label_new("Message: ");
	GtkWidget *args[3];

	{
	GtkTextBuffer *txtbuf1 = gtk_text_buffer_new(NULL);
	gtk_text_buffer_set_text(txtbuf1, "127.0.0.1", -1);
	GtkTextBuffer *txtbuf2 = gtk_text_buffer_new(NULL);
	gtk_text_buffer_set_text(txtbuf2, "25514", -1);

	GtkTextBuffer *txtbuf3 = gtk_text_buffer_new(NULL);
	gtk_text_buffer_set_text(txtbuf3, "I Love GNU/Linux.\nI Love Open Source.", -1);

	args[0] = gtk_text_view_new_with_buffer(txtbuf1);
	args[1] = gtk_text_view_new_with_buffer(txtbuf2);
	args[2] = gtk_text_view_new_with_buffer(txtbuf3);
	g_object_unref(txtbuf1),g_object_unref(txtbuf2),
		g_object_unref(txtbuf3);
	}
	//args[2] = gtk_text_view_new();

	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(args[0]),2);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(args[1]),2);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(args[2]),2);

	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(args[0]),4);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(args[1]),4);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(args[2]),4);

	GtkWidget *sendmsgbtn = gtk_button_new_with_label("Send Message");
	g_signal_connect(G_OBJECT(sendmsgbtn),"clicked",G_CALLBACK(on_sendmsg),(gpointer)args);
	GtkWidget *sendfilebtn = gtk_button_new_with_label("Send File");
	g_signal_connect(G_OBJECT(sendfilebtn),"clicked",G_CALLBACK(on_sendfile),(gpointer)args);

	HBox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	HBox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	HBox3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	HBox4 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

	VBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);

	gtk_box_pack_start(GTK_BOX(HBox1),label1,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(HBox1),args[0],FALSE,FALSE,0);

	gtk_box_pack_start(GTK_BOX(HBox2),label2,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(HBox2),args[1],FALSE,FALSE,0);

	gtk_box_pack_start(GTK_BOX(HBox3),label3,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(HBox3),args[2],FALSE,FALSE,0);

	gtk_box_pack_start(GTK_BOX(HBox4),sendmsgbtn,FALSE,FALSE,0);


	gtk_box_pack_start(GTK_BOX(HBox4),sendfilebtn,FALSE,FALSE,0);

	gtk_box_pack_start(GTK_BOX(VBox), HBox1, FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(VBox), HBox2, FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(VBox), HBox3, FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(VBox), HBox4, FALSE,FALSE,0);

	gtk_container_add(GTK_CONTAINER(w), VBox);
	gtk_widget_show_all(w);

	gtk_main();
	return 0;
}

