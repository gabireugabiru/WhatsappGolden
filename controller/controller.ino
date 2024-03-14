#include <Wire.h>
#include <LiquidCrystal.h>

enum Status {
  Missing = 0,
  Idle = 1,
  Pending = 2
};

enum Message {
 Request = 0,
 Sending = 1 
};

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int CHECK_TIME = 100;
const int MESSAGE_LEN = 16;
const int DISPLAY_COLUMNS = 16;
const int DISPLAY_ROWS = 2;
const int COLUMNS_FOR_STATUS = 4;
const int TOTAL_IN_SCREEN =  (int)(DISPLAY_COLUMNS/COLUMNS_FOR_STATUS) * DISPLAY_ROWS;
const int MAX_PERIPHERALS = 99;
const int PAGE_DOWN_PIN = 6;
const int PAGE_UP_PIN = 7;
const int TIME_BETWEEN_PUSHES = 100;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int peripherals = 0; 
int last_occourance = CHECK_TIME * 2;
int starting_id = 1;
int ending_id = TOTAL_IN_SCREEN;
int page = 1;
char message[MESSAGE_LEN] = {0};
long int last_page_up = 0; 
long int last_page_down = 0; 

int input_page_up() {
  int pin_value = !digitalRead(PAGE_UP_PIN);
  if (pin_value) {
    last_page_up = millis();
  }
  return pin_value;
}
int input_page_down() {
  int pin_value = !digitalRead(PAGE_DOWN_PIN);
  if (pin_value) {
    last_page_down = millis();
  }
  return pin_value;
}

void setup() {
  Serial.begin(9600);
  // lower the timeout because for some reason it is waiting for it
  // and 1 second is too much time (especially in this speed)
  Serial.setTimeout(100);

  pinMode(PAGE_UP_PIN, INPUT_PULLUP);
  pinMode(PAGE_DOWN_PIN, INPUT_PULLUP);
  lcd.begin(DISPLAY_COLUMNS, DISPLAY_ROWS);
  Wire.begin();
  Serial.println("o numero de arduinos (excluindo esse)");
  while (peripherals == 0) {
    fill_peripherals();
  }
}
int fill_peripherals() {
  int has_filled = 0;
  while (Serial.available()) {
    int integer = Serial.parseInt();
    if (integer > 0) {
      if (integer > MAX_PERIPHERALS) {
        Serial.print(integer);
        Serial.println(" e maior que o numero maximo de arduinos");
      } else {
        has_filled = 1;
        peripherals = integer;
      }
    }
  }
  return has_filled;
}

void clearMessage() {
  for (int i = 0; i < MESSAGE_LEN; i++) {
    message[i] = 0;
  }
}

void broadcast(int id) {
  for (int i = 1; i - 1 < peripherals; i++) {
  	Wire.beginTransmission(i);
    Wire.write(Sending);
    Wire.write(id);
    for (int x = 0; x < MESSAGE_LEN; x++) {
      Wire.write(message[x]);
    }
	Wire.endTransmission();
  }
  clearMessage();
}

void printStatus(int peripheral_id, int status) {
  //maybe this bit should go back (not wasting stack)
  if (peripheral_id < starting_id || peripheral_id > ending_id) {
  	return;
  }
  
  int i = peripheral_id - 1;
  //
  int x = i * COLUMNS_FOR_STATUS;
  int row = (int)(x/DISPLAY_COLUMNS);
  lcd.setCursor(x%DISPLAY_COLUMNS, row%DISPLAY_ROWS);
  
  //print with left pad
  if (i < 9) {
    lcd.print("0");
  }
  lcd.print(peripheral_id);
  
  if (status == Missing) {
  	lcd.print("?");
  } else {
    lcd.print("O");
  }
}
void update_id_range() {
  //RANGE OF IDS IN SCREEN
  starting_id = ((page-1)*TOTAL_IN_SCREEN) + 1;
  ending_id = minn(page*TOTAL_IN_SCREEN, peripherals);
}

int minn(int x, int y) {
  if (x > y) {
    return y;
  } else {
    return x;
  }
}

void loop() {
  int _millis = millis();
  int elapsed = _millis - last_occourance;
 
  if (elapsed >= CHECK_TIME) {
	for (int i = 1; i - 1 < peripherals; i++) {
      // Status do periferico começa faltante
      int status = Missing;
      // Faz a requisição do status do periferico
      Wire.requestFrom(i, 1);
      while (Wire.available()) {
        status = Wire.read();
        printStatus(i, status);
      }
      // Se nao teve resposta mostra que nao foi encontrado
      if (status == Missing) {
        printStatus(i, status);
      }
      
      // se tiver uma mensagem pendente requisita a mensagem e passa para todos os arduino
      if (status == Pending) {
 		// faz uma requisição da mensaem
        Wire.beginTransmission(i);
		Wire.write(Request);
		Wire.endTransmission();
        
        // resposta com a mensagem
        Wire.requestFrom(i, MESSAGE_LEN);
        int x = 0;
        while(Wire.available()){
          char value = Wire.read();
          if(x<MESSAGE_LEN) {
            message[x] = value;
            x++;
          }
        }
        // transmissao para todos os arduinos
        broadcast(i);
      }
    }
    last_occourance = _millis;
  }
  // logica de page_down se a pagina for 1 vai para a ultima pagina
  if (millis() > (last_page_down + 500) && input_page_down()) {
    if (page == 1) {
      page = (int)(peripherals/TOTAL_IN_SCREEN)+!!(peripherals%TOTAL_IN_SCREEN);
    } else {
    	page -= 1;
    }
    update_id_range();
    lcd.clear();
  }
  // logica de page_up se a pagina for a ultima vai para a primeira pagina
  if (millis() > (last_page_up + 500) && input_page_up()) {
    if (page >= (int)(peripherals/TOTAL_IN_SCREEN)+!!(peripherals%TOTAL_IN_SCREEN)) {
      page = 1;
    } else { 
      page += 1;
    }
    update_id_range();
    lcd.clear();
  }
  
  // atualiza o numero de arduinos de acordo com o serial
  if (Serial.available()) {
    int has_filled = fill_peripherals();
    if (has_filled) {
      Serial.print("O numero de arduinos agora e ");
      Serial.println(peripherals);
      page = 1;
      update_id_range();
    }
  }
}