#include <Servo.h>
#include <Math.h>

//Valores de trabalho dos servos
#define srv_min 544
#define srv_max 2600

//Parametros da Base
#define b0 544			//544
#define b90 1472		//1610
#define b180 2600		//2750

//Parametros da Haste
#define h0 544			//610
#define h90 1550		//1472
#define h180 2600		//2550

//Parametros da Caneta
#define up 1100
#define down 1900
#define caneta(a) (biela.writeMicroseconds(a));

//Tamanho das hastes
#define d1 7.5
#define d2 11

//Função Sign
#define sign(a) (a >= 0? 1: -1)

//Diferença absoluta entre dois valores
#define absdif(a,b) (max(a,b)-min(a,b))

//Verifica se está entre dois limites
#define lim(a, b, c) ((a >= b && a <= c)? a : 999)

//Posição de estacionamento dos servos
#define park_position 90

//Delay para a função move servo
#define move_delay 40

#define UM 		49
#define DOIS 	50
#define TRES 	51
#define QUATRO 	52
#define CINCO 	53
#define SEIS 	54

Servo base, haste, biela;

float ang1 = 0;
float ang2 = 0;

//Thread
unsigned long last1 = 0; 
unsigned long last2 = 0;

const long trigger1 = 0;
const long trigger2 = 0;

byte incomingByte = 0;

void setup()
{
	//Comunicação Serial
	Serial.begin(9600);
	Serial.println("Iniciando\n");

	//Inicializa Servo Motores
	base.attach(9, b0, b180);
	haste.attach(10, h0, h180);
	biela.attach(11, srv_min, srv_max);

	caneta(up);

	move(base, 5);
	move(haste, 155);

	caneta(down);
	delay(500);

	while(base.read() < 22)
	{
		base.write(base.read() + 1);
		haste.write(haste.read() - 2);
		delay(20);
	}
	delay(1000);	

	while(base.read() < 29)
	{
		base.write(base.read() + 1);
		haste.write(haste.read() + 2);
		delay(20);
	}
	delay(1000);	

	caneta(up);
	park();
}


void loop()
{
	if (Serial.available() > 0) 
	{
        incomingByte = Serial.read();
        switch_byte(incomingByte);
    }
}

void switch_byte(byte b)
{
	switch(b)
	{
		case UM:		base.write(base.read() - 1);
						debug();
						break;
		case DOIS:		base.write(base.read() + 1);
						debug();
						break;
		case TRES:		haste.write(haste.read() - 1);
						debug();
						break;
		case QUATRO:	haste.write(haste.read() + 1);
						debug();
						break;
		case CINCO:		pen();
						debug();
						break;
		case SEIS:		park();
						break;
	}
}

void debug()
{
    Serial.println("==================================");
	Serial.print("Base  ");
	Serial.println(base.read());
	Serial.print("Haste ");
	Serial.println(haste.read());
	Serial.println("==================================");
}
void pen()
{	
	delay(200);
	caneta(down);
	delay(200);
	caneta(up);
	delay(200);
}

void move(Servo srv, int tgt_pos)
{

	short int cur_pos = 0;
	short int dif = 0;

	do
	{
		cur_pos = srv.read();
		dif = cur_pos - tgt_pos;

	    srv.write(cur_pos + (1 * -sign(dif)));

	    delay(move_delay);

	} while (abs(dif) > 0);
	srv.write(srv.read() + (1 * sign(dif)));	
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
