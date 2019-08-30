#include <Servo.h>

//Valores de trabalho dos servos
#define srv_min 544
#define srv_max 2750

//Parametros da Base
#define b0 544			//544
#define b90 1610		//1610
#define b180 2750		//2750

//Parametros da Haste
#define h0 610			//610
#define h90 1472		//1472
#define h180 2600		//2550

//Tamanho das hastes
#define d1 10
#define d2 8

//Função Sign
#define sign(a) (a >= 0? 1: -1)

//Diferença absoluta entre dois valores
#define dif(a,b) (max(a,b)-min(a,b))

//Posição de estacionamento dos servos
#define park_position 1500

//Delay para a função move servo
#define move_delay 2

Servo base, haste;

boolean run = false;

enum base_sides
{
	neutral,
	cima,
	baixo
} base_ang = neutral;

void setup() 
{
	Serial.begin(9600);

	base.attach(9, b0, b180);
	haste.attach(10, h0, h180);

	delay(2000);
}

void loop() 
{
	if (!run)
	{		
		int points[] = {12, 12, 7, 12, 7, 11, 7, 10, 7, 9};
		int b =  run_dots(points, 5, 2000);

		delay(2000);
		park();
		run = true;
	}

}

unsigned int haste1(float x1, float y1)
{
	float h = sqrt(sq(x1) + sq(y1));

	float alpha = atan2(y1, x1);

	float betha = acos((sq(d1) - sq(d2) + sq(h))/(2*d1*h));

	//Serial.println(dg2ms90(degrees(alpha+betha), b0, b90));
	//Serial.println(dg2ms180(degrees(alpha+betha), b0, b180));

	//return map(dg2ms180(degrees(alpha + betha)), 544, 1472, b0, b90);
	//return dg2ms90(degrees(alpha+betha), b0, b90);
	return dg2ms180(degrees(alpha+betha), b0, b180);
}

unsigned int haste2(float x2, float y2)
{
	float h = sqrt(sq(x2) + sq(y2));

	float gama = acos((-sq(h) + sq(d1) + sq(d2))/(2*d1*d2));

	//Serial.println(dg2ms90(degrees(gama) - 90, h0, h90));
	//Serial.println(dg2ms180(degrees(gama) - 90, ah0, h180));

	//return map(dg2ms180(degrees(gama) - 90), 544, 1472, h0, h90);
	//return dg2ms90(degrees(gama) - 90, h0, h90);
	return dg2ms180(degrees(gama) - 90, h0, h180);
}

int run_dots(int dots[], int size, int dtime)
{
	for (int i = 0; i < 2 * size; i+= 2)
	{
		move(base, haste1(dots[i], dots[i+1]));
		move(haste, haste2(dots[i], dots[i+1]));
		delay(dtime);
	}
	return 0;
}

int run_dots2(int dots[], int size, int dtime)
{
	move(base, haste1(dots[0], dots[1]));
	move(haste, haste2(dots[0], dots[1]));
	delay(2000);

	float ang1 = haste1(dots[2], dots[3]);
	float ang2 = haste2(dots[2], dots[3]);

	tween(base, ang1, haste, ang2);
	delay(2000);

	return 0;
}

void tween(Servo srv1, int tgt1, Servo srv2, int tgt2)
{
	float dif1 = 0, dif2 = 0;
	float cur1 = 0, cur2 = 0;

	cur1 = srv1.readMicroseconds();
	cur2 = srv2.readMicroseconds();

	dif1 = dif(tgt1, cur1);
	dif2 = dif(tgt2, cur2);

	float amount2 = (dif2/dif1);

	int frames = min(dif1, dif2);

	do
	{
		cur1 = srv1.readMicroseconds();
		cur2 = srv2.readMicroseconds();

		dif1 = cur1 - tgt1;
		dif2 = cur2 - tgt2;

		if (dif(tgt1, cur1) > dif(tgt2, cur2))
		{
			//Servo 2 está mais proximo, utilizar amount1 para servo 2
	    	srv1.writeMicroseconds(cur1 + round((amount2 * -sign(dif2))));
	    	srv2.writeMicroseconds(cur2 + (1 * -sign(dif2)));
		}
		else 
		{
			//Servo 1 está mais proximo, utilizar amount1 para servo 1
			srv1.writeMicroseconds(cur1 + (1 * -sign(dif1)));
			srv2.writeMicroseconds(cur2 + (round((amount2 * -sign(dif2)))));
		}

		delay(50);

		frames--;

	} while (frames > 0);
	
	srv1.writeMicroseconds(srv1.readMicroseconds() + (1 * sign(dif1)));
	srv2.writeMicroseconds(srv2.readMicroseconds() + (1 * sign(dif2)));

}

void park()
{
	move(base, park_position);
	move(haste, park_position);
	delay(100);
}

int dg2ms(float degrees, short int vmin, short int vmax, short int deg_amount)
{
	//Converte valor em grau para microsegundos
	return round((((vmax - vmin) * degrees)/deg_amount) + vmin);
}

int dg2ms90(float degrees, float vmin, float vmax)
{
	//vmin = valor para 0°
	//vmax = valor para 90°
	return round((((vmax - vmin) * degrees)/90) + vmin);
}

int dg2ms180(float degrees, float vmin, float vmax)
{
	//vmin = valor para 0°
	//vmax = valor para 180°
	return round(((vmax - vmin) * degrees)/180 + vmin);
}

int mapf(float x, float in_min, float in_max, float out_min, float out_max)
{

  return round((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void move(Servo srv, float tgt_pos)
{
	short int cur_pos = 0;
	short int dif = 0;

	tgt_pos = constrain(tgt_pos, srv_min, srv_max);

	do
	{
		cur_pos = srv.readMicroseconds();
		dif = cur_pos - tgt_pos;

	    srv.writeMicroseconds(cur_pos + (1 * -sign(dif)));
	    
	    //Serial.println(srv.readMicroseconds());

	    delay(move_delay);

	} while (abs(dif) > 0);
	srv.writeMicroseconds(srv.readMicroseconds() + (1 * sign(dif)));	
}