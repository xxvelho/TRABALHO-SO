# Programando com Múltiplas Threads
##### Antes de tudo verifique se está com a biblioteca do mpfr no seu Ubuntu com o seguinte codigo:
* "dpkg -l | grep libmpfr" 
##### Se a biblioteca MPFR estiver instalada, você verá uma linha que contém "libmpfr" e informações sobre a versão instalada, então você pode seguir para a seção de "como compliar". Senão instale usando o codigo abaixo:
* "sudo apt update"
* "sudo apt install libmpfr-dev"
##### O processo de instalação começará e você será perguntado se deseja continuar. Digite 'Y' e pressione 'Enter' para confirmar. Aguarde até que o Ubuntu termine de baixar e instalar o pacote MPFR e suas dependências. Após isso basta compilar.
# Como compilar
* "gcc main.c -o servidor -lmpfr -lgmp -lpthread"
* "./servidor <quantidade_requisicoes>"

# Bug encontrado
Em alguns casos a saida do resultado de PI acaba saindo como 'nan' (Not a Number), um problema dificil de ser tradado, pois existe varios motivos para ocorrer esse tipo de problema, como:

* Divisão por zero: Quando você tenta dividir um número por zero, o resultado é indefinido e é representado como "NaN".

* Operações com infinito: Algumas operações com infinito podem resultar em "NaN". Por exemplo, a subtração de infinito por infinito ou a multiplicação de infinito por zero resultará em "NaN".

* Operações inválidas: Certas operações matemáticas podem produzir resultados indefinidos, como a raiz quadrada de um número negativo ou o logaritmo de zero. Nessas situações, o resultado também será "NaN".

* Valores de entrada inválidos: Se você estiver passando argumentos inválidos para uma função, como valores que não podem ser interpretados corretamente, a função pode retornar "NaN" como resultado.

## Dificuldades encotradatas no trabalho
A maior dificuldade, com certeza, foi utilizando a biblioteca pthread, pois eu não tinha muito conhecimento sobre ela, e passei dias tentando resolver o problema de escalonamento e sincronismo. As threads sempre executavam todas as requisições ao invés de dividir o trabalho entre elas. Comentei esse problema com um amigo, e ele me disse que utilizou uma fila para controlar melhor as requisições. Então, resolvi fazer do meu jeito, usando a ideia da fila, e consegui solucionar o problema.
