compgraf_t2
===========

Trabalho da disciplina de Computação Gráfica
===========

Descrição:
O segundo trabalho prático da disciplina de Computação Gráfica I, que deverá ser feito em dupla ou individualmente, consiste em desenvolver um protótipo em OpenGL para realizar a animação de objetos 3D usando scripts.

A primeira etapa do trabalho consiste em ler o script e tratá-lo, preparando a estrutura de dados para receber as transformações (translação, rotação e escala) de cada objeto a cada frame. Após, a leitura dos arquivos .obj deverá ser realizada. Com estas informações, será possível começar o processo de interpolação (linear ou outra) para encontrar as transformações de cada objeto a cada frame. Abaixo, apresento a definição da sintaxe do script, bem como um exemplo.
#actors <n>
#id_actor <i> cube.obj
Frame=<f> trans=<x,y,z> scale=<x,y,z> rot=<x,y,z>
Frame=<f> trans=<x,y,z> scale=<x,y,z> rot=<x,y,z>
Frame=<f> trans=<x,y,z> scale=<x,y,z> rot=<x,y,z>
[Interpolation=linear]
…
#end
Note que se tu implementares somente um tipo de interpolação, não é necessário especificar o tipo de interpolação no script (por isso está entre [,]). Além disso, se uma transformação faltar na especificação (por exemplo trans não for definido em algum frame), não deve ser aplicada nenhuma translação, ou seja o objeto fica onde estava. Abaixo um exemplo do script.
#actors 2
#id_actor 1 cube.obj
Frame=1 trans=<0,10,0> scale=<1,1,1> rot=<0,0,0>
Frame=10 trans=<10,10,10>
Frame=<20> rot=<0,90,0>
#id_actor 2 yoda.obj
Frame=3 trans=<20,10,0> scale=<1,1,1> rot=<0,0,90>
Frame=7 trans=<30,10,0> scale=<0.5,0.5,0.5>
Frame=<20> rot=<0,90,0>
#end
 
Por fim, a visualização da animação deverá permitir os seguintes comandos: play (tocar até o final), play-looping (tocar até o final e iniciar de novo), rewind (voltar um frame), pause (pausar a animação), forward (avançar um frame).
