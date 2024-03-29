//  =========
//  > DEFINES
//  =========
#define ZERO_TO_HERO 0      // Inicio contador
#define MAX_SECONDS 1023    // Fin contador
#define BUTTON_START 2      // Boton start
#define BUTTON_RESET 3      // Boton reinicio
#define FIRST_LED 4         // Primer pin de leds
#define LAST_LED 13         // Ultimo pin de leds
#define TAM 10              // Tamaño vector binario y vector leds
#define LED_PRENDIDO 1      // Led prendido
#define LED_APAGADO 0       // Led apagado

//  ===========
//  > VARIABLES
//  ===========
int arrayEnteros[TAM];
int arrayLeds[TAM];
int contador = ZERO_TO_HERO;

//  > VARIABLES MILLIS
unsigned long tiempo = 0;   // Millis
unsigned long tiempo1 = 0;  // Millis Auxiliar
int diferencia1 = 1000;     // Diferencia millis

//  > VARIABLES BOTONES
int valorBotonPausaAntes = 0;
int valorBotonResetAntes = 0;
int flag = 0;

//  =======
//  > SETUP
//  =======
void setup()
{
    Serial.begin(9600);
    Recorrer_Pins();                                      // Funcion que recorre los pins
    Inicializar_Vector(arrayEnteros, TAM);                // Funcion que inicializa vector binario
    Inicializar_Vector_Leds (arrayLeds, TAM, FIRST_LED);  // Funcion que inicializa vector de leds
	imprimirHomero();
}


//  ======
//  > LOOP
//  ======
void loop()
{   
    Pausar_Boton (BUTTON_START);    // Funcion boton pausa    
    Resetear_Boton (BUTTON_RESET);  // Funcion boton restart
    secuencia_millis();	
       
    delay(10);      
}
//  ==================
//  > FIN DEL PROGRAMA
//  ==================



//  ===========
//  > FUNCIONES
//  ===========

/// @brief Funcion que realiza la secuencia del programa
void secuencia_millis()
{
  tiempo = millis();  // Declaracion millis
  	if(tiempo >= (tiempo1 + diferencia1) && flag == 1 ) // Condicional millis
    {
        if(contador <= MAX_SECONDS) //Contador
        {           
            Transformar_Decimal_A_Binario_Recursivo(contador, arrayEnteros, TAM);               // Funcion que permite mediante recursividad transformar un decimal a un binario
            Prender_Led_Segun_Binario (arrayEnteros, TAM, arrayLeds, LED_PRENDIDO, LED_APAGADO);// Funcion que toma valor del array de binario y enciende los leds
            
         	//sistemclear();
            //Serial.println("|  DECIMAL   |   BINARIO	 |");
          	Serial.print ("| SEGUNDO: ");
          	Serial.print((String)"[" + contador + "] --> ");
            //Serial.print(contador);             // Imprimir numero decimal
            //Serial.print(" | ");
            //Serial.print("Binario: ");
            Serial.print ("[");          
            Mostrar_Vector (arrayEnteros, TAM); // Imprimir numero binario
            Serial.println ("]");
            contador++;     // Actualizar contador      	
        }
      	tiempo1 = tiempo;   // Actualizacion tiempo1 de millis
    }
}

/// @brief Funcion para recorrer los pins y habilitarlos segun entrada o salida
void Recorrer_Pins()
{
    for(int i = FIRST_LED; i <= LAST_LED; i++)
    {
        pinMode(i, OUTPUT);
    }
    pinMode(BUTTON_START, INPUT);
    pinMode(BUTTON_RESET, INPUT);
}
//--------------------------------------------------------------------------------------------------------------//
/// @brief Funcion que inicializa un vector, colocando todos sus valores en 0
/// @param vector INT[] vector a inicializar
/// @param longitudVector INT tamaño vector a inicializar
void Inicializar_Vector(int vector[], int longitudVector)
{
  for (int i = 0; i < longitudVector ; i++)
  {
    vector[i] = 0;    
  }
}

/// @brief Funcion que inicializa el vector de leds, de acuerdo al pin donde estos se encuentren conectados
/// @param vector vector INT[] a inicializar
/// @param longitudVector INT tamaño vector a inicializar
/// @param desdePin INT inicio del pin a inicializar
void Inicializar_Vector_Leds (int vector[], int longitudVector, int desdePin)
{
  for (int i = 0; i <= longitudVector - 1 ; i++)
  {
    vector[i] = desdePin;
    
    desdePin++;
  }
}
//--------------------------------------------------------------------------------//

/// @brief Funcion que mediante un boton pausa el cronometro binario, 
///        mediante una bandera que cambia de valor.
/// @param pinStart INT pin de conexion del boton de pausa
void Pausar_Boton (int pinStart)
{    
    int valorBotonPausaAhora = digitalRead(pinStart);
  	if(valorBotonPausaAhora == 1 && valorBotonPausaAntes == 0)
    {          
        if(flag == 0)
        {
          flag = 1;  
        }
        else
        {
          flag = 0;
        }                   
  	}  
  	valorBotonPausaAntes = valorBotonPausaAhora;
}
//--------------------------------------------------------------------------------//

/// @brief funcion que mediante un boton resetea el cronometro binario, 
///        haciendo que el contador vuelva a 0.
/// @param pinReset INT pin de conexion del boton de reset
void Resetear_Boton (int pinReset)
{
    int valorBotonResetAhora = digitalRead(pinReset);
  	if(valorBotonResetAhora == 1 && valorBotonResetAntes == 0)
    {       
      contador = ZERO_TO_HERO;  
      for (int i = 0; i < TAM; i++)
      {
        arrayEnteros[i] = 0;
      }        
    }  	
  	valorBotonResetAntes = valorBotonResetAhora;   
}
//--------------------------------------------------------------------------------//

/// @brief funcion que permite mediante recursividad transformar un decimal a un binario
/// @param decimal INT numero a transofmar en binario
/// @param vector INT[] tamaño del vector binario para guardar los numeros (0 o 1)
/// @param longitud INT tamaño del vector binario
void Transformar_Decimal_A_Binario_Recursivo(int decimal, int vector[], int longitud)				
{
  int resto;
  int retorno;
  
  resto = decimal % 2;
  vector[longitud - 1] = resto;
  
  if(decimal < 2)								
  {
    resto = decimal;																
  }
  else
  {
    decimal = decimal / 2;
    longitud --;
    Transformar_Decimal_A_Binario_Recursivo (decimal, vector, longitud);												
  }								
}
//--------------------------------------------------------------------------------//

/// @brief Funcion que permite encender los leds de acuerdo al numero binario (0 o 1)
/// @param vectorUno INT[]vector binario
/// @param longitudVectorUno INT tamaño del vector binario
/// @param vectorDos INT[] vector correspondiente a la salida de los pines
/// @param ledPrendido INT HIGH o 1
/// @param ledApagado INT LOW o 0
void Prender_Led_Segun_Binario (int vectorUno[], int longitudVectorUno, int vectorDos[], int ledPrendido, int ledApagado)
{
  int i;
  int estadoLed;
  int posicionVectorDos;
  
  posicionVectorDos = vectorDos[0];
  
	for (i = longitudVectorUno - 1; i >= 0; i--)
	{
  		estadoLed = vectorUno[i];
    	
    	if(estadoLed == 1)
  		{
        digitalWrite(vectorDos[posicionVectorDos - 4], ledPrendido);
  		}
      if(estadoLed == 0)
      {
        digitalWrite(vectorDos[posicionVectorDos - 4], ledApagado);
      }      
      posicionVectorDos++;
	}
}
//--------------------------------------------------------------------------------//

/// @brief Funcion que permite mostrar todas las variables dentro de un vector. 
///        La utilizamos para mostrar el vector de binarios.
/// @param vector INT[] vector a imprimir
/// @param longitudVector INT tamaño del vector a imprimir
void Mostrar_Vector (int vector[], int longitudVector)
{
  int i;
  for (i = 0; i < longitudVector; i++)
  {
    Serial.print(vector[i]);
  }
  
  //Serial.println();
}
//--------------------------------------------------------------------------------//




//IMPRIMIR HOMERO
void imprimirHomero()
{
  Serial.println(" ");
  Serial.println("      ,---.");
  Serial.println("    ,.'-.   \"");
  Serial.println("  ( ( ,''´´´´''-.");
  Serial.println("  `,X          `");
  Serial.println("  /` `           `._ ");
  Serial.println(" (            ,   ,_\"");
  Serial.println(" |          ,---.,'o `.");
  Serial.println(" |         / o   \     )");
  Serial.println("  \ ,.    (      .____,");
  Serial.println("   \| \    \____,'    \ ");
  Serial.println(" '`'\  \        _,____,'");
  Serial.println(" \  ,--      ,-'     \ ");
  Serial.println("   ( C     ,'         \ ");
  Serial.println("    `--'  .'           |");
  Serial.println("      |   |         .O |");  
  Serial.println("    __|    \        ,-'_"); 
  Serial.println("   / `L     `._  _,'  ' `. "); 
  Serial.println("  /    `--.._  `',.   _\  ` "); 
  Serial.println("  `-.       /\  | `. ( ,\  \""); 
  Serial.println(" _/  `-._  /  \ |--´  (     \"");
  Serial.println("'  `-.   `'    \/\`.   `.    )"); 
  Serial.println("      \           \ `.  |    |"); 
}

/*void sistemclear()
{
  Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}*/

