# Programando com Múltiplas Threads
##### Antes de tudo, verifique se você possui a biblioteca do MPFR instalada em seu sistema Ubuntu, utilizando o seguinte código:
* "dpkg -l | grep libmpfr" 
##### Se a biblioteca MPFR estiver instalada, você verá uma linha que contém "libmpfr" e informações sobre a versão instalada. Nesse caso, você pode prosseguir para a seção de "COMO COMPILAR". Caso contrário, instale-a usando o código abaixo:
* "sudo apt update"
* "sudo apt install libmpfr-dev"
##### O processo de instalação começará e você será perguntado se deseja continuar. Digite 'Y' e pressione 'Enter' para confirmar. Aguarde até que o Ubuntu termine de baixar e instalar o pacote MPFR e suas dependências. Após isso, basta compilar.
# Como compilar
* "gcc main.c -o servidor -lmpfr -lgmp -lpthread"
* "./servidor <quantidade_requisicoes>"

Obs: O codigo só mostra mensagem em caso de erro. Verifique os arquivos criados para conferir se está tudo certo. 
Caso queira fazer outro teste recomendo que apague os arquivos, pois a nova exercução não vai excluir os antigos.

# Bug encontrado
BUG 01 - Em alguns casos a saida do resultado de PI acaba saindo como 'nan' (Not a Number), um problema dificil de ser tradado, pois existe varios motivos para ocorrer esse tipo de problema, como:

* Divisão por zero: Quando você tenta dividir um número por zero, o resultado é indefinido e é representado como "NaN".

* Operações com infinito: Algumas operações com infinito podem resultar em "NaN". Por exemplo, a subtração de infinito por infinito ou a multiplicação de infinito por zero resultará em "NaN".

* Operações inválidas: Certas operações matemáticas podem produzir resultados indefinidos, como a raiz quadrada de um número negativo ou o logaritmo de zero. Nessas situações, o resultado também será "NaN".

* Valores de entrada inválidos: Se você estiver passando argumentos inválidos para uma função, como valores que não podem ser interpretados corretamente, a função pode retornar "NaN" como resultado.

BUG 02 - Caso apareça alguma mensagem como: 

* "../../src/mpfr-3.1.5/src/get_str.c:153  MPFR assertion failed: size_s1 >= m"

Reinicie: Apague todos os arquivos criados e reinicie o programa, ou apenas tente novamente. Lembrando que, caso você execute novamente, as requisições que não falharam, estarão juntas no arquivo de saída, pois a função adiciona no final do arquivo as requisições processadas mesmo se algumas não foram processadas.

## Dificuldades encotradatas no trabalho
A maior dificuldade, com certeza, foi utilizando a biblioteca pthread, pois eu não tinha muito conhecimento sobre ela, e passei dias tentando resolver o problema de escalonamento e sincronismo. As threads sempre executavam todas as requisições ao invés de dividir o trabalho entre elas. Comentei esse problema com um amigo, e ele me disse que utilizou uma fila para controlar melhor as requisições. Então, resolvi fazer do meu jeito, usando a ideia da fila, e consegui solucionar o problema.
