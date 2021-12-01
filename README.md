PAV - P3: detección de pitch
============================

Esta práctica se distribuye a través del repositorio GitHub [Práctica 3](https://github.com/albino-pav/P3).
Siga las instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para realizar un `fork` de la
misma y distribuir copias locales (*clones*) del mismo a los distintos integrantes del grupo de prácticas.

Recuerde realizar el *pull request* al repositorio original una vez completada la práctica.

Ejercicios básicos
------------------

- Complete el código de los ficheros necesarios para realizar la detección de pitch usando el programa
  `get_pitch`.

   * Complete el cálculo de la autocorrelación e inserte a continuación el código correspondiente.
     ```c++
     
     void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {

    	for (unsigned int l = 0; l < r.size(); ++l) {
      	r[l]=0;
      		for (unsigned int n=l; n<x.size(); n++) {
          			r[l] += x[n-l]*x[n];
      		}
   	    }

    	if (r[0] == 0.0F) //to avoid log() and divide zero 
      		r[0] = 1e-10; 
     }

     ```

   * Inserte una gŕafica donde, en un *subplot*, se vea con claridad la señal temporal de un segmento de
     unos 30 ms de un fonema sonoro y su periodo de pitch; y, en otro *subplot*, se vea con claridad la
	 autocorrelación de la señal y la posición del primer máximo secundario.

	 NOTA: es más que probable que tenga que usar Python, Octave/MATLAB u otro programa semejante para
	 hacerlo. Se valorará la utilización de la librería matplotlib de Python.
	 
![Plots Python](https://github.com/ikergf/P3/blob/Garcia-Pons/IMG_P3/Gr%C3%A1ficas.PNG)

	**Tanto en la señal temporal como en la autocorrelación de la misma, se puede apreciar claramente que la distancia entre dos periodos o entre la posición del máximo principal y el secundario es de unos 4,16 ms, es decir, que la señal tiene un pitch de unos 420Hz.**

   * Determine el mejor candidato para el periodo de pitch localizando el primer máximo secundario de la
     autocorrelación. Inserte a continuación el código correspondiente.
     
     ```c++
     
     for(iR = r.begin() + npitch_min; iR < r.begin()+npitch_max; iR++){  
     	if(*iR > *iRMax){ 
				iRMax = iR; //Vamos iterando y, si el valor de iR es mayor a iRMax, entonces iRMax pasará a valer iR. De esta forma, encontraremos el primer máximo secundario de la autocorrelación 
	 	} 
     } 
     unsigned int lag = iRMax - r.begin(); //El lag será la distancia entre el valor máximo secundario (iRMax) y el máximo total de la autocorrelación (en r[0])
     
     ```

**Mediante el código mostrado se obtiene la distancia a la que se encuentra el primer máximo secundario de la autocorrelación, con lo que se obtiene un pitch de 240,96Hz.**

   * Implemente la regla de decisión sonoro o sordo e inserte el código correspondiente.
     ```c++
     
       bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm) const {
    		if(rmaxnorm > 0.35 && r1norm > 0.37 && pot > -40 && Z<3000){
      		return false;
    		}else{
      		return true;
    		}
  	   }
     
     ```
**Cualquier trama que cumpla todas las condiciones planteadas (se le ha añadido también la tasa de cruces por 0) será considerada como sonora y, por lo contrario, si hay alguna que no se supera se considerará sorda.**

- Una vez completados los puntos anteriores, dispondrá de una primera versión del detector de pitch. El 
  resto del trabajo consiste, básicamente, en obtener las mejores prestaciones posibles con él.

  * Utilice el programa `wavesurfer` para analizar las condiciones apropiadas para determinar si un
    segmento es sonoro o sordo. 
	
	  - Inserte una gráfica con la detección de pitch incorporada a `wavesurfer` y, junto a ella, los 
	    principales candidatos para determinar la sonoridad de la voz: el nivel de potencia de la señal
		(r[0]), la autocorrelación normalizada de uno (r1norm = r[1] / r[0]) y el valor de la
		autocorrelación en su máximo secundario (rmaxnorm = r[lag] / r[0]).

		Puede considerar, también, la conveniencia de usar la tasa de cruces por cero.

	    Recuerde configurar los paneles de datos para que el desplazamiento de ventana sea el adecuado, que
		en esta práctica es de 15 ms.
		
	![Wavesurfer](https://github.com/ikergf/P3/blob/Garcia-Pons/IMG_P3/Wavesurfer_2.PNG)
	
	**En orden descendente encontramos:
		+ Pitch
		+ Señal temporal
		+ Tasa de cruces por 0
		+ Potencia
		+ Rmaxnorm
		+ R1norm**
	**Hemos añadido la tasa de cruces por 0 porque creemos que también puede ser útil en la detección, ya que las tramas sordas tienden a tener una tasa más alta que las sonoras.**

      - Use el detector de pitch implementado en el programa `wavesurfer` en una señal de prueba y compare
	    su resultado con el obtenido por la mejor versión de su propio sistema.  Inserte una gráfica
		ilustrativa del resultado de ambos detectores.
		
	![Comparación](https://github.com/ikergf/P3/blob/Garcia-Pons/IMG_P3/Comparaci%C3%B3n_pitch.PNG)
	
	*Los puntos azules corresponden con nuestro detector y los naranjas al detector implementado en wavesurfer.*
	
	**Como se puede observar, los resultados obtenidos en la detección de pitch de nuestro programa y del detector de wavesurfer son muy similares, por lo que se puede decir que el código creado está bien implementado. Aún así, se podría destacar los 4 puntos azules que hay alrededor de la frecuencia de 500Hz, los cuales seguramente serán reconocidos como Gross pitch errors. Algunos de estos errores podrán ser corregidos con ayuda de algún tipo de pre o postprocesado.**
  
  * Optimice los parámetros de su sistema de detección de pitch e inserte una tabla con las tasas de error
    y el *score* TOTAL proporcionados por `pitch_evaluate` en la evaluación de la base de datos 
	`pitch_db/train`..
	
	![Evaluación](https://github.com/ikergf/P3/blob/Garcia-Pons/IMG_P3/Eva_final_sin_extras.PNG)
	
**Este es el resultado habiendo optimizado los parámetros, pero sin haber añadido ningún preprocesado ni postprocesado.**

   * Inserte una gráfica en la que se vea con claridad el resultado de su detector de pitch junto al del
     detector de Wavesurfer. Aunque puede usarse Wavesurfer para obtener la representación, se valorará
	 el uso de alternativas de mayor calidad (particularmente Python).
   

Ejercicios de ampliación
------------------------

- Usando la librería `docopt_cpp`, modifique el fichero `get_pitch.cpp` para incorporar los parámetros del
  detector a los argumentos de la línea de comandos.
  
  Esta técnica le resultará especialmente útil para optimizar los parámetros del detector. Recuerde que
  una parte importante de la evaluación recaerá en el resultado obtenido en la detección de pitch en la
  base de datos.

  * Inserte un *pantallazo* en el que se vea el mensaje de ayuda del programa y un ejemplo de utilización
    con los argumentos añadidos.

- Implemente las técnicas que considere oportunas para optimizar las prestaciones del sistema de detección
  de pitch.

  Entre las posibles mejoras, puede escoger una o más de las siguientes:

  * Técnicas de preprocesado: filtrado paso bajo, *center clipping*, etc.
  * Técnicas de postprocesado: filtro de mediana, *dynamic time warping*, etc.
  * Métodos alternativos a la autocorrelación: procesado cepstral, *average magnitude difference function*
    (AMDF), etc.
  * Optimización **demostrable** de los parámetros que gobiernan el detector, en concreto, de los que
    gobiernan la decisión sonoro/sordo.
  * Cualquier otra técnica que se le pueda ocurrir o encuentre en la literatura.

  Encontrará más información acerca de estas técnicas en las [Transparencias del Curso](https://atenea.upc.edu/pluginfile.php/2908770/mod_resource/content/3/2b_PS%20Techniques.pdf)
  y en [Spoken Language Processing](https://discovery.upc.edu/iii/encore/record/C__Rb1233593?lang=cat).
  También encontrará más información en los anexos del enunciado de esta práctica.

  Incluya, a continuación, una explicación de las técnicas incorporadas al detector. Se valorará la
  inclusión de gráficas, tablas, código o cualquier otra cosa que ayude a comprender el trabajo realizado.

  También se valorará la realización de un estudio de los parámetros involucrados. Por ejemplo, si se opta
  por implementar el filtro de mediana, se valorará el análisis de los resultados obtenidos en función de
  la longitud del filtro.
   
**De entre las técnicas ofrecidas, hemos decidido aplicar la de central-clipping para el preprocesado y el filtro de mediana para el postprocesado. Con la primera aumentamos los armónicos de orden elevado además de aumentar la robustez frente al ruido y con la segunda corregimos algunos de los errores groseros de pitch. Los códigos y resultados son los siguientes:**
* Central-Clipping:
```c++
for(unsigned int i=0; i<x.size(); i++){
  if(abs(x[i])<llindar){
      x[i]=0;
  } 
  else if (x[i]>=llindar){
      x[i]=x[i]-llindar;
  } 
  else {
      x[i]=x[i]+llindar;
  }
}
```
**Observamos su efecto en la siguiente imagen, donde la señal inferior es la original. Como vemos, el central-clipping ha eliminado la parte central que había entre el umbral de 0.008087158203125 elegido, de forma que nos elimina parte del ruido sin comprometer las tramas sonoras.**

![Central-clipping](https://github.com/ikergf/P3/blob/Garcia-Pons/IMG_P3/Central-clipping.PNG)

**Además, en las 2 imágenes siguientes se muestran los resultados de la evaluación de una señal arbitraria con o sin CC:**

![Sin_CC](https://github.com/ikergf/P3/blob/Garcia-Pons/IMG_P3/Pitch_sin_centerclipping.PNG)

*Evaluación sin CC*

![Con_CC](https://github.com/ikergf/P3/blob/Garcia-Pons/IMG_P3/Pitch_con_centerclipping300.PNG)

*Evaluación con CC*

**Cómo vemos, hay una gran robustez fentre a los sonidos sordos.**


* Filtro de mediana:
```c++
for (unsigned int i=0; i<f0.size(); i++){
  
    for (int j=-1; j<=1; j++){
      if(i>0 && i<(f0.size())){
        mediana[j+1]=f0[j+i];
      } else if(i==0){
        mediana[0]=0;
        mediana[1]=f0[0];
        mediana[2]=f0[1];
      } else if(i==f0.size()){
        mediana[0]=f0[f0.size()-1];
        mediana[1]=f0[f0.size()];
        mediana[2]=0;
      }
    }
    for (int m=0; m<3-1; m++){
      for (int n=m+1; n<3; n++){
        if(mediana[m]>mediana[n]){
          aux = mediana[n];
          mediana[n] = mediana[m];
          mediana[m] = aux;
        }
      }
    }
    f0[i]=mediana[1];
}

```

**La imagen que se muestra a continuación es la del pitch de una señal cualquiera después de aplicar el filtro de mediana (el pitch de la original se encuentra en un apartado anterior de la práctica). Se puede apreciar que se nos ha corregido algún GPE:**

![Pitch_mediana](https://github.com/ikergf/P3/blob/Garcia-Pons/IMG_P3/Pitc_bueno.PNG)

**Por último, se muestran las tablas de error y el *score* total del sistema con las nuevas mejoras añadidas respecto a la anterior versión:**

![Eva_final](https://github.com/ikergf/P3/blob/Garcia-Pons/IMG_P3/Evaluaci%C3%B3n_final.PNG)

**Vemos que el resultado ha mejorado bastante, por lo que concluimos que las mejoras introducidas han sido de ayuda en la optimización del detector.**


Evaluación *ciega* del detector
-------------------------------

Antes de realizar el *pull request* debe asegurarse de que su repositorio contiene los ficheros necesarios
para compilar los programas correctamente ejecutando `make release`.

Con los ejecutables construidos de esta manera, los profesores de la asignatura procederán a evaluar el
detector con la parte de test de la base de datos (desconocida para los alumnos). Una parte importante de
la nota de la práctica recaerá en el resultado de esta evaluación.
