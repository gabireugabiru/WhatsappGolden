# Whatsapp Golden 🔆
Feito por Gabiru Gabireu, Gustavo Kwanzas, Pedro Pedra.
Este é um projeto microcontrolado onde existe uma comunicação entre um numero X de arduino, simulando a comunicação do famoso aplicativos de mensagem Whatsapp.
## Motivações para o projeto 🐣
- Atualmente o whatsapp tem muitas features que realmente não são necessárias como Encriptação end to end, Backups, suporte UTF-8, nomes, chamadas e até mesmo o salvamento de mensagens que vovê envia
- O whatsapp tem cada vez mais se tornado um aplicativo pesado, que nenhum arduino consegue aguentar e como Whatsapp Golden é feito com arquiteturas fracas em mente se torna ideal em comunicações internas que não podem vazar pela natureza local do Whatsapp Golden
- Com o sistema atualmente implementado no whatsapp é muito facil de pessoas maliciosas terem acesso ao seu numero, porém com Whatsapp Golden, esse processo se torna complicado, pois ele precisaria de uma conexão fisica com a rede de arduinos assim diminuindo a quantidade de scams

## Funcionamento 🔧
O circuito contempla 1 arduino de controle e X arduinos de perifericos:
- O arduino de controle, controla quantos arduinos estão na rede, verifica se os arduinos estão realmente conectados, recebe e transmite as mensagens no circuito.
- O arduino periferico aguarda entrada do usuario no serial e envia a mensagem digitada pelo usuario para o controlador.
 
### Setup Arduino de controle 🚬
Quando se liga o arduino é pedido o numero de arduinos que serao conectados ao sistema, esse numero pode ser trocado a qualquer instante apenas digitando o numero no serial

### Setup Arduino Periferico 🔪
Ao iniciar o arduino periferico é requistado um ID para o dispositivo este id deve ser um numero inteiro único dentro do numero de de arduinos selecionados no arduino de controle. Seu ID pode ser trocado a qualquer instante ao entrar no modo edição(chave presente no periferico) e entrando com o id desejado no serial. Mensagens enviadas antes da troca de ID não serão alteradas.

##  Display do Arduino de Controle 📺
O arduino de controle contém um display que mostra todos os arduinos que supostamente estam presentes no sistema, Arduinos que são confirmados no sistema possuem um "O" após seu ID, arduinos faltantes ou que estão inacessíveis são marcados com um "?". Se existem mais Arduinos do que cabem no display, existem dois botões que funcionam como um page up e page down, trocando o range de arduinos visiveis no display.
> ![alt text](https://github.com/gabireugabiru/WhatsappGolden/tree/master/assets/display.png)

## Leds do Arduino Periférico 💡
Existem dois leds nos arduinos perifericos, sendo o vermelho o led de espera, que significa que o arduino não está fazendo nada, apenas esperando uma requisição do Arduino de controle. O outro led é verde significando que o arduino está enviando uma mensagem para o arduino de controle.
> ![alt text](https://github.com/gabireugabiru/WhatsappGolden/tree/master/assets/LEDS.png)

:shipit: Isso foi feito por **Os Não Criadores Do Whatsapp** :shipit:
<sub><sup>
:shipit: **Este aviso definitivamente não foi copiado do thaylor** :shipit:
</sup></sub>