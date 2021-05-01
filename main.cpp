#include <gtk/gtk.h>
#include <iostream>

using namespace std;

int n = 1;

void wyswietlaniePozycjiOkna(GtkWindow *okno, GdkEvent *zdarzenie, gpointer dane){
    int x, y;
    char polozenie[10];
    x = zdarzenie->configure.x;
    y = zdarzenie->configure.y;

    sprintf(polozenie, "%d, %d", x, y);
    gtk_window_set_title(okno, polozenie);
}

gboolean draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    guint width, height;
    GdkRGBA color;




    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);
    cairo_arc (cr,
               n, height / 2.0,
               MIN (width, height) / 2.0,
               0, 2 * G_PI);

    gtk_style_context_get_color (gtk_widget_get_style_context (widget), GTK_STATE_FLAG_ACTIVE,&color);
    gdk_cairo_set_source_rgba (cr, &color);

    cairo_fill (cr);

    gtk_widget_queue_draw(widget);

    return FALSE;
}

void buttonFunction (GtkButton *button, gpointer user_data) /* No extra parameter here ! */
{
    /* you cast to the type of what you passed as last argument of g_signal_connect */
    int *pn = (gint *) user_data;
    *pn += 10;
    cout<<*pn<<endl;
}

int main (int argc, char *argv[]) {
    GtkWidget *okno;
    GtkWidget *kontener, *przycisk;
    gtk_init (&argc, &argv);

    okno = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    kontener = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(okno), kontener);
    przycisk = gtk_button_new_with_label("ELOOPSZ");


    g_signal_connect(G_OBJECT(przycisk), "clicked", G_CALLBACK(buttonFunction), &n);



    gtk_widget_set_size_request(przycisk, 180, 35);
    gtk_window_set_default_size(GTK_WINDOW(okno), 500, 500);
    gtk_window_set_position(GTK_WINDOW(okno), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(okno), "Dzień dobry");

    gtk_fixed_put(GTK_FIXED(kontener), przycisk, 100, 10);

    g_signal_connect(G_OBJECT(okno), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(okno), "configure-event", G_CALLBACK(wyswietlaniePozycjiOkna), NULL);

    GtkWidget *drawing_area = gtk_drawing_area_new ();
    gtk_widget_set_size_request (drawing_area, 100, 100);
    g_signal_connect (G_OBJECT (drawing_area), "draw",G_CALLBACK (draw_callback), NULL);



    gtk_fixed_put(GTK_FIXED(kontener), drawing_area, 10, 200);


    gtk_widget_show_all(okno);
    gtk_main();
    return 0;
}