# Procedimento


- Classe top "Fourier" -> input, output, stats
- Classe Iterative, Recursive figlie di Fuorier, override del metodo compute
- Argv e Argc (vittorio)

## Iterative FFT

#### Bit-reversal permutation

$m = log_2(n)$ -> number of bit for the paresentation
Switch to correct position

Indice Decimale,Binario (Originale),Binario (Invertito),Indice Bit-Reversed
0 -> 000 -> 000 -> 0
1 -> 001 -> 100 -> 4
2 -> 010 -> 010 -> 2
3 -> 011 -> 110 -> 6
4 -> 100 -> 001 -> 1
5 -> 101 -> 101 -> 5
6 -> 110 -> 011 -> 3
7 -> 111 -> 111 -> 7

#### Butterfly operations

---

___


## Ricorsive

-> Vittorio

## OpenMP

-> Bianca