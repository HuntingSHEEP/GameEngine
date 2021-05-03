#include <gtk/gtk.h>
#include <iostream>

using namespace std;

int n = 1;
int z = 1;

int x = 40;
int y = 40;

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
    GdkRGBA color = { 1,1,1,1};

    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);

    //cairo_arc (cr, n, height / 2.0,MIN (width, height) / 2.0,0, 2 * G_PI);

    cairo_rectangle(cr, 0, 0, width, height);
    gdk_cairo_set_source_rgba (cr, &color);
    cairo_fill (cr);

    color = { 0,0,1,1};
    cairo_rectangle(cr, 10+n, 500+z, 50, 50);
    gdk_cairo_set_source_rgba (cr, &color);
    cairo_fill (cr);

    cairo_move_to(cr, x, y);
    cairo_line_to(cr, 400, 160);
    cairo_close_path(cr);

    cairo_stroke_preserve(cr);
    cairo_fill(cr);

    gtk_widget_queue_draw(widget);
    return FALSE;
}

void buttonFunction (GtkButton *button, gpointer user_data) /* No extra parameter here ! */
{
    /* you cast to the type of what you passed as last argument of g_signal_connect */
    int *pn = (gint *) user_data;
    *pn += 10;
    //cout<<*pn<<endl;
}

gboolean wcisnietoGuzik(GtkWidget *widget, GdkEventKey *event, gpointer data){
    cout<<"Wciśnięto guzik: "<<event->keyval<<"; leftArrow: "<<GDK_KEY_leftarrow<<endl;

    if(event->keyval == GDK_KEY_space){
        z--;
        return true;
    }else if(event->keyval == 65362){
        z-=5;
        return true;
    }else if(event->keyval == 65364){
        z+=5;
        return true;
    }else if(event->keyval == 65361){
        n-=5;
        return true;
    }else if(event->keyval == 65363){
        n+=5;
        return true;
    }

    return false;
}

static gboolean mouse_moved(GtkWidget *widget,GdkEvent *event, gpointer user_data) {

    if (event->type==GDK_MOTION_NOTIFY) {
        GdkEventMotion* e=(GdkEventMotion*)event;
        printf("Coordinates: (%u,%u)\n", (guint)e->x,(guint)e->y);
        x = (guint)e->x;
        y = (guint)e->y;
    }
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
    gtk_window_set_default_size(GTK_WINDOW(okno), 1000+10+10, 700+50+10);
    gtk_window_set_position(GTK_WINDOW(okno), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(okno), "Dzień dobry");



    g_signal_connect(G_OBJECT(okno), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(okno), "configure-event", G_CALLBACK(wyswietlaniePozycjiOkna), NULL);

    GtkWidget *drawing_area = gtk_drawing_area_new ();
    gtk_widget_set_size_request (drawing_area, 1000, 700);
    g_signal_connect (G_OBJECT (drawing_area), "draw",G_CALLBACK (draw_callback), NULL);
    gtk_fixed_put(GTK_FIXED(kontener), drawing_area, 10, 50);

    //OBSŁUGA KLAWIATURY

    gtk_widget_add_events(okno, GDK_KEY_PRESS_MASK);
    g_signal_connect(G_OBJECT(okno), "key_press_event", G_CALLBACK(wcisnietoGuzik), NULL);


    //obsługa myszy

    g_signal_connect (G_OBJECT (okno), "motion-notify-event",G_CALLBACK (mouse_moved), NULL);
    gtk_widget_set_events(okno, GDK_POINTER_MOTION_MASK);


    gtk_fixed_put(GTK_FIXED(kontener), przycisk, 100, 50);

    gtk_widget_show_all(okno);
    gtk_main();
    return 0;
}

