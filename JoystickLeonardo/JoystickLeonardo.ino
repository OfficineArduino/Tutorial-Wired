#define KeysNum 9  // Numero di tasti necessari per usare tutti 
                   // i comandi di tuxkart per ogni giocatore

//Soglia massima e minima per interpretare il joystick come input digitale
#define joy_leftThreshold 600
#define joy_rightThreshold 300
#define joy_upThreshold 600
#define joy_downThreshold 300

// serve per creare una lista, in pratica sostituisce i numeri con dei nomi
// molto figo, non strettamente necessario. Dopo frena non li ho piu' usati
enum playerPin { 
  sinistra, destra, accelera, frena, fuoco, nitro, curvaStretta, guardaDietro, riposiziona };

/* Player 1 controls */
// questo array contiene i pin da assengare ai controlli
// seguendo la convenzione in "enum playerPin" 
const int p1_pins[KeysNum] = {
  A0, A1, 0, 1, 2, 3, 4 };  //<-nota che posso prendere anche 0 e 1 perche' sulla leonardo
                            //la seriale hw e' non viene usata per comunicare col PC

// array che contiene i corripondenti tasti che la leonardo
// preme simulando di essere una tastiera
char p1_keyboardKeys[9] = {
  KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW, ' ', 'n', 'v', 'b', KEY_BACKSPACE };

/* Player 2 controls */
//valgono gli stessi commenti del player 1
const int p2_pins[KeysNum] = {
  A2, A3, 5, 6, 7, 8, 9 };

char p2_keyboardKeys[9] = {
  'a', 'd', 'w', 's', 'q', 'e', '1', '2', '3' };

void setup()
{
  //inizializzo i pin come INPUT
  inizializzaPlayer(p1_pins);
  inizializzaPlayer(p2_pins);

  Serial.begin(9600);
}

void loop()
{
  leggiController(p1_pins, p1_keyboardKeys);
  leggiController(p2_pins, p2_keyboardKeys);
}

// funzione per inizializzare i pin come INPUT.
// usando l'array per contenere i pin posso fare tutto con un ciclo for
// e posso usare la stessa funzione per piu' giocatori
void inizializzaPlayer (const int *pins) //<- se il puntatore confonde lo togliamo
{
  for(int i=0; i<KeysNum; i++)
    pinMode(pins[i], INPUT);
}

// questa funzione riceve come parametri:
// [*pins]    - i piedini assegnati al controller del giocatore X
// [*keylist] - l'elenco dei tasti dalla tastiera da emulare per contollare il gioco
//
// Scandisce in sequenza ogni input di arduino e se attivo associa il 
// corrispondente tasto della tastiera.
void leggiController (const int *pins, char *keyList)
{
  int direzione, throttle;
  
  // ciclo for per scandire tutti i tasti della tastiera
  for(int i=0; i<KeysNum; i++)
  {
    // sinistra e' un elemento della enum...vale 1
    if(i == sinistra){
      // leggo l'asse X del joystick
      direzione = analogRead( pins[0] );
      // se supero la soglia devo mandare il comando corrispondente
      if( direzione > joy_leftThreshold ) 
        Keyboard.press(keyList[sinistra]);
      else 
        // devo rilasciare anche il tasto della tastiera 
        // quando sul controller il rispettivo comando non viene premuto
        Keyboard.release(keyList[sinistra]);
    }

    else if(i == destra) {
      // ho già letto il pin dell'asse X prima, non c'e' bisogno di rifarlo
      if( direzione < joy_rightThreshold )
        Keyboard.press(keyList[destra]); 
      else
        Keyboard.release(keyList[destra]);
    }

    // uguale all'asse X ma per l'asse Y che controlla freno ed acceleratore
    else if(i == accelera) {
      throttle = analogRead( pins[1] );
      if( throttle > joy_upThreshold ) 
        Keyboard.press(keyList[frena]);
      else
        Keyboard.release(keyList[frena]);
    }

    else if(i == frena) {
      if( throttle < joy_downThreshold )
        Keyboard.press(keyList[accelera]);
      else
        Keyboard.release(keyList[accelera]);
    }

   // ho finito di leggere i piedini analogici
   //
   // posso passare a leggere quelli digitali che, non dovendo comparare
   // delle soglie posso leggere molto più facilmente e in sequenza.
   // Sfruttando la comodita' di aver usato un array
    else {
      if( digitalRead( pins[i] ) == HIGH )
        Keyboard.press(keyList[i]);
      else
        Keyboard.release(keyList[i]);
    }
  }
}








