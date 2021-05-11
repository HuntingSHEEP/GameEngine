/*
 * sudo apt install libgtk-3-dev
 */

#include <gtk/gtk.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <math.h>


using namespace std;

class Point{
    public:
        double x;
        double y;

        Point(){
            this->x = 0;
            this->y = 0;
        }

        Point(double x, double y){
            this->x = x;
            this->y = y;
        }

        void setPosition(double x, double y){
            this->x = x;
            this->y = y;
        }

        void moveBy(double dx, double dy){
            this->x += dx;
            this->y += dy;
        }

    private:
};

class Line{
    public:
        Point aPoint, bPoint;

        Line(){
            this->aPoint = Point();
            this->bPoint = Point();
        }

        Line(Point A, Point B){
            this->aPoint = A;
            this->bPoint = B;
        }

        void drawMe(cairo_t *cr, GdkRGBA *color){
            //TODO >> SET COLOR METHOD?

            cairo_move_to(cr, aPoint.x, aPoint.y);
            cairo_line_to(cr, bPoint.x, bPoint.y);
            cairo_close_path(cr);
            cairo_stroke_preserve(cr);
            gdk_cairo_set_source_rgba (cr, color);
            cairo_fill(cr);
        }
};

class Crosshair{
    public:
        int R, r;
        Point middlePoint;

        Crosshair(Point middlePoint, int R, int r){
            this->middlePoint = middlePoint;
            this->R = R;
            this->r = r;
            setupLines();
        }

        void setPosition(Point p){
            this->middlePoint.setPosition(p.x, p.y);
            updateLines();
        }

        void setPosition(double x, double y){
            this->middlePoint.setPosition(x, y);
            updateLines();
        }

        void drawMe(cairo_t *cr, GdkRGBA *color){
            lineZero.drawMe(cr, color);
            lineOne.drawMe(cr, color);
            lineTwo.drawMe(cr, color);
            lineThree.drawMe(cr, color);
        }


    private:
        Line lineZero, lineOne, lineTwo, lineThree;

        void setupLines(){
            lineZero = Line(Point(middlePoint.x, middlePoint.y-r), Point(middlePoint.x, middlePoint.y-R));
            lineOne = Line(Point(middlePoint.x+r, middlePoint.y), Point(middlePoint.x+R, middlePoint.y));
            lineTwo = Line(Point(middlePoint.x, middlePoint.y+r), Point(middlePoint.x, middlePoint.y+R));
            lineThree = Line(Point(middlePoint.x-r, middlePoint.y), Point(middlePoint.x-R, middlePoint.y));
        }

        void updateLines(){
            //UPDATE THE UPPER VERTICAL LINE
            lineZero.aPoint.setPosition(middlePoint.x, middlePoint.y-r);
            lineZero.bPoint.setPosition(middlePoint.x, middlePoint.y-R);
            //UPDATE THE RIGHT HORIZONTAL LINE
            lineOne.aPoint.setPosition(middlePoint.x+r, middlePoint.y);
            lineOne.bPoint.setPosition(middlePoint.x+R, middlePoint.y);
            //UPDATE THE LOWER VERTICAL LINE
            lineTwo.aPoint.setPosition(middlePoint.x, middlePoint.y+r);
            lineTwo.bPoint.setPosition(middlePoint.x, middlePoint.y+R);
            //UPDATE THE LEFT HORIZONTAL LINE
            lineThree.aPoint.setPosition(middlePoint.x-r, middlePoint.y);
            lineThree.bPoint.setPosition(middlePoint.x-R, middlePoint.y);
        }
};


class Vector{
public:
    double x, y, z;

    Vector(double x, double y){
        this->x = x;
        this->y = y;
        this->z = 0;
    }

    Vector(double x, double y, double z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void add(Vector vector){
        this->x += vector.x;
        this->y += vector.y;
        this->z += vector.z;
    }

    void add(double x, double y, double z){
        this->x += x;
        this->y += y;
        this->z += z;
    }

    void multiply(double scale){
        this->x *= scale;
        this->y *= scale;
        this->z *= scale;
    }

    double length(){
        return sqrt(x*x + y*y + z*z);
    }
};

class Object{
public:
    int width, height;
    Point leftUpperVertex;

    void setPosition(Point p){
        this->leftUpperVertex.setPosition(p.x, p.y);
    }

    void setPosition(double x, double y){
        this->leftUpperVertex.setPosition(x, y);
    }


    void moveBy(double dx, double dy){
        this->leftUpperVertex.moveBy(dx, dy);
    }

    Point getPosition(){
        return leftUpperVertex;
    }

};

class PhysicComponent: public Object{
public:
    Vector v, a;
    PhysicComponent(Vector v, Vector a) : v(v), a(a){}

    void addForce(Vector vector){
        v.add(vector);
    }

    void addAcceleration(Vector vector){
        a.add(vector);
    }

    void setGravity(double g){
        addAcceleration(Vector(0, g));
    }

    void calculatePhysic(double deltaTime){
        //zmiana położenia
        double dx = v.x*deltaTime + (a.x/2)*deltaTime*deltaTime;
        double dy = v.y*deltaTime + (a.y/2)*deltaTime*deltaTime;
        //nowa prędkość
        double dvx = a.x*deltaTime;
        double dvy = a.y*deltaTime;
        //aktualizacja położenia
        leftUpperVertex.moveBy(dx, dy);
        //aktualizacja prędkości
        v.add(dvx, dvy, 0);
    }
};

class Rectangle: public PhysicComponent{
public:

    Rectangle(int width, int height)
        : PhysicComponent(Vector(0, 0, 0), Vector(0, 0))
    {

        this->width = width;
        this->height = height;
        this->leftUpperVertex = Point(0, 0);

    }

    Rectangle(int width, int height, Point leftUpperVertex)
        : PhysicComponent(Vector(0, 0, 0), Vector(0, 0))
    {
        this->width = width;
        this->height = height;
        this->leftUpperVertex = leftUpperVertex;
    }


    void drawMe(cairo_t *cr, GdkRGBA *color){
        cairo_rectangle(cr, leftUpperVertex.x, leftUpperVertex.y, width, height);
        gdk_cairo_set_source_rgba (cr, color);
        cairo_fill(cr);
    }


};

class Key{
public:
    bool isPressed;
    bool pressedAgain;
    bool resetDone;

    Key(){
        this->isPressed = false;
        this->pressedAgain = false;
        this->resetDone = true;
    }

    Key(bool isPressed, bool pressedAgain){
        this->isPressed = isPressed;
        this->pressedAgain = pressedAgain;
        this->resetDone = true;
    }
};


class Input{
public:
    Key wPressed;
    Key sPressed;
    Key dPressed;
    Key aPressed;

    Input(){
        wPressed = Key();
        sPressed = Key();
        dPressed = Key();
        aPressed = Key();
    }
};


//*****************************************************************************************************************

int n = 1;
int z = 1;

int x = 40;
int y = 40;

int xOffset = -10;
int yOffset = -50;

double t = 0;
double v0 = 0;
double a = 1;

double xk = 700;
double yk = 50;

Line kreska = Line(Point(40, 40), Point(400, 160));
Crosshair celownik = Crosshair(Point(50, 50), 40, 5);

Rectangle kwadrat = Rectangle(40, 40, Point(400, 200));
Rectangle kwadrat2 = Rectangle(40, 40, Point(500, 200));

Input klawiatura = Input();

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
    //białe tło
    cairo_rectangle(cr, 0, 0, width, height);
    gdk_cairo_set_source_rgba (cr, &color);
    cairo_fill (cr);


    color = { 0,0,1,1};
    //cairo_rectangle(cr, xk, yk, 50, 50);
    gdk_cairo_set_source_rgba (cr, &color);

    cairo_fill (cr);


    kwadrat.drawMe(cr, &color);
    color = { 1,0,0,1};
    kwadrat2.drawMe(cr, &color);

    color = { 0,1,1,1};

    //draw to me the LINE
    celownik.drawMe(cr, &color);


    gtk_widget_queue_draw(widget);
    return FALSE;
}

void buttonFunction (GtkButton *button, gpointer user_data) /* No extra parameter here ! */
{
    /* you cast to the type of what you passed as last argument of g_signal_connect */
    int *pn = (gint *) user_data;
    *pn += 10;
    //cout<<*pn<<endl;
    cout<<"BEE:"<<endl;
    kwadrat2.addAcceleration(Vector(0, -15, 0));
}



[[noreturn]] void silnikFizyki(){
    double microsecond = 5*1000;
    double deltaTime = microsecond/150000;
    int skok = 10;

    while(true){

        //OŚ PIONOWA

        if(klawiatura.wPressed.isPressed & (!klawiatura.wPressed.pressedAgain) ){

            kwadrat2.addAcceleration(Vector(0, -skok));
            klawiatura.wPressed.resetDone = false;
            klawiatura.wPressed.pressedAgain = true;
            cout<<"do góry"<<endl;
        }else if(klawiatura.sPressed.isPressed & (! klawiatura.sPressed.pressedAgain)){

            kwadrat2.addAcceleration(Vector(0, skok));
            klawiatura.sPressed.resetDone = false;
            klawiatura.sPressed.pressedAgain = true;
            cout<<"w dół"<<endl;
        }

        //OŚ POZIOMA

        if(klawiatura.aPressed.isPressed & (!klawiatura.aPressed.pressedAgain) ){

            kwadrat2.addAcceleration(Vector(-skok, 0));
            klawiatura.aPressed.resetDone = false;
            klawiatura.aPressed.pressedAgain = true;
            cout<<"w lewo"<<endl;
        }else if(klawiatura.dPressed.isPressed & (! klawiatura.dPressed.pressedAgain)){

            kwadrat2.addAcceleration(Vector( skok, 0));
            klawiatura.dPressed.resetDone = false;
            klawiatura.dPressed.pressedAgain = true;
            cout<<"w prawo"<<endl;
        }




        //blok rezygnacji

        if((!klawiatura.wPressed.isPressed) & (!klawiatura.wPressed.resetDone)){
            kwadrat2.addAcceleration(Vector( 0, skok ));
            klawiatura.wPressed.resetDone = true;
            cout<<"reset do góry"<<endl;
        }else if((!klawiatura.sPressed.isPressed) & (!klawiatura.sPressed.resetDone)){
            kwadrat2.addAcceleration(Vector(0, -skok));
            klawiatura.sPressed.resetDone = true;
            cout<<"reset w dół"<<endl;
        }


        if((!klawiatura.aPressed.isPressed) & (!klawiatura.aPressed.resetDone)){
            kwadrat2.addAcceleration(Vector( skok, 0));
            klawiatura.aPressed.resetDone = true;
            cout<<"reset w lewo"<<endl;
        }else if((!klawiatura.dPressed.isPressed) & (!klawiatura.dPressed.resetDone)){
            kwadrat2.addAcceleration(Vector(-skok, 0));
            klawiatura.dPressed.resetDone = true;
            cout<<"reset w prawo"<<endl;
        }



        kwadrat2.calculatePhysic(deltaTime);
        kwadrat.calculatePhysic(deltaTime);

        usleep( microsecond);//sleeps for 3 second
    }

}


void wcisnietoGuzik(GtkWidget *widget, GdkEventKey *event, gpointer data){
    if(event->keyval == GDK_KEY_w){
        if(!klawiatura.wPressed.isPressed){
            klawiatura.wPressed.isPressed = true;
        }else{
            klawiatura.wPressed.pressedAgain = true;
        }

    }

    if(event->keyval == GDK_KEY_s){
        if(!klawiatura.sPressed.isPressed){
            klawiatura.sPressed.isPressed = true;
        }else{
            klawiatura.sPressed.pressedAgain = true;
        }
    }

    if(event->keyval == GDK_KEY_a) {
        if(!klawiatura.aPressed.isPressed){
            klawiatura.aPressed.isPressed = true;
        }else{
            klawiatura.aPressed.pressedAgain = true;
        }
    }

    if(event->keyval == GDK_KEY_d){
        if(!klawiatura.dPressed.isPressed){
            klawiatura.dPressed.isPressed = true;
        }else{
            klawiatura.dPressed.pressedAgain = true;
        }
    }

    if(event->hardware_keycode == 16){
        cout<<"MOUSE CLICKED"<<endl;
    }

}

void puszczonoGuzik(GtkWidget *widget, GdkEventKey *event, gpointer data){
    //cout<<"puszczono guzik "<<event->keyval<<endl;

    if(event->keyval == GDK_KEY_w){
        klawiatura.wPressed.isPressed = false;
        klawiatura.wPressed.pressedAgain = false;
    }

    if(event->keyval == GDK_KEY_s){
        klawiatura.sPressed.isPressed = false;
        klawiatura.sPressed.pressedAgain = false;
    }

    if(event->keyval == GDK_KEY_a) {
        klawiatura.aPressed.isPressed = false;
        klawiatura.aPressed.pressedAgain = false;
    }

    if(event->keyval == GDK_KEY_d){
        klawiatura.dPressed.isPressed = false;
        klawiatura.dPressed.pressedAgain = false;
    }

}

static gboolean mouse_moved(GtkWidget *widget,GdkEvent *event, gpointer user_data) {

    if (event->type==GDK_MOTION_NOTIFY) {
        GdkEventMotion* e=(GdkEventMotion*)event;
        //printf("Coordinates: (%u,%u)\n", (guint)e->x,(guint)e->y);
        x = (guint)e->x + xOffset;
        y = (guint)e->y + yOffset;

        kreska.aPoint.setPosition(x,y);
        celownik.setPosition(x,y);
        return true;
    }
    return false;
}

int main (int argc, char *argv[]) {
    GtkWidget *okno;
    GtkWidget *kontener, *przycisk;
    gtk_init (&argc, &argv);

    //kwadrat.setGravity(10);
    //kwadrat2.setGravity(5);

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
    g_signal_connect(G_OBJECT(okno), "key_release_event", G_CALLBACK(puszczonoGuzik), NULL);


    //obsługa myszy
    g_signal_connect (G_OBJECT (okno), "motion-notify-event",G_CALLBACK (mouse_moved), NULL);
    g_signal_connect (G_OBJECT (okno), "button-press-event",G_CALLBACK (wcisnietoGuzik), NULL);

    gtk_widget_set_events(okno, GDK_POINTER_MOTION_MASK|GDK_BUTTON_PRESS_MASK|GDK_KEY_PRESS_MASK );


    gtk_fixed_put(GTK_FIXED(kontener), przycisk, 100, 50);

    gtk_widget_show_all(okno);


    std::thread silnik(&silnikFizyki);
    //silnik.join();
    cout<<"uruchomiono wątek, niby"<<endl;


    gtk_main();
    return 0;
}

