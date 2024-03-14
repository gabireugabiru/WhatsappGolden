#include <Wire.h>

enum Status
{
  Missing = 0,
  Idle = 1,
  Pending = 2
};

enum Message
{
  Request = 0,
  Sending = 1
};
enum State
{
  Writing = 0,
  Editing = 1,
  Waiting = 2
};

const int SENDING_LED_PIN = 6;
const int WAITING_LED_PIN = 7;
const int MAX_ID = 99;
const int TIMEOUT = 110;
int id = 0;
int state = Writing;
int is_sending = 0;
int time_started_waiting = 0;
char message[16] = {0};

int fill_id()
{
  int has_filled = 0;
  while (Serial.available())
  {
    int integer = Serial.parseInt();
    if (integer != 0)
    {
      if (integer > MAX_ID)
      {
        Serial.print(integer);
        Serial.println(" e maior que o numero maximo de arduinos");
      }
      else
      {
        has_filled = 1;
        id = integer;
      }
    }
  }
  return has_filled;
}

void clear_message()
{
  for (int i = 0; i < 16; i++)
  {
    message[i] = 0;
  }
}
int has_message()
{
  return message[0] != 0;
}
void set_sending(int new_value)
{
  is_sending = new_value;
  digitalWrite(SENDING_LED_PIN, new_value);
}
void start_wire()
{
  Wire.begin(id);
  Wire.onRequest(request);
  Wire.onReceive(receiveEvent);
}
void set_state(int new_state)
{
  if (state == Editing && new_state != state)
  {
    Serial.println("Saindo do modo edicao");
  }
  if (new_state == Writing)
  {
    digitalWrite(WAITING_LED_PIN, 0);
  }
  if (new_state == Waiting)
  {
    digitalWrite(WAITING_LED_PIN, 1);
    time_started_waiting = millis();
  }
  if (new_state == Editing)
  {
    Serial.println("Entrando no modo edicao");
  }
  state = new_state;
}
void notify_id_change()
{
  Serial.print("Id do dispositivo agora e ");
  Serial.println(id);
}
void setup()
{
  Serial.begin(9600);

  Serial.setTimeout(100);

  pinMode(SENDING_LED_PIN, OUTPUT);
  pinMode(WAITING_LED_PIN, OUTPUT);
  pinMode(5, INPUT_PULLUP);
  Serial.println("Entre com o Id do dispositivo");
  while (id == 0)
  {
    fill_id();
  }
  notify_id_change();
  start_wire();
}

void loop()
{
  int new_state = state;
  if (state == Writing)
  {
    // Espera por uma mensagem no serial
    if (Serial.available())
    {
      // for some reason it is splitting the Serial input
      // really dont understand
      // waiting seems to fix the issue
      delay(100);
      // Pega a mensagem do serial limitando a 16 caracteres
      int i = 0;
      while (Serial.available())
      {
        char ch = Serial.read();
        if (i < 16)
        {
          message[i] = ch;
          i += 1;
        }
      }
      // se tiver uma mensagem no arduino entre em espera
      if (has_message())
      {
        new_state = Waiting;
        time_started_waiting = millis();
      }
    }
    // Entra no modo edição se tiver com a chave ligada
    int edit_mode = !digitalRead(5);
    if (edit_mode && new_state == Writing)
    {
      new_state = Editing;
    }
  }
  if (state == Editing)
  {
    // atualiza o id se tiver um id valido no serial
    if (Serial.available())
    {
      int has_changed = fill_id();
      if (has_changed)
      {
        notify_id_change();
        Wire.end();
        start_wire();
      }
    }

    int edit_mode = !digitalRead(5);
    // sai do modo edição baseado na chave de edição
    if (!edit_mode)
    {
      new_state = Writing;
    }
  }
  if (state == Waiting)
  {
    // timeout de espera e limpa a mensagem se ocorrer
    if (time_started_waiting != 0 && (time_started_waiting + TIMEOUT) <= millis())
    {
      Serial.println("timeouted");
      clear_message();
      new_state = Writing;
    }
  }

  if (state != new_state)
  {
    set_state(new_state);
  }
  delay(50);
}

void request()
{
  if (is_sending)
  {
    // se tiver enviando a mensagem envia a mensagem
    for (int i = 0; i < 16; i++)
    {
      Wire.write(message[i]);
    }
    // para o envio e volta ao estado inicial
    set_sending(0);
    set_state(Writing);
    clear_message();
  }
  else
  {
    // se tiver mensagem no arduino avisa o controlador que esta pending
    // se nao fala que ta idle
    if (!has_message())
    {
      Wire.write(Idle);
    }
    else
    {
      Wire.write(Pending);
    }
  }
}
void receiveEvent(int howMany)
{
  if (Wire.available())
  {
    int c_message = Wire.read();
    // Quando é uma requisição do controlador inicia a enviagem
    if (c_message == Request)
    {
      set_sending(1);
    }

    // Se for uma mensagem mostra a mensagem no serial
    if (c_message == Sending)
    {
      int from_id = Wire.read();
      if (id == from_id)
      {
        Serial.print("Voce: ");
      }
      else
      {
        Serial.print("Id ");
        Serial.print(from_id);
        Serial.print(": ");
      }
      while (Wire.available())
      {
        char ch = Wire.read();
        Serial.print(ch);
      }
      Serial.println(" ");
    }
  }
}