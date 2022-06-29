/*
================================================================================================
 *  Trabajo Práctico 2: Sistema de cerradura inteligente con codigo    
 *
 *  Materia: Sistema de Procesamiento de Datos - UTN
 *  Año: 2022
 *  Alumnos: Castello Thiago; Forni Gonzalo; Gatti Ignacio Pablo; Heredia Facundo.
 *
 *  Objetivo:
 *  Desarrollar un sistema capaz de validar una contraseña ingresada a través del
 *  keypad, mediante la utilización de un código. El sistema deberá notificar al usuario
 *  mediante un mensaje en el display y luces leds en caso de que la contraseña sea
 *  correcta o incorrecta.
================================================================================================
*/

/*
================================================================================================
> INCLUDES
================================================================================================
*/
//LIBRERIA PARA USO DE PANTALLA LCD
#include <LiquidCrystal.h>
//LIBRERIA PARA USO DE TECLADO
#include <Keypad.h>

/*
================================================================================================
> DEFINES
================================================================================================
*/
// LARGO CADENADA CARACTERES PASSWORD
#define LARGO_PASSWORD 7
// BOTONTES
#define BOTON_CONFIGURACION A4
#define BOTON_RESET 12
#define BOTON_TEST_PASSWORD 11
// LED Y LCD
#define LED_OK 9
#define LED_FAIL 10
#define TIEMPO_LIMITE_PARPADEO 5000

/*
================================================================================================
> VARIABLES
================================================================================================
*/
//	----------------------------------
//	----[CONFIGURACION CONTRASEÑA]----
//	----------------------------------
char contraseniaPorDefecto[LARGO_PASSWORD] = "1C2022";	// [ contraseña con la que se inicia la cerradura ]
char contrasenia[LARGO_PASSWORD];						// [ contraseña ingresada para comprobar ]
char caracterIngresado;									// [ caracter ingresado que compone la contraseña ]
int indice = 0;											// [ indice de la cadena de password ]

//	-------------------------------
//	----[CONFIGURACION TECLADO]----
//	-------------------------------
//	>> DIMESIONES DEL TECLADO
const byte FILAS = 4;					// [ numero de filas ]
const byte COLUMNAS = 4;				// [ numero de columnas ]
//	>> MATRIZ QUE INTERPRETA EL TECLADO
char teclas [FILAS][COLUMNAS] =			// [ configuracion del teclado con sus caracteres ]
{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'},
};
//	>> PINES TECLADO
byte pinFilas[FILAS] = {7,6,5,4};		// [ pines contectados para las filas ]
byte pinColumnas[COLUMNAS] = {3,2,1,0}; // [ pines contectados para las columnas ]
//	>> TECLADO
Keypad teclado(makeKeymap(teclas), pinFilas, pinColumnas,FILAS, COLUMNAS); // [ mapeo de teclado ]

//	------------------------------------
//	----[CONFIGURACION PANTALLA LCD]----
//	------------------------------------
//	>> PANTALLA LCD
LiquidCrystal pantallaLCD(8, 13, A0, A1, A2, A3); // [ pines de pantalla LCD ]

//	-------------------------------
//	----[CONFIGURACION BOTONES]----
//	-------------------------------
//	>> BOTON CONFIGURACION
int valorBotonConfiguracionAntes = 0;
//	>> BOTON RESET
int valorBotonResetAntes = 0;
//	>> BOTON TEST PASSWORD
int valorBotonTestPasswordAntes = 0;
//	>> ESTADO CONFIGURACION CONTRASEÑA CORRECTA
int password_Correcto = 0;
int banderaBotonConfiguracion = 0;

//	------------------------------------
//	----[CONFIGURACION PARPADEO LED]----
//	------------------------------------
int password_estado = 0;
unsigned long tiempoInicioParpadeo = 0;
unsigned long tiempo = 0;
int bandera_inicioParpadeo = 0;

int estado_mensaje = 0;

int estadoLed = LOW;
unsigned long tiempoAnterior = 0;
int parpadeoLed = 300;
int empezar_parpadeo = 0;
int bandera_apagarLed = 0;
int pinLed;

/*
================================================================================================
> SETUP
================================================================================================
*/
void setup()
{
  //Inicio Botones
  pinMode(BOTON_CONFIGURACION, INPUT);
  pinMode(BOTON_RESET,INPUT);
  pinMode(BOTON_TEST_PASSWORD,INPUT);
  
  //Inicio Leds
  pinMode(LED_OK,OUTPUT);
  pinMode(LED_FAIL,OUTPUT);
  
  //Inicio de pantalla LCD
  pantallaLCD.begin(16,2);
  
  mostrar_mensaje();
}

/*
================================================================================================
> LOOP
================================================================================================
*/
void loop()
{
  secuencia_millis();			// [ secuenciador para parpadeo y borrado de pantalla ]
  
  if(password_Correcto == 0)	// [ Si todavia no se hizo nada (estado inicial del programa) ]
  {
    password_pedido(contrasenia,contraseniaPorDefecto);	//[ Deja ingresar la password ]
  }
  
  boton_reset();			//[BOTON RESET PANTALLA]
  
  if(indice != 0)			// [ Condicion para que haya algun caracter al cual comparar con la contraseña o borrar ]
  {
    boton_testPassword();	//[BOTON PARA COMPROBAR CONTRASEÑA]
  }
  
  if(password_Correcto == 2)	// [ Si todavia no se comprobo el password correcto no puedo usar el boton configuracion ]
  {
    boton_configuracion();		//[BOTON CONFIGURACION PASSWORD]
  }

  secuencia_parpadeo_led();
   
  delay(10); 
}
/*
================================================================================================
> FIN DEL PROGRAMA
================================================================================================
*/



/*
================================================================================================
//  > FUNCIONES
================================================================================================
*/

/*
-------------------------------
----[FUNCIONES DE PASSWORD]----
-------------------------------
*/

// INGRESO DE PASSWORD
/// @brief Permite ingresar caracteres, mostrarlos por pantalla y modificar el estado del password
///	Si el indice llega a 6 se comprueba automaticamente el password
void password_pedido(char* contrasenia, char* contraseniaPorDefecto)
{  
  password_ingresoCaracteres();	// [ funcion de ingreso teclas ]

  if(indice == LARGO_PASSWORD - 1 && banderaBotonConfiguracion == 0)	// [ si el indice llega al limite de caracteres y no estamos usando el boton configuracion]	
  {	
    password_comprobar(contrasenia,contraseniaPorDefecto);	// [ Llamo a la funcion comprobar_Password ]
    indice = 0;												// [ ponemos el indice en 0 para proximo ingreso ]						
  }
}
//-----------------------------------------------

//-----------------------------------------------
//COMPROBACION PASSWORD
/// @brief Verifica el password ingresado de manera manual (apretando boton test) o automatica (indice == 6)
void password_comprobar(char* contrasenia, char* contraseniaPorDefecto)
{
  if(strcmp(contrasenia, contraseniaPorDefecto) == 0)	// [ si la comparacion de la cadena es igual la password es correcta ]	
  {
    password_Correcto = 1;		// [ se actualiza el primer ingreso para usar boton configuracion ]
    estado_mensaje = 2; 
  }
  else							// [ sino, es incorrecta ]
  {
    password_Correcto = -1;		// [ no se deja usar el teclado mientras muestre el mensaje ]
    estado_mensaje = 3; 		
  }
  
  bandera_inicioParpadeo = 1;
  mostrar_mensaje();
  reiniciar_password(); 
}
//-----------------------------------------------

//-----------------------------------------------
//INGRESO DE CONTRASEÑA NUEVA
/// @brief Permite ingresar una password nueva
void password_nuevo(char* contrasenia, char* contraseniaPorDefecto)
{
  estado_mensaje = 1;

  mostrar_mensaje();			
  
  while(indice < LARGO_PASSWORD - 1)
  {
    caracterIngresado = teclado.getKey(); 				
  																						
  	if(caracterIngresado)								
  	{													
      contraseniaPorDefecto[indice] = caracterIngresado;		
      pantallaLCD.print(caracterIngresado);				
      indice++;											
  	}
    
    banderaBotonConfiguracion = 1;
    boton_reset();
  }

  if(indice == 6)
  {
    estado_mensaje = 4;
    mostrar_mensaje(); 
  	password_Correcto = 1;
    bandera_inicioParpadeo = 1;
  }
  
  banderaBotonConfiguracion = 0;
  indice = 0;
}
//-----------------------------------------------

//-----------------------------------------------
//IINGRESO CARACTER
/// @brief Muestra por pantalla los caracteres ingresados y guarda el password para compararlo
void password_ingresoCaracteres(void)
{
  caracterIngresado = teclado.getKey();	// [ obtiene el caracter apretado del teclado ]			
  
  if(caracterIngresado)	// [ si ingreso un caracter ]					
  {
    contrasenia[indice] = caracterIngresado;	// [ guardo ese caracter en la contrasenia ]
    pantallaLCD.print(caracterIngresado);		// [ muestro ese caracter ingresado en el LCD ]
    indice++;									// [ incremento el indice de la cadena por cada caracter apretado]
  }
}
//-----------------------------------------------

//-----------------------------------------------
//INGRESO DE CONTRASEÑA NUEVA
/// @brief Reinicia la contraseña
void reiniciar_password(void)
{
  strcpy(contrasenia, "xxxxxx");  // [ copia en cada posicion de la cadena una x ]
}
//-----------------------------------------------

/*
-------------------------------
----[FUNCIONES DE BOTONES]----
-------------------------------
*/

//-----------------------------------------------
//BOTON TEST PASSWORD
/// @brief Permite testear el password sin llegar a los 6 caracteres ingresados
void boton_testPassword()
{
  int valorBotonTestPasswordAhora;
  
  valorBotonTestPasswordAhora = digitalRead(BOTON_TEST_PASSWORD);
  
  if(valorBotonTestPasswordAhora == 1 && valorBotonTestPasswordAntes == 0)
  {
    password_comprobar(contrasenia, contraseniaPorDefecto);	//	[ Llama a la funcion de comprobar ]
  }
  
  valorBotonTestPasswordAntes = valorBotonTestPasswordAhora;
}
//-----------------------------------------------

//-----------------------------------------------
//BOTON CONFIGURACION
/// @brief Permite ingresar al estado cambiar password
void boton_configuracion()
{
  int valorBotonConfiguracionAhora;
  
  valorBotonConfiguracionAhora = digitalRead(BOTON_CONFIGURACION);
  
  if(valorBotonConfiguracionAhora == 1 && valorBotonConfiguracionAntes == 0)
  {
    apagar_led();
    password_nuevo(contrasenia,contraseniaPorDefecto); // [ Llama a la funcion de nuevo password ]
  }
  
  valorBotonConfiguracionAntes = valorBotonConfiguracionAhora;
}
//-----------------------------------------------

//-----------------------------------------------
//BOTON RESET
/// @brief Permite borrar los caracteres ingresados
void boton_reset(void)
{
  int valorBotonResetAhora;
  
  valorBotonResetAhora = digitalRead(BOTON_RESET);
  
  if(valorBotonResetAhora == 1 && valorBotonResetAntes == 0)
  {
    //mostrarMensaje();		// [ Llama a la funcion que muestra el mensaje correcpondiente ]
    bandera_inicioParpadeo = 0;
    bandera_apagarLed = 0;
    estado_mensaje = 0;
    if(banderaBotonConfiguracion == 1)
    {
     	estado_mensaje = 1;
    }
    mostrar_mensaje();
    tiempoInicioParpadeo = 0;
    indice = 0;				// [ el indice de la password vuelve a 0 ]
    password_Correcto = 0;	// [ el estado vuelve a 0 ]
    reiniciar_password();
  }
 
  valorBotonResetAntes = valorBotonResetAhora;
}
//-----------------------------------------------

/*
-------------------------------
------[FUNCIONES DE LEDS]------
-------------------------------
*/

//-----------------------------------------------
//FUNCION SECUENCIA PARPADEO
/// @brief genera el parpadeo del led y el display cada X tiempo
void secuencia_parpadeo_led(void)
{
  tiempo = millis();
  
  switch(password_Correcto)  // [ Comprueba el estado de la contraseña para prender el led ]
  {
    case 1:
    pinLed = LED_OK;
    break;
    case -1:
    pinLed = LED_FAIL;
    break;
  }

  if(tiempo >= (tiempoAnterior + parpadeoLed) && bandera_apagarLed == 1)  // [ Compara el tiempo del parpadeo del led y el display ]
  {
    digitalWrite(pinLed,estadoLed);
  	estadoLed = !estadoLed;
  	tiempoAnterior = tiempo;
    
    if(estadoLed)   // [ Verifica el estado del led, para prender y apagar el display ]
  	{
      pantallaLCD.noDisplay();
    }
    else
    {
      pantallaLCD.display();
    }
  } 
   
  if(bandera_apagarLed == 0) 
  {
    apagar_led();
  }
}
//-----------------------------------------------

//-----------------------------------------------
//FUNCION APAGAR LEDS
/// @brief genera el parpadeo del led y el display cada X tiempo
void apagar_led(void)
{
    digitalWrite(pinLed,LOW);
    pantallaLCD.display();
}

/*
-------------------------------
------[FUNCIONES MILLIS]-------
-------------------------------
*/

//-----------------------------------------------
/// @brief captura el tiempo inicial del parpadeo, y lo detiene cuando pasen los 5 segundos
void secuencia_millis(void)
{
  tiempo = millis(); 

  if((password_Correcto == 1 || password_Correcto == -1) && bandera_inicioParpadeo == 1)  // [ Si se corrabora el password por correcto o incorrecto ]
  {
    tiempoInicioParpadeo = tiempo;  // [ Capturo el tiempo en el que inicia el parpadeo ]

    if(password_Correcto == 1)
    {
      password_Correcto = 2;
    }

    bandera_inicioParpadeo = 0;
    bandera_apagarLed = 1;
  }
  
  if(tiempoInicioParpadeo != 0 && tiempo >= (tiempoInicioParpadeo + TIEMPO_LIMITE_PARPADEO)) // [ Si supero el tiempo desde el inicio del parpadeo y el limite de 5000 mseg]
  {
    estado_mensaje = 0;
    mostrar_mensaje();
    tiempoInicioParpadeo = 0;
    password_Correcto = 0;
    bandera_apagarLed = 0;
  }
}
//-----------------------------------------------

/*
-------------------------------
-----[FUNCIONES MENSAJES]------
-------------------------------
*/

//-----------------------------------------------
/// @brief muestra los mensajes en el display segun el estado de la contraseña
void mostrar_mensaje (void)
{
  pantallaLCD.clear();
  pantallaLCD.setCursor(0,0);	

  switch(estado_mensaje)
  {
    case 1: 
        pantallaLCD.print("Password new:");
        pantallaLCD.setCursor(0,1);	
        break;
    case 2: 
        pantallaLCD.print("Password ok");
        break;
    case 3: 
        pantallaLCD.print("Password fail");
        break;
    case 4: 
        pantallaLCD.print("Password changed");
        break;
    default: 
        pantallaLCD.print("Password:");
        break;
  }
  
  indice = 0;
}
//-----------------------------------------------
