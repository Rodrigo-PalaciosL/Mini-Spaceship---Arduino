//Melodia de la canción
#pragma once
#include "notas.h"

/*
//OPCION 1
struct Nota {
  int frecuencia;
  int duracion;
};

const Nota melodia[] = {
                    {SOL4, 400}, {LA4, 200}, {SOL4, 400}, {MI4, 800},
                    {SOL4, 400}, {LA4, 200}, {SOL4, 400}, {MI4, 800},
                    {RE5, 800}, {RE5, 400), (SI4, 800),
                    {DO5, 800}, {SOL5, 400}, {SOL4, 800},
                    {LA4, 800}, {LA4, 400}, {DO5, 400},
                    {SI4, 200}, {LA4, 400}, {SOL4, 400}
};
*/

//OPCION 2
const int melodia [][2]= {{SOL4, 400}, {LA4, 200}, {SOL4, 400}, {MI4, 800},
                    {SOL4, 400}, {LA4, 200}, {SOL4, 400}, {MI4, 800},
                    {RE5, 800}, {RE5, 400}, {SI4, 800},
                    {DO5, 800}, {SOL5, 400}, {SOL4, 800},
                    {LA4, 800}, {LA4, 400}, {DO5, 400},
                    {SI4, 200}, {LA4, 400}, {SOL4, 400}
                  };

const int numNotas = sizeof(melodia) / sizeof(melodia[0]);