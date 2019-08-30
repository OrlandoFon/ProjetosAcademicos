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
//#define dif(a,b) (max(a,b)-min(a,b))

//Posição de estacionamento dos servos
#define park_position 1500

//Delay para a função move servo
#define move_delay 2

Servo base, haste;

float ang1 = 0;
float ang2 = 0;

void setup()
{
	//Comunicação Serial
	Serial.begin(9600);
	Serial.println("Iniciando");

	//Inicializa Servo Motores
	base.attach(9, b0, b180);
	haste.attach(10, h0, h180);

	delay(5000);

	movedot(12, 12);
	movedot(12, 11);
	movedot(12, 10);

	movedot(11, 12);
	movedot(11, 11);
	movedot(11, 10);


	movedot(10, 12);
	movedot(10, 11);
	movedot(10, 10);

	park();
}

void loop()
{

}

void movedot(float x, float y)
{	
	//Executa cinemática inversa para o ponto cartesiano
	cinematic(x, y);

	//Verifica disponibilidade do ponto
	if (ang1 != 999 && ang2 != 999)
	{
		//O ponto está localizado no work envelope
		debugdot(x, y, true);

		//Corrige Distorção do Angulo
		ang1 = dg2ms180(ang1, b0, b180);
		ang2 = dg2ms180(ang2, h0, h180);

		//Executa Movimento
		move(base, ang1);
		move(haste, ang2);
	}
	else 
	{
		//O ponto não está dentro do work envelope
		debugdot(x, y, false);
	}

	delay(2000);
}

void cinematic(float x, float y)
{
	//Valores possíveis para Theta1
	float t2a = (acos((sq(x) + sq(y) - sq(d1) - sq(d2))/(2*d1*d2)));
	float t2b = -t2a;

	//Valores possíveis para Theta2
	float t1a = atan((y*(d1 + d2*cos(t2a)) - x*d2*sin(t2a))/(x*(d1+d2*cos(t2a)) + y*(d2*sin(t2a))));
	float t1b = atan((y*(d1 + d2*cos(t2b)) - x*d2*sin(t2b))/(x*(d1+d2*cos(t2b)) + y*(d2*sin(t2b))));

	//Converte valores para graus e adapta os eixos
	t2a = degrees(t2a) + 90;
	t2b = degrees(t2b) + 90;
	t1a = degrees(t1a);
	t1b = degrees(t1b);

	//Classifica angulos a serem utilizados
	ang2 = (t2a < 0 || t2a > 180)? t2b : t2a;
	ang1 = (ang2 == t2a)? t1a : t1b;

	//Verifica se o ponto está no work envelope
	ang2 = (ang2 >= 0 && ang2 <= 180)? ang2 : 999;
	ang1 = (ang1 >= 0 && ang1 <= 180)? ang1 : 999;
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

	    delay(move_delay);

	} while (abs(dif) > 0);
	srv.writeMicroseconds(srv.readMicroseconds() + (1 * sign(dif)));	
}

int dg2ms180(float degrees, float vmin, float vmax)
{
	//vmin = valor para 0°
	//vmax = valor para 180°
	return round(((vmax - vmin) * degrees)/180 + vmin);
}

void park()
{
	move(base, park_position);
	move(haste, park_position);
	delay(100);
}

void debugdot(float x, float y, boolean exist)
{
	//Envia mensagem dizendo se o ponto está no work envelope ou não
	if(exist)
	{
		Serial.print("O ponto (");
		Serial.print(x);
		Serial.print(", ");
		Serial.print(y);
		Serial.println(") existe, executando movimento");
	}
	else
	{
		Serial.print("O ponto (");
		Serial.print(x);
		Serial.print(", ");
		Serial.print(y);
		Serial.println(") nao existe, executando proximo ponto");
	}
}

